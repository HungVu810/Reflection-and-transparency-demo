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
		gl_shader(GLenum shaderType);

		~gl_shader();

		// load the given shader source from 'path'
		void loadData(const std::string& path);

		// compile the loaded shader source and check for the compilation status
		void compile();

		GLenum getType() const;

	private:

		std::string src;

		GLenum type;

		// convert the source file strings to a c-str
		const char* srcToCstr(const std::string& path);

		void checkCompileStatus();
};
#endif // GLSHADER_H
