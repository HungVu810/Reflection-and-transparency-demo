#include "../include/gl_program.h"
#include <iostream>
#include <cassert>

gl_program::gl_program() : gl_object(){
    name = glCreateProgram();
};

gl_program::~gl_program(){
    glDeleteProgram(name);
};

void gl_program::attachCompiledShader(const gl_shader *shader){
	assert(shader);
	switch(shader->getType()){
		case GL_VERTEX_SHADER: attachOrSwapShader(0, shader); break;
		// case GL_TESS_CONTROL_SHADER: attachOrSwapShader(1, shader); break;
		// case GL_TESS_EVALUATION_SHADER: attachOrSwapShader(2, shader); break;
		case GL_GEOMETRY_SHADER: attachOrSwapShader(3, shader); break;
		case GL_FRAGMENT_SHADER: attachOrSwapShader(4, shader); break;
		// case GL_COMPUTE_SHADER: attachOrSwapShader(5, shader); break;
	}
	if(print_debug_attached_shaders_id){
		checkAttachedShadersID();
	}
};

void gl_program::link() const{
    glLinkProgram(name);
	checkLinkStatus();
};

void gl_program::use() const{
    glUseProgram(name);
};

// private

void gl_program::checkLinkStatus() const{
    int link_status;
    glGetProgramiv(name, GL_LINK_STATUS, &link_status);
    if(!link_status){
        char infolog[256];
        glGetProgramInfoLog(name, 256, NULL, infolog);
        std::cout << "\nPROGRAM LINK STATUS LOG ======\n" << infolog << std::endl;
        std::terminate();
    }
};

void gl_program::attachOrSwapShader(unsigned shader_stage, const gl_shader *shader){
	// swap with the currently attached shader in the shader_stage
	if(shader_pipeline[shader_stage]){
		if(shader_pipeline[shader_stage]->getID() == shader->getID()){
			return;
		}
		else {
			glDetachShader(name, shader_pipeline[shader_stage]->getID());
		}
	}
	glAttachShader(name, shader->getID());
	link();
	shader_pipeline[shader_stage] = shader;
};

void gl_program::checkAttachedShadersID() const{
	unsigned shaderID[5] {0};
	GLsizei count;
	glGetAttachedShaders(name, 5, &count, shaderID);
	std::cout << "Currently attached shader IDs: ";
	for(size_t i = 0; i < count; i++){
		std::cout << shaderID[i] << (i != count - 1 ? ", " : "");
	}
	std::cout << std::endl;
};
