#include "../include/gl_shader.h"
#include <iostream>
#include <fstream>
#include <cassert>


// GLenum shader type is one of: GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER
gl_shader::gl_shader(const std::string &shader_path, bool loadAndCompile) : gl_object(){
	type = checkShaderExtension(shader_path);
	name = glCreateShader(type);
	srcToCstr(shader_path);
	if(loadAndCompile){
		loadData();
		compile();
	}
};

gl_shader::~gl_shader(){
    glDeleteShader(name);
};

// load the shader source from the formated one line cstr from ctor shader_path
void gl_shader::loadData(){
	assert(oneline_src.size());
	const char *cstr = oneline_src.c_str();
    glShaderSource(name, 1, &cstr, nullptr);
}

// compile the loaded shader source and check for the compilation status
void gl_shader::compile(){
    glCompileShader(name);
    checkCompileStatus();
}

GLenum gl_shader::getType() const{
	return type;
};

// private

GLenum gl_shader::checkShaderExtension(const std::string &shader_path){
	size_t cindex = shader_path.rfind('.');
	std::string fileExt = shader_path.substr(cindex + 1, shader_path.size() - cindex);
	if(fileExt == shader_path){
		std::cerr << "shader_path" << " has invalid file extension (must be one of .vert, .geom or .frag)" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	if(fileExt == "vert") return GL_VERTEX_SHADER;
	// if(fileExt == "tesc") return GL_TESS_CONTROL_SHADER;
	// if(fileExt == "tese") return GL_TESS_EVALUATION_SHADER;
	if(fileExt == "geom") return GL_GEOMETRY_SHADER;
	if(fileExt == "frag") return GL_FRAGMENT_SHADER;
	// if(fileExt == "comp") return GL_COMPUTE_SHADER;
	else {
		std::cerr << "shader_path" << " has invalid file extension (must be one of .vert, .geom or .frag)" << std::endl;
		std::exit(EXIT_FAILURE);
	}
};

// convert the source file strings to a c-str
void gl_shader::srcToCstr(const std::string& path){
    std::fstream file{path};
	if(!file.is_open()){
		std::cerr << path << " can't be read" << std::endl;
		assert(0);
	}
	else{
		std::string temp;
		while(getline(file, temp)){
			oneline_src += (temp += '\n');
		}
	}
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
