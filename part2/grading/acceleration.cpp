#include "acceleration.h"
#include "object.h"
#include "hit.h"
#include <limits>
#include <algorithm>
#include <unordered_set>
#include <cmath>

extern int acceleration_grid_size;

Acceleration::Acceleration() {
    domain.Make_Empty();
    num_cells.fill(acceleration_grid_size);
}

void Acceleration::Add_Object(const Object* obj, int id) {

    std::pair<Box,bool> bbox=obj->Bounding_Box(-1);

    if(bbox.second){ 
        infinite_objects.push_back({obj,-1,id});
    }
    else{

        if(obj->num_parts>1){ 

            for(int i=0;i<obj->num_parts;i++){
                finite_objects.push_back({obj,i,id});

                Box b=obj->Bounding_Box(i).first;
                domain.Include_Point(b.lo);
                domain.Include_Point(b.hi);
            }
        }
        else{ 

            finite_objects.push_back({obj,-1,id});

            domain.Include_Point(bbox.first.lo);
            domain.Include_Point(bbox.first.hi);
        }
    }
}

ivec3 Acceleration::Cell_Index(const vec3& pt) const {

    ivec3 index;

    for(int i=0;i<3;i++){

        index[i]=(int)std::floor((pt[i]-domain.lo[i])/dx[i]);

        index[i]=std::clamp(index[i],0,num_cells[i]-1);
    }

    return index;
}

void Acceleration::Initialize(){

    cells.resize(num_cells[0]*num_cells[1]*num_cells[2]);

    if(finite_objects.empty()) return;

    vec3 padding=(domain.hi-domain.lo)*1e-6;

    domain.lo-=padding;
    domain.hi+=padding;

    for(int i=0;i<3;i++){

        dx[i]=(domain.hi[i]-domain.lo[i])/num_cells[i];

        dx[i]=std::max(dx[i],1e-9); // avoid zero size cells
    }

    for(const auto& prim:finite_objects){

        Box b=prim.obj->Bounding_Box(prim.part).first;

        ivec3 imin=Cell_Index(b.lo);
        ivec3 imax=Cell_Index(b.hi);

        for(int i=imin[0];i<=imax[0];i++)
        for(int j=imin[1];j<=imax[1];j++)
        for(int k=imin[2];k<=imax[2];k++)
        {
            Cell_Data({i,j,k}).push_back(prim);
        }
    }

    finite_objects.clear();
}

std::pair<int,Hit> Acceleration::Closest_Intersection(const Ray& ray) const{

    Hit closest_hit;
    closest_hit.dist=std::numeric_limits<double>::infinity();

    int closest_id=-1;

    for(const auto& prim:infinite_objects){

        Hit hit=prim.obj->Intersection(ray,prim.part);

        if(hit.dist>small_t && hit.dist<closest_hit.dist){
            closest_hit=hit;
            closest_id=prim.id;
        }
    }

    if(cells.empty()) return {closest_id,closest_hit};

    auto box_hit=domain.Intersection(ray);

    if(!box_hit.first && !domain.Test_Inside(ray.endpoint))
        return {closest_id,closest_hit};

    double t_start=std::max(0.0,box_hit.second);

    vec3 p=ray.Point(t_start);

    ivec3 cell=Cell_Index(p);

    ivec3 step;
    vec3 t_delta;
    vec3 t_next;

    for(int i=0;i<3;i++){

        if(ray.direction[i]>0){

            step[i]=1;

            t_delta[i]=dx[i]/ray.direction[i];

            double boundary=domain.lo[i]+(cell[i]+1)*dx[i];

            t_next[i]=t_start+(boundary-p[i])/ray.direction[i];
        }
        else if(ray.direction[i]<0){

            step[i]=-1;

            t_delta[i]=-dx[i]/ray.direction[i];

            double boundary=domain.lo[i]+cell[i]*dx[i];

            t_next[i]=t_start+(boundary-p[i])/ray.direction[i];
        }
        else{

            step[i]=0;

            t_delta[i]=std::numeric_limits<double>::infinity();

            t_next[i]=std::numeric_limits<double>::infinity();
        }
    }

    std::unordered_set<long long> tested;

    while(cell[0]>=0 && cell[0]<num_cells[0] &&
          cell[1]>=0 && cell[1]<num_cells[1] &&
          cell[2]>=0 && cell[2]<num_cells[2]){

        const auto& list=Cell_Data(cell);

        for(const auto& prim:list){

            long long key=((long long)prim.id<<32)|(prim.part+1);

            if(tested.count(key)) continue;

            tested.insert(key);

            Hit hit=prim.obj->Intersection(ray,prim.part);

            if(hit.dist>small_t && hit.dist<closest_hit.dist){
                closest_hit=hit;
                closest_id=prim.id;
            }
        }

        double next_boundary=std::min({t_next[0],t_next[1],t_next[2]});

        if(closest_hit.dist<next_boundary) break;

        int axis;

        if(t_next[0]<=t_next[1] && t_next[0]<=t_next[2]) axis=0;
        else if(t_next[1]<=t_next[2]) axis=1;
        else axis=2;

        cell[axis]+=step[axis];

        t_next[axis]+=t_delta[axis];
    }

    return {closest_id,closest_hit};
}