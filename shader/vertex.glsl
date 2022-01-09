#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex;
out vec2 texture_coord;
out vec3 frag_vec;
out vec3 normal_vec;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    // MAKE SURE THAT THERE ARE NO NON-UNIFORM SCALE, otherwise the normal need to be multiplied with the transpose(invert(model))
    frag_vec = vec3(view * model * vec4(pos, 1.0f));
    // DO NOT APPLY translation on the plane normal
    normal_vec = normalize(vec3(view * model * vec4(normal, 0.0f)));
    texture_coord = tex;
}
