// Student Name: Tristan Zhao
// Student ID: 862345701
#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>

Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

// Intersect with the plane.  The plane's normal points outside.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    // TODO;
    Hit hit;
    hit.dist = -1;

    float denominator = dot(ray.direction, normal);
   if (std::abs(denominator) > 1e-6){
        float t = dot(x - ray.endpoint, normal) / denominator;

        if (t >= small_t)
        {
            hit.dist = t;
        }
    }
    return hit;
}

vec3 Plane::Normal(const Ray& ray, const Hit& hit) const
{
    return normal;
}

std::pair<Box,bool> Plane::Bounding_Box(int part) const
{
    Box b;
    b.Make_Full();
    return {b,true};
}
