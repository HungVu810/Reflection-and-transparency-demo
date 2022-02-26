#include"../include/gl_vbo.h"
#include"../include/runtime_except.h"
#include<cassert>

gl_vbo::gl_vbo() : gl_object(){
    glGenBuffers(1, &name);
}

gl_vbo::~gl_vbo() noexcept{
    glDeleteBuffers(1, &name);
}

void gl_vbo::bind() const{
	glBindBuffer(GL_VERTEX_ARRAY, name);
}

void gl_vbo::loadData(const std::vector<vertex> &vertices, GLenum usage){
	// 3 pos, 3 normal, 2 tex
	// GL_FLOAT vbuf[vertices.size() * 8];
	// vbuf index
	// size_t j = 0;

	// posx - posy - posz
	// nomx - nomy - nomz
	// -- texx -- texy --
	for(const vertex &v: vertices){
		vbuf.push_back(v.position.x);
		vbuf.push_back(v.position.y);
		vbuf.push_back(v.position.z);
		vbuf.push_back(v.normal.x);
		vbuf.push_back(v.normal.y);
		vbuf.push_back(v.normal.z);
		vbuf.push_back(v.texcoord.x);
		vbuf.push_back(v.texcoord.y);
	}
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbuf.size(), vbuf.data(), usage);
}
