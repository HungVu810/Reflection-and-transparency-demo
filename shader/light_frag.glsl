#version 450 core
out vec4 frag_color;

uniform vec3 light_color;
uniform vec3 time;

void main(){
    // light color
    frag_color = vec4(light_color, 0.0f);
}
