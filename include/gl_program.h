#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include "./gl_object.h"
#include "./gl_shader.h"
#include <array>
#include <functional>
#include <cassert>
#include <array>

class gl_program : public gl_object{

	public:

		gl_program();

		~gl_program();

		// attach a compiled shader into 1 of the 6 pipeline shader stage. If
		// there is an already attached shader in the designated pipeline
		// shader stage, the old shader is detached and the stage is attached
		// with the given shader (MUST RELINK THE PROGRAM IF THIS IS THE CASE)
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
			gl_uniform_fn(location, xp...);
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

		void checkLinkStatus() const;

		void attachOrSwapShader(unsigned shader_stage, const gl_shader *shader);
};

#endif // GLPROGRAM_H
