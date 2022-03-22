#ifndef SCENE_H
#define SCENE_H
#include "model.h"
#include "light_model.h"
#include "gl_program.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <array>
#include <sstream>
// #include <memory>

namespace scene{
	template<size_t msize, size_t lsize>
	void draw(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view, std::array<model, msize>& models, std::array<light_model, lsize>& lights, GLenum usage){
		for(unsigned i = 0; i < lights.size(); i++){
			light_model &light = lights[i];
			program.assignUniform(gl_program::creatUnifName("light[", i, "].direction").c_str(), glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(light.getDirection())));
			program.assignUniform(gl_program::creatUnifName("light[", i, "].position").c_str(), glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(light.getPosition())));
			program.assignUniform(gl_program::creatUnifName("light[", i, "].color").c_str(), glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(light.getVertexColor())));
			program.assignUniform(gl_program::creatUnifName("light[", i, "].inner_cone").c_str(), glUniform1f, light.getInnerConeAngle());
			program.assignUniform(gl_program::creatUnifName("light[", i, "].outer_cone").c_str(), glUniform1f, light.getOuterConeAngle());
		}
		for(model& m : models){
			m.draw(program, projection, view, usage);
		}
		for(light_model& light : lights){
			light.draw(program, projection, view, usage);
		}
		program.printDebugUniform(0);
	}
};

// // contains different models that are needed to be rendered together into a
// // scene. A scene may contain 0 or more models, as well as 0 or more light
// // model
// class scene{

// 	public:

// 		scene() = default;

// 		~scene(){};

// 		// vector of normal models to be rendered
// 		void loadNormalModels(){};

// 		// vector of light source models to be rendered within the scene
// 		void loadLightModels(){};

// 		// draw all of the specified normal/light model objects within this
// 		// scene object
// 		void draw(){
			
// 			// Light0, Light1, Light2,.. ? in frag shader ? see learnopengl
// 		};

// 	private:

// 		model light;

// 		std::unique_ptr<model[]> models { nullptr };

// 		std::unique_ptr<model[]> lights { nullptr };
// };
#endif // SCENE_H
