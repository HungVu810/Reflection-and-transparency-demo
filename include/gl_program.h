#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include "./gl_object.h"
#include "./gl_shader.h"
#include <array>
#include <functional>
#include <cassert>
#include <array>
#include <iostream>
#include <sstream>

#define fval_ptr(arg) const_cast<const float*>(glm::value_ptr(arg))

class gl_program : public gl_object{

	public:

		gl_program();

		~gl_program();

		// attach a compiled shader into 1 of the 6 pipeline shader stage. If
		// there is an already attached shader in the designated pipeline
		// shader stage and it is the same shader, do nothing. Otherwise, the
		// old shader is detached and the stage is attached with the given
		// shader and relink is done automatically 
		void attachCompiledShader(const gl_shader *shader);

		void link() const;

		void use() const;

		// 'uniform_name' is the name of the uniform variable to be assigned.
		// '*gl_uniform_fn' is the function pointer to a glUniform... function.
		// 'Tp... xp' is the packed paramters that contains all arguments
		// beside the first GLint (this function will automatically process
		// that value via glGetUniformLocation).
		// i.e: assignUniform<float>("time", glUniform1f, 1.0f)
		template<typename ... Tp>
		void assignUniform(const char *uniform_name, void (*gl_uniform_fn)(GLint, Tp...), Tp... xp){
			// make sure glLinkProgram and glUseProgram is called before assign uniforms
			int location = glGetUniformLocation(name, uniform_name);
			if(location == -1 && print_debug_unif){
				std::cerr << "the uniform [" << uniform_name << "] is not a valid name" << std::endl;
				// std::exit(EXIT_FAILURE);
			}
			gl_uniform_fn(location, xp...);
		}

		template<typename T>
		static std::string creatUnifName(T arg){
			std::ostringstream sstr;
			sstr << arg;
			return sstr.str();
		}

		template<typename T, typename ...Ptype>
		static std::string creatUnifName(T arg, Ptype ...Pargs){
			std::ostringstream sstr;
			sstr << arg << creatUnifName(Pargs...);
			return sstr.str();
		}

		void printDebugUniform(bool val){
			print_debug_unif = val;
		}

		void printDebugAttachedShadersID(bool val){
			print_debug_attached_shaders_id = val;
		}

	private:

		// manages the currently active shader in each part of the pipeline
		// index 0 is vert
		// index 1 is tesc
		// index 2 is tese
		// index 3 is geom
		// index 4 is frag
		// index 5 is comp
		const gl_shader *shader_pipeline[5] = {nullptr};

		// turn off this value after the first rendering of a scene
		bool print_debug_unif = 0;
		bool print_debug_attached_shaders_id = 0;

		// check the shader stage and attach, do nothing, or swap the current
		// active shader at that stage with the given shader appropriately
		void attachOrSwapShader(unsigned shader_stage, const gl_shader *shader);

		void checkLinkStatus() const;

		void checkAttachedShadersID() const;
};

#endif // GLPROGRAM_H
