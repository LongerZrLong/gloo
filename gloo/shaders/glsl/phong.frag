#version 330 core

out vec4 frag_color;

struct AmbientLight {
    bool enabled;
    vec3 ambient;
};

struct PointLight {
    bool enabled;
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

struct DirectionalLight {
    bool enabled;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 world_position;
in vec3 world_normal;
in vec2 tex_coord;

uniform float BIAS = 0.001f;

uniform vec3 camera_position;

uniform sampler2D ambient_texture;
uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;

uniform bool ambient_texture_flag;
uniform bool diffuse_texture_flag;
uniform bool specular_texture_flag;

uniform bool draw_shadow = false;
uniform sampler2D shadow_texture;
uniform mat4 world_to_light_ndc_matrix;

uniform Material material;// material properties of the object
uniform AmbientLight ambient_light;
uniform PointLight point_light;
uniform DirectionalLight directional_light;
vec3 CalcAmbientLight();
vec3 CalcPointLight(vec3 normal, vec3 view_dir);
vec3 CalcDirectionalLight(vec3 normal, vec3 view_dir);

void main() {
    vec3 normal = normalize(world_normal);
    vec3 view_dir = normalize(camera_position - world_position);

    frag_color = vec4(0.0);

    if (ambient_light.enabled) {
        frag_color += vec4(CalcAmbientLight(), 1.0);
    }

    if (point_light.enabled) {
        frag_color += vec4(CalcPointLight(normal, view_dir), 1.0);
    }

    if (directional_light.enabled) {
        frag_color += vec4(CalcDirectionalLight(normal, view_dir), 1.0);
    }
}

vec3 GetAmbientColor() {
    if (ambient_texture_flag) {
        return texture(ambient_texture, tex_coord).xyz;
    } else {
        return texture(ambient_texture, tex_coord).xyz;
    }
}

vec3 GetDiffuseColor() {
    if (diffuse_texture_flag) {
        return texture(diffuse_texture, tex_coord).xyz;
    } else {
        return material.diffuse;
    }
}

vec3 GetSpecularColor() {
    if (specular_texture_flag) {
        return texture(specular_texture, tex_coord).xyz;
    } else {
        return material.specular;
    }
}

vec3 CalcAmbientLight() {
    return ambient_light.ambient * GetAmbientColor();
}

vec3 CalcPointLight(vec3 normal, vec3 view_dir) {
    PointLight light = point_light;
    vec3 light_dir = normalize(light.position - world_position);

    float diffuse_intensity = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_color = diffuse_intensity * light.diffuse * GetDiffuseColor();

    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_intensity = pow(
    max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = specular_intensity *
    light.specular * GetSpecularColor();

    float distance = length(light.position - world_position);
    float attenuation = 1.0 / (light.attenuation.x +
    light.attenuation.y * distance +
    light.attenuation.z * (distance * distance));

    vec3 final_color = attenuation * (diffuse_color + specular_color);

    if (draw_shadow){
        vec4 point_ndc = world_to_light_ndc_matrix * vec4(world_position, 1.0);

        float this_depth = (point_ndc.z + 1.0f)/2.0f;

        vec2 point_shadow_tex = vec2((point_ndc.x + 1.0f)/2.0f, (point_ndc.y + 1.0f)/2.0f);

        float occluder_depth = texture(shadow_texture, point_shadow_tex).r;
        if (occluder_depth + 0.01 < this_depth) {
            final_color = 0.5f*final_color;
        }
    }

    return final_color;
}

vec3 CalcDirectionalLight(vec3 normal, vec3 view_dir) {

    DirectionalLight light = directional_light;
    vec3 light_dir = normalize(-light.direction);
    float diffuse_intensity = max(dot(normal, light_dir), 0.0);
    vec3 diffuse_color = diffuse_intensity * light.diffuse * GetDiffuseColor();

    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_intensity = pow(
    max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular_color = specular_intensity *
    light.specular * GetSpecularColor();

    vec3 final_color = diffuse_color + specular_color;

    if (draw_shadow){
        vec4 point_ndc = world_to_light_ndc_matrix * vec4(world_position, 1.0);

        float this_depth = (point_ndc.z + 1.0f)/2.0f;

        vec2 point_shadow_tex = vec2((point_ndc.x + 1.0f)/2.0f, (point_ndc.y + 1.0f)/2.0f);

        float occluder_depth = texture(shadow_texture, point_shadow_tex).r;
        if (occluder_depth + 0.01 < this_depth) {
            final_color = 0.5f*final_color;
        }
    }
    return final_color;
}



