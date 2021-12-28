#include"../include/gl_object.h"
#include"../include/gl_context.h"
#include"../include/runtime_except.h"
#include<cassert>

gl_object::gl_object(){
    gl_context::st_glObject.push(*this);
}
gl_object::~gl_object(){};

GLuint gl_object::getID() const {
    return name;
}

void gl_object::bind(){};
