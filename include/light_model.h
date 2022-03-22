#ifndef LIGHT_MODEL
#define LIGHT_MODEL
#include "gl_shader.h"
#include "model.h"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include <string>
#include <sstream>

class light_model : public model{
	public:
	// 'index' refere to the unsigned number that is used with the 'light'
	// identifier in the fragment shader. For instance, 'light0, light4, ...'

	light_model();

	// copy/assignment stor
	light_model(const light_model &light);
	light_model& operator=(const light_model &light);

	// no moving of light objects
	light_model(light_model &&light) = delete;
	light_model& operator=(light_model &&light) = delete;

	~light_model(){};

	void assignDirection(const glm::vec3& dir);

	void assignOuterConeAngle(float radian);

	void assignInnerConeAngle(float radian);

	// void assignLightColor(const glm::vec4& lcolor){
	// 	light_color = lcolor;
	// };

	glm::vec3 getDirection() const;

	float getOuterConeAngle() const;

	float getInnerConeAngle() const;

	// glm::vec4 getLightColor() const{
	// 	return light_color;
	// }

	private:
		// use the vertex_color for the light_color
		// glm::vec4 light_color{0.5f, 0.5f, 0.5f, 0.0f};
		glm::vec3 direction{0.0f, 0.0f, -1.0f};
		float outer_cone{0.0f};
		float inner_cone{0.0f};
};

#endif
