#include "../include/gl_shader.h"
#include <iostream>
#include <fstream>
#include <cassert>


// GLenum shader type is one of: GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER
gl_shader::gl_shader(GLenum shaderType) : gl_object(){
	type = shaderType;
	name  = glCreateShader(type);
	switch(type){
		case GL_VERTEX_SHADER: name = glCreateShader(GL_VERTEX_SHADER); break;
		// case GL_TESS_CONTROL_SHADER: name = glCreateShader(GL_TESS_CONTROL_SHADER); break;
		// case GL_TESS_EVALUATION_SHADER: name = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
		case GL_GEOMETRY_SHADER: name = glCreateShader(GL_GEOMETRY_SHADER); break;
		case GL_FRAGMENT_SHADER: name = glCreateShader(GL_FRAGMENT_SHADER); break;
		// case GL_COMPUTE_SHADER: name = glCreateShader(GL_COMPUTE_SHADER); break;
		default:{
			std::cerr << "Unknown GLenum shader type: " << shaderType << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
};

gl_shader::~gl_shader(){
    glDeleteShader(name);
};

// load the shader source from the given path
void gl_shader::loadData(const std::string &path){
    const char* csrc = srcToCstr(path);
    glShaderSource(name, 1, &csrc, nullptr);
}

// compile the loaded shader source and check for the compilation status
void gl_shader::compile(){
	assert(!src.empty());
    glCompileShader(name);
    checkCompileStatus();
}

GLenum gl_shader::getType() const{
	return type;
};

// private

// convert the source file strings to a c-str
const char* gl_shader::srcToCstr(const std::string& path){
    std::fstream file{path};
    assert(file.is_open());
    std::string temp;
    while(getline(file, temp)){
        src += (temp += '\n');
    }
    const char* csrc = src.c_str();
    return csrc;
};

void gl_shader::checkCompileStatus(){
    int compile_status;
    glGetShaderiv(name, GL_COMPILE_STATUS, &compile_status);
    if(!compile_status){
        char infolog[256];
        glGetShaderInfoLog(name, 256, NULL, infolog);
        std::string which_shader;
        switch(type){
            case GL_VERTEX_SHADER: which_shader = "\nVERTEX SHADER LOG ======\n"; break;
            case GL_GEOMETRY_SHADER: which_shader = "\nGEOMETRY SHADER LOG ======\n "; break;
            case GL_FRAGMENT_SHADER: which_shader = "\nFRAGMENT SHADER LOG ======\n "; break;
        }
        std::cout << which_shader << infolog << std::endl;
        std::terminate();
    }
}
