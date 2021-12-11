#version 450 core
/* layout(location=1) in vec3 color; */
out vec4 frag_color;

void main(){
    frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
