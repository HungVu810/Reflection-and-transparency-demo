#version 450 core
in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_tex;

out vec4 frag_color;

struct Material{
	bool use_vertex_color;
	vec4 vertex_color;
};

uniform vec3 time;
uniform Material material;

void main(){
    // light color
	if(material.use_vertex_color)
		frag_color = material.vertex_color;
	else frag_color = vec4(0.5f, 0.5f, 0.5f, 0.0f);
}
