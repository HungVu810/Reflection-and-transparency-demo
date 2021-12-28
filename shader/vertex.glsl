#version 450 core
layout (location = 0) in vec3 pos;
/* layout (location = 1) in vec3 color_input; */
layout (location = 1) in vec2 tex_coord;
// out vec3 color;
out vec2 texture_coord;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    // hypotenuse
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    // color = color_input;
    texture_coord = tex_coord;
}
