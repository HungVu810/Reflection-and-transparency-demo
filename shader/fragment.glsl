#version 450 core
/* layout(location=1) in vec3 color; */
// in vec3 color;
in vec2 texture_coord;
out vec4 frag_color;

uniform float time;
uniform vec3 object_color;
uniform vec3 light_src;
uniform sampler2D myTexture;

void main(){
    /* frag_color = vec4(color.x, sin(color.y + time), cos(color.z + time), 1.0f); */
    frag_color = texture(myTexture, texture_coord);
}
