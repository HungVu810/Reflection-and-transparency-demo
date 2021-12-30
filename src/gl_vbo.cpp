#include"../include/gl_vbo.h"
#include"../include/runtime_except.h"
#include<cassert>

gl_vbo::gl_vbo() : gl_object(){
    glGenBuffers(1, &name);
}

gl_vbo::~gl_vbo() noexcept{
    glDeleteBuffers(1, &name);
}

void gl_vbo::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, name);
}

void gl_vbo::loadData(const float *buffer, size_t size, GLenum usage, unsigned attribOne = 0, unsigned attribTwo = 0, unsigned attribThree = 0){
    glBufferData(GL_ARRAY_BUFFER, size, buffer, usage);
    // the ordering of the buffer vertex attribs data is: attribOne, attribTwo, then attribThree
    attribOne_comp = attribOne;
    attribTwo_comp = attribTwo;
    attribThree_comp = attribThree;
}

unsigned gl_vbo::attribOneCompNum() const {
    assert(attribOne_comp);
    return attribOne_comp;
}
unsigned gl_vbo::attribTwoCompNum() const {
    assert(attribTwo_comp);
    return attribTwo_comp;
}
unsigned gl_vbo::attribThreeCompNum() const {
    assert(attribThree_comp);
    return attribThree_comp;
}
size_t gl_vbo::stride() const {
    assert(attribOne_comp);
    return (attribOne_comp + attribTwo_comp + attribThree_comp) * sizeof(GLfloat);
}
const void* gl_vbo::attribOneOffset() const {
    assert(attribOne_comp);
    return (void*)0;
}
const void* gl_vbo::attribTwoOffset() const {
    assert(attribTwo_comp);
    return (void*)(attribOne_comp * sizeof(float));
}
const void* gl_vbo::attribThreeOffset() const {
    assert(attribThree_comp);
    return (void*)((attribOne_comp + attribTwo_comp) * sizeof(float));
}
