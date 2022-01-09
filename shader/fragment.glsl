#version 450 core
/* layout (binding = 0) uniform sampler2D myTexture; */
in vec2 texture_coord;
in vec3 normal_vec;
// viewSpace_procVertex !!!!!!
in vec3 frag_vec;

out vec4 frag_color;

struct Material{
    sampler2D wood_box; // diffuse map
    sampler2D steel_frame; // specular map
    sampler2D matrix; // emission map
    float matrix_emission_strength; // >= 0
    float steel_frame_ambient; // >= 0
    float diffuse_tol; // diffuse tolerance (0, 1)
    float specular_tol; // specular tolerance (0, 1)
    float shininess;
};

uniform float time;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 light_direction;
/* uniform vec3 light_falloff_angle; */
uniform Material material;

void main(){
    // ^ is for integral
    vec3 frag_to_light = normalize(light_position - frag_vec);
    vec3 reflected_frag_to_light = normalize(reflect(-frag_to_light, normal_vec));
    vec3 frag_to_camera = normalize(-frag_vec);
    float norm_frag_to_light = length(light_position - frag_vec);
    float attenuation = 1.0f / (1.0f + 0.22f * norm_frag_to_light + 0.20f * pow(norm_frag_to_light, 2));
    // fall_off 1 = dont apply on texture
    int fall_off = dot(normalize(light_direction), -frag_to_light) > cos(radians(20.0f)) ? 1 : 0;

    vec3 Ambient;
    // factor the texture() and multiply it once ?
    if(texture(material.steel_frame, texture_coord).xyz == vec3(0, 0, 0)){
        Ambient = vec3(texture(material.matrix, vec2(texture_coord.x, texture_coord.y + time)));
        if(Ambient.g >= 0.1f){ // greeness tolerance
            Ambient = material.matrix_emission_strength * vec3(cos(time), sin(time), cos(time));
        }
    }
    else{
        Ambient = material.steel_frame_ambient * vec3(texture(material.steel_frame, texture_coord));
    }
    vec3 Diffuse = light_color * fall_off * material.diffuse_tol * attenuation * max(dot(frag_to_light, normal_vec), 0.0f) * vec3(texture(material.wood_box, texture_coord));
    /* vec3 Specular = 5.0f * light_color * fall_off * material.specular_tol * attenuation * pow(max(dot(frag_to_camera, reflected_frag_to_light), 0.0f), material.shininess) * vec3(texture(material.steel_frame, texture_coord)); */
    frag_color = vec4((Ambient + Diffuse /*+ Specular*/), 0.0f);
}
