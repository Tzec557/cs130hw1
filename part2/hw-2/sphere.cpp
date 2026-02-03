// Student Name: Tristan Zhao
// Student ID: 862345701
#include "sphere.h"
#include "ray.h"

Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    Hit hit;
    hit.dist = -1;
    vec3 L = ray.endpoint - center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0f * dot(ray.direction, L);
    float c = dot(L, L) - radius * radius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant >= 0){

        float sqrt_disc = sqrt(discriminant);
        
        float t0 = (-b - sqrt_disc) / (2.0f * a);
        float t1 = (-b + sqrt_disc) / (2.0f * a);

        if (t0 >= small_t) {
            hit.dist = t0;
        } else if (t1 >= small_t) {
            hit.dist = t1;
        }
    }
    return hit;
}

vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 normal;
    vec3 hit_point = ray.endpoint + ray.direction * hit.dist;
    normal = (hit_point - center);
   //TODO;  compute the normal direction
    return  normal.normalized();;
}

std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}
