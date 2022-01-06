#version 450 core
/* layout (binding = 0) uniform sampler2D myTexture; */
in vec2 texture_coord;
in vec3 normal_vec;
in vec3 frag_vec;

out vec4 frag_color;

struct Material{
    sampler2D wood_box; // diffuse map
    sampler2D steel_frame; // specular map
    sampler2D matrix; // ambient map
    float shininess;
    vec3 color;
};

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform float time;
uniform vec3 light_position;
uniform Material material;
uniform Light light;

void main(){
    // make the diffuse curve like a bell chart, there should be a fall off distant if the object move further away to a certain distance then the intensity losened until hit 0
    vec3 frag_to_light = normalize(light_position - frag_vec);
    vec3 reflected_frag_to_light = normalize(reflect(-frag_to_light, normal_vec));
    vec3 frag_to_camera = normalize(-frag_vec);
    vec3 Ambient;

    // factor the texture() and multiply it once ?
    if(texture(material.steel_frame, texture_coord).xyz == vec3(0, 0, 0)){
        Ambient = light.ambient * vec3(texture(material.matrix, vec2(texture_coord.x, texture_coord.y + time)));
        if(Ambient.g >= 0.1f){ // greeness tolerance
            Ambient = vec3(cos(time), sin(time), cos(time));
        }
    }
    else{
        Ambient = light.ambient * vec3(texture(material.steel_frame, texture_coord));
    }
    vec3 Diffuse = light.diffuse * max(dot(frag_to_light, normal_vec), 0.0f) * vec3(texture(material.wood_box, texture_coord));
    vec3 Specular = light.specular * pow(max(dot(frag_to_camera, reflected_frag_to_light), 0.0f), material.shininess) * vec3(texture(material.steel_frame, texture_coord));
    frag_color = vec4((Ambient + Diffuse + Specular), 0.0f);
}
