#ifndef GLSHADER_H
#define GLSHADER_H
#include "./gl_object.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

// wrapper of shader object
class gl_shader : public gl_object{

	public:

		// GLenum shader type is one of: GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER
		gl_shader(const std::string &shader_path, bool loadAndCompile = 0);

		~gl_shader();

		// load the shader source from the formated one line cstr from ctor shader_path
		void loadData();

		// compile the loaded shader source and check for the compilation status
		void compile();

		GLenum getType() const;

	private:

		// one line format string version of the source file
		std::string oneline_src {""};

		GLenum type;

		// return the shader file extension from path
		GLenum checkShaderExtension(const std::string &shader_path);

		// convert the source file strings to a c-str
		void srcToCstr(const std::string& path);

		void checkCompileStatus();
};
#endif // GLSHADER_H
