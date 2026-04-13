#include "light.h"
#include "parse.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world, const Ray& ray, const Hit& hit,
    const vec3& intersection_point, const vec3& normal, int recursion_depth) const
{
    // 1. Ambient Component
    // render_world.ambient_color is a pointer to a Color object
    vec3 color;
    if (render_world.ambient_color && color_ambient) {
        color = color_ambient->Get_Color(hit.uv) * render_world.ambient_color->Get_Color(hit.uv) * render_world.ambient_intensity;
    }

    for (const auto* light : render_world.lights) {
        vec3 light_vec = light->position - intersection_point;
        vec3 light_dir = light_vec.normalized();
        double dist_to_light = light_vec.magnitude();

        // 2. Shadows
        if (render_world.enable_shadows) {
            // Offset the ray by a small epsilon along the normal to prevent self-intersection
            Ray shadow_ray(intersection_point + normal * 1e-4, light_dir);
            
            // Closest_Intersection returns std::pair<Shaded_Object, Hit>
            std::pair<Shaded_Object, Hit> result = render_world.Closest_Intersection(shadow_ray);
            
            // Check if result.first.object is not null (meaning something was hit)
            // and if that hit is between the surface and the light source
            if (result.first.object && result.second.dist < dist_to_light) {
                continue;
            }
        }

        // 3. Diffuse Component
        double cos_theta = std::max(0.0, dot(normal, light_dir));
        vec3 emitted_light = light->Emitted_Light(light_vec); 
        vec3 diffuse = color_diffuse->Get_Color(hit.uv) * emitted_light * cos_theta;

        // 4. Specular Component
        vec3 view_dir = -ray.direction.normalized();
        vec3 reflection = (2.0 * dot(normal, light_dir) * normal - light_dir).normalized();
        double cos_alpha = std::max(0.0, dot(reflection, view_dir));
        vec3 specular = color_specular->Get_Color(hit.uv) * emitted_light * std::pow(cos_alpha, specular_power);

        color += diffuse + specular;
    }

    return color;
}