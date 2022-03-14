#include "../include/gl_vbo.h"
#include "../include/runtime_except.h"
#include <cassert>

gl_vbo::gl_vbo() : gl_object(){
    glGenBuffers(1, &name);
}

gl_vbo::~gl_vbo() noexcept{
    glDeleteBuffers(1, &name);
}

void gl_vbo::bind() const{
	glBindBuffer(GL_ARRAY_BUFFER, name);
	// GL_ARRAY_BUFFER not GL_VERTEX_ARRAY
}

void gl_vbo::loadData(const std::vector<float> &vbuf, GLenum usage){
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbuf.size(), vbuf.data(), usage);
}
