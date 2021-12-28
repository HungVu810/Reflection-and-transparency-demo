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

void gl_vbo::loadData(const float *buffer, size_t size, GLenum usage, unsigned pos = 0, unsigned color = 0, unsigned texture = 0){
    glBufferData(GL_ARRAY_BUFFER, size, buffer, usage);
    // the ordering of the buffer vertex attribs data is: pos, color, then texture
    pos_comp = pos;
    color_comp = color;
    tex_comp = texture;
}

unsigned gl_vbo::positionCompNum() const {
    assert(pos_comp);
    return pos_comp;
}
unsigned gl_vbo::colorCompNum() const {
    assert(color_comp);
    return color_comp;
}
unsigned gl_vbo::textureCompNum() const {
    assert(tex_comp);
    return tex_comp;
}
size_t gl_vbo::stride() const {
    assert(pos_comp);
    return (pos_comp + color_comp + tex_comp) * sizeof(GLfloat);
}
const void* gl_vbo::positionOffset() const {
    assert(pos_comp);
    return (void*)0;
}
const void* gl_vbo::colorOffset() const {
    assert(color_comp);
    return (void*)(pos_comp * sizeof(float));
}
const void* gl_vbo::textureOffset() const {
    assert(tex_comp);
    return (void*)((pos_comp + color_comp) * sizeof(float));
}
