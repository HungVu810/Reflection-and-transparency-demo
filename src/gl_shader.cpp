#include<../include/gl_shader.h>
#include<iostream>
#include<fstream>
#include<cassert>

gl_shader::gl_shader(SHADER_TYPE shader_type) : gl_object(){
    type = shader_type;
    switch(type){
        case VERTEX_SHADER: name = glCreateShader(GL_VERTEX_SHADER); break;
        case GEOMETRY_SHADER: name = glCreateShader(GL_GEOMETRY_SHADER); break;
        case FRAGMENT_SHADER: name = glCreateShader(GL_FRAGMENT_SHADER); break;
    };
    attach_status = 0;
};

gl_shader::~gl_shader(){
    glDeleteShader(name);
};

void gl_shader::compileSource(const std::string& path){
    const char* csrc = getSource(path);
    glShaderSource(name, 1, &csrc, nullptr);
    glCompileShader(name);
    checkCompileStatus();
}

void gl_shader::attach(unsigned program){
    glAttachShader(program, name);
    attach_status = 1;
}

void gl_shader::detach(unsigned program){
    glDetachShader(program, name);
    attach_status = 0;
}

bool gl_shader::getAttachStatus() const {
    return attach_status;
}

// private

const char* gl_shader::getSource(const std::string& path){
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
            case VERTEX_SHADER: which_shader = "\nVERTEX SHADER LOG ======\n"; break;
            case GEOMETRY_SHADER: which_shader = "\nGEOMETRY SHADER LOG ======\n "; break;
            case FRAGMENT_SHADER: which_shader = "\nFRAGMENT SHADER LOG ======\n "; break;
        }
        std::cout << which_shader << infolog << std::endl;
        std::terminate();
    }
}
