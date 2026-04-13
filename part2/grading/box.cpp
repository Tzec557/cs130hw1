// Student Name: Tristan Zhao
// Student ID: 862345701

#include <limits>
#include "box.h"

// Return whether the ray intersects this box.
std::pair<bool,double> Box::Intersection(const Ray& ray) const
{
    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();

    for (int i = 0; i < 3; i++) {
        if (std::abs(ray.direction[i]) < 1e-9) {
            if (ray.endpoint[i] < lo[i] || ray.endpoint[i] > hi[i]) return {false, 0};
        } else {
            double t1 = (lo[i] - ray.endpoint[i]) / ray.direction[i];
            double t2 = (hi[i] - ray.endpoint[i]) / ray.direction[i];
            t_min = std::max(t_min, std::min(t1, t2));
            t_max = std::min(t_max, std::max(t1, t2));
        }
    }

    bool hit = t_max >= t_min && t_max > 0;
    return {hit, t_min};
}

// Compute the smallest box that contains both *this and bb.
Box Box::Union(const Box& bb) const
{
   Box box;
    for(int i=0; i<3; i++) {
        box.lo[i] = std::min(lo[i], bb.lo[i]);
        box.hi[i] = std::max(hi[i], bb.hi[i]);
    }
    return box;
}

// Compute the smallest box that contains both *this and bb.
Box Box::Intersection(const Box& bb) const
{
    Box box;
    for(int i=0; i<3; i++) {
        box.lo[i] = std::max(lo[i], bb.lo[i]);
        box.hi[i] = std::min(hi[i], bb.hi[i]);
    }
    return box;
}

// Enlarge this box (if necessary) so that pt also lies inside it.
void Box::Include_Point(const vec3& pt)
{
    for(int i=0; i<3; i++) {
        lo[i] = std::min(lo[i], pt[i]);
        hi[i] = std::max(hi[i], pt[i]);
    }
}

// Create a box to which points can be correctly added using Include_Point.
void Box::Make_Empty()
{
    lo.fill(std::numeric_limits<double>::infinity());
    hi=-lo;
}

// Create a box that contains everything.
void Box::Make_Full()
{
    hi.fill(std::numeric_limits<double>::infinity());
    lo=-hi;
}

bool Box::Test_Inside(const vec3& pt) const
{
    for(int i=0;i<3;i++)
        if(pt[i]<lo[i] || pt[i]>hi[i])
            return false;
    return true;
}

// Useful for debugging
std::ostream& operator<<(std::ostream& o, const Box& b)
{
    return o << "(lo: " << b.lo << "; hi: " << b.hi << ")";
}
