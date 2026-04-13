#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    // Calculate the local color from the decorated shader 
    vec3 local_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

    // Base Case: If recursion limit is reached, return only the local component
    if (recursion_depth >= render_world.recursion_depth_limit) {
        return (1.0 - reflectivity) * local_color;
    }

    // Compute Reflection Vector: r = v - 2(v.n)n 
    vec3 v = ray.direction;
    vec3 reflected_direction = (v - 2.0 * dot(v, normal) * normal).normalized();

    // Cast Reflected Ray: Use a small offset (1e-4) to avoid self-intersection 
    Ray reflected_ray(intersection_point + normal * 1e-4, reflected_direction);
    vec3 reflected_color = render_world.Cast_Ray(reflected_ray, recursion_depth + 1);

    // Blend: (1-R)*Local + R*Reflected 
    return (1.0 - reflectivity) * local_color + reflectivity * reflected_color;
}