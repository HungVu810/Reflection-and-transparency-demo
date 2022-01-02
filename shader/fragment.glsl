#version 450 core
in vec2 texture_coord;
in vec3 normal_vec;
in vec3 frag_vec;

out vec4 frag_color;

uniform float time;
uniform vec3 object_color;
uniform vec3 white_light;
/* uniform vec3 look_at; */
uniform vec3 light_position;
uniform sampler2D myTexture;

void main(){
    // make the diffuse curve like a bell chart, there should be a fall off distant if the object move further away to a certain distance then the intensity losened until hit 0
    vec3 frag_to_light = normalize(light_position - frag_vec);
    vec3 reflected_frag_to_light = -reflect(frag_to_light, normal_vec);
    vec3 frag_to_camera = -frag_vec;
    float ambientConstant = 0.5f; 
    float specularStrength = 0.1f;

    vec3 Ambient = ambientConstant * white_light;
    vec3 Diffuse = max(dot(frag_to_light, normal_vec), 0.0f) * white_light;
    vec3 Specular = specularStrength * pow(max(dot(reflected_frag_to_light, frag_to_camera), 0.0f), 2) * white_light;
    frag_color = vec4((Ambient + Diffuse + Specular) * object_color, 0.0f);
}
