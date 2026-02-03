// Student Name: Tristan Zhao
// Student ID: 862345701
#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    float min_t = std::numeric_limits<float>::max();
    std::pair<Shaded_Object, Hit> closest;
    closest.first.object = nullptr; 
    closest.second.dist = min_t;

    for (const auto& obj : objects){
        Hit hit = obj.object->Intersection(ray, -1); 

        if (hit.dist >= small_t && hit.dist < min_t){
            min_t = hit.dist;
            closest.first = obj;  
            closest.second = hit; 
        }
    }

    return closest;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    TODO; // set up the initial view ray here
    vec3 ray_origin = camera.position;
    vec3 world_pos = camera.World_Position(pixel_index);
    vec3 ray_direction = (world_pos - ray_origin).normalized();
    Ray ray(ray_origin, ray_direction);

    vec3 color = Cast_Ray(ray, 1);
    camera.Set_Pixel(pixel_index, Pixel_Color(color));
}
void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
    vec3 color;
    std::pair<Shaded_Object, Hit> closest = Closest_Intersection(ray);
    if (closest.first.object)
    {
        // Intersection point: P = O + tD
        vec3 hit_point = ray.Point(closest.second.dist);
        
        vec3 normal = closest.first.object->Normal(ray, closest.second);

        return closest.first.shader->Shade_Surface(
            *this, ray, closest.second, hit_point, normal, recursion_depth);
    }

    if (background_shader)
    {
        return background_shader->Shade_Surface(*this, ray, {}, {}, {}, recursion_depth);
    }

    return vec3(0, 0, 0);
}
