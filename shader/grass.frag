#version 450 core
in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_tex;
out vec4 frag_color;

struct Material{
	sampler2D diffuse0;
};

uniform Material material;

void main(){
	// check frag_tex of the brickwall ?
	vec4 color = texture(material.diffuse0, frag_tex);
	// check for the texile color's alpha value
	if(color.a < 0.1f){
		discard;
	}
	/* else if(color.r == 1.0f){ */
	/* 	color.r = 0.5f; */
	/* } */
	frag_color = color;
}
