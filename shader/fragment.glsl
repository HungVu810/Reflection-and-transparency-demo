#version 450 core
in vec2 texture_coord;
in vec3 frag_position;
in vec3 normal_vec;
out vec4 frag_color;

uniform float time;
uniform vec3 object_color;
uniform vec3 white_light;
uniform vec3 light_position;
uniform sampler2D myTexture;

void main(){
    // make the diffuse curve like a bell chart, there should be a fall off distant if the object move further away to a certain distance then the intensity losened until hit 0
    vec4 Ambient = 0.5f * vec4(white_light, 0.0f);
    vec4 Diffuse = max(dot(normalize(light_position - frag_position), normal_vec), 0.0f) * vec4(white_light, 0.0f);
    frag_color = (Ambient + Diffuse) * vec4(object_color, 0.0f);
}
