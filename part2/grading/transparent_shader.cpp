#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Transparent_Shader::
Transparent_Shader(const Parse* parse,std::istream& in)
{
    in>>name>>index_of_refraction>>opacity;
    shader=parse->Get_Shader(in);
    assert(index_of_refraction>=1.0);
}

// Use opacity to determine the contribution of this->shader and the Schlick
// approximation to compute the reflectivity.  This routine shades transparent
// objects such as glass.  Note that the incoming and outgoing indices of
// refraction depend on whether the ray is entering the object or leaving it.
// You may assume that the object is surrounded by air with index of refraction
// 1.
vec3 Transparent_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    // Calculate the local color component 
    vec3 local_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);

    if (recursion_depth >= render_world.recursion_depth_limit) {
        return opacity * local_color;
    }

    // Determine Indices of Refraction (assuming Air = 1.0) [cite: 3, 4]
    double n1 = 1.0;
    double n2 = index_of_refraction;
    vec3 n = normal;
    vec3 v = ray.direction.normalized();
    double cos_theta_i = -dot(v, n);

    // If cos_theta_i < 0, the ray is hitting the surface from the inside 
    if (cos_theta_i < 0) {
        std::swap(n1, n2);
        n = -normal;
        cos_theta_i = -dot(v, n);
    }

    // Schlick Approximation for Reflectivity 
    double r0 = std::pow((n1 - n2) / (n1 + n2), 2);
    double R_theta = r0 + (1.0 - r0) * std::pow(1.0 - cos_theta_i, 5);

    // Calculate Reflection Component 
    vec3 refl_dir = (v - 2.0 * dot(v, n) * n).normalized();
    Ray refl_ray(intersection_point + n * 1e-4, refl_dir);
    vec3 reflected_color = render_world.Cast_Ray(refl_ray, recursion_depth + 1);

    // Calculate Refraction Component 
    double ratio = n1 / n2;
    double sin2_theta_t = ratio * ratio * (1.0 - cos_theta_i * cos_theta_i);
    
    vec3 transmitted_color;
    if (sin2_theta_t > 1.0) {
        // Total Internal Reflection: All light reflects 
        transmitted_color = reflected_color;
    } else {
        double cos_theta_t = std::sqrt(1.0 - sin2_theta_t);
        vec3 refr_dir = (ratio * v + (ratio * cos_theta_i - cos_theta_t) * n).normalized();
        Ray refr_ray(intersection_point - n * 1e-4, refr_dir);
        vec3 refracted_color = render_world.Cast_Ray(refr_ray, recursion_depth + 1);
        
        // Blend reflection and refraction via Schlick 
        transmitted_color = R_theta * reflected_color + (1.0 - R_theta) * refracted_color;
    }

    //blend using opacity 
    return opacity * local_color + (1.0 - opacity) * transmitted_color;
}