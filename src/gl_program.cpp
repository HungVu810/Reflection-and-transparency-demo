#include"../include/gl_program.h"
#include<iostream>

gl_program::gl_program() : gl_object(){
    name = glCreateProgram();
}

gl_program::~gl_program(){
    glDeleteProgram(name);
}

void gl_program::link(){
    glLinkProgram(name);
}

void gl_program::use(){
    glUseProgram(name);
}

void gl_program::changeShader(unsigned old_shader_name, unsigned new_shader_name){
    glDetachShader(name, old_shader_name);
    glAttachShader(name, new_shader_name);
    glLinkProgram(name);
}

void gl_program::checkLinkStatus(){
    int link_status;
    glGetProgramiv(name, GL_LINK_STATUS, &link_status);
    if(!link_status){
        char infolog[256];
        glGetProgramInfoLog(name, 256, NULL, infolog);
        std::cout << "\nPROGRAM LINK STATUS LOG ======\n" << infolog << std::endl;
        std::terminate();
    }
}
