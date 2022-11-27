#version 450 core
in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_tex;

out vec4 frag_color;

uniform vec4 vertex_color;
uniform float time;

void main(){
	frag_color = vertex_color;
}
