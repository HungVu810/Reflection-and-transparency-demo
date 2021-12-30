#include"../include/gl_program.h"
#include<iostream>

gl_program::gl_program() : gl_object(){
    name = glCreateProgram();
}

gl_program::~gl_program(){
    glDeleteProgram(name);
}

void gl_program::loadCompiledShaders(gl_shader** shaders, size_t size){
    assert(size);
    for(size_t i = 0; i < size; i++){
        shaders[i]->attach(name);
    }
    glLinkProgram(name);
    checkLinkStatus();
    glUseProgram(name);
}

void gl_program::link(){
    glLinkProgram(name);
}

void gl_program::use(){
    glUseProgram(name);
}

// remove the old_shader, grap it with glAttachedShader since there can be only 1 active shader each shader binding point
void gl_program::changeShader(gl_shader &old_shader, gl_shader &new_shader){
    if(old_shader.getAttachStatus()){
        old_shader.detach(name);
        new_shader.attach(name);
        glLinkProgram(name);
    }
    else {
        std::cerr << "Can't detach an unattached shader id " << old_shader.getID() << std::endl;
    }
}

// private

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
