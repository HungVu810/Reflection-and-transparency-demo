#version 450 core
/* layout (binding = 0) uniform sampler2D myTexture; */
in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_tex;

out vec4 frag_color;

struct Material{
	// mat_avail = 0 if model does not have any texture, fragment color are assigned with Material::color
	bool use_vertex_color;
	vec4 vertex_color;
	sampler2D ambient0;
	sampler2D ambient1;
	sampler2D ambient2;
	sampler2D diffuse0;
	sampler2D diffuse1;
	sampler2D diffuse2;
	sampler2D specular0;
	sampler2D specular1;
	sampler2D specular2;
    float shininess;
};

struct Light{
	vec3 position;
	vec3 direction;
	float inner_cone;
	float outer_cone;
	vec3 color;
};

uniform float time;
uniform Light light[3];
uniform Material material;

void main(){
    // ^ is for integral
	float blending_factor = 1.0f / light.length();
	float ambient_rescale = 0.25f;
	float final_frag_rescale = 2.0f;
	vec3 frag_to_camera = normalize(-frag_pos);
	frag_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	if(material.use_vertex_color){
		frag_color = material.vertex_color;
	}
	else{
		for(uint i = 0; i < light.length(); i++){
			vec3 frag_to_light = normalize(light[i].position - frag_pos);
			float length_frag_to_light = length(frag_to_light);
			vec3 reflected_frag_to_light = normalize(reflect(-frag_to_light, frag_normal));
			float attenuation = 1.0f / (1.0f + 0.022f * length_frag_to_light + 0.020f * pow(length_frag_to_light, 2));
			float fall_off = clamp((cos(radians(light[i].outer_cone)) - dot(normalize(light[i].direction), -frag_to_light)) / (cos(radians(light[i].outer_cone)) - cos(radians(light[i].inner_cone))), 0.0, 1.0);

			vec3 ambient = ambient_rescale * vec3(texture(material.diffuse0, frag_tex));

			vec3 diffuse = light[i].color * /* fall_off *  */ attenuation * max(dot(frag_to_light, frag_normal), 0.0f) * vec3(texture(material.diffuse0, frag_tex));

			vec3 specular = light[i].color * /* fall_off * */ attenuation * pow(max(dot(frag_to_camera, reflected_frag_to_light), 0.0f), material.shininess) * vec3(texture(material.specular0, frag_tex));

			frag_color +=  blending_factor * vec4(ambient + diffuse + specular, 0.0f);
		}
		frag_color *= final_frag_rescale;
	}
}
