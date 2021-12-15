#version 450 core
/* layout(location=1) in vec3 color; */
out vec4 frag_color;

uniform float time;

void main(){
    frag_color = vec4(cos(time), sin(time), 0.2f, 1.0f);
}
