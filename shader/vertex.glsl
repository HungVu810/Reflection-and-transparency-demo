#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex;
out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_tex;

uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    // MAKE SURE THAT THERE ARE NO NON-UNIFORM SCALE, otherwise the normal need to be multiplied with the transpose(invert(model))
    frag_pos = vec3(view * model * vec4(pos, 1.0f));
    // DO NOT APPLY translation on the plane normal
    frag_normal = normalize(vec3(view * model * vec4(normal, 0.0f)));
    frag_tex = tex;
	// the frag_... (intuiatively, they should be vec_...) but the value will be interpolated per fragment during the rasterization step
}
