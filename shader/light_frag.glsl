#version 450 core
out vec4 frag_color;

uniform vec3 white_light;
void main(){
    // light color
    frag_color = vec4(white_light, 0.0f);
}
