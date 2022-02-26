#ifndef GLVBO_H
#define GLVBO_H
#include"./gl_object.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/vec3.hpp>
#include<glm/vec2.hpp>
#include<vector>

struct vertex{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

// vbos manager
class gl_vbo: public gl_object{
    public:
        // ctor and dtor
        gl_vbo();
        ~gl_vbo() noexcept;

        // make the current vertex buffer active
        void bind() const;

        // associate the vbo with the provided buffer and its byte size and
        // usage (glBufferData). The last 3 parameter specify the number of
        // component of the corresponding attributes, which are optional and
        // used to specify the offset and stride for vao faster.
        void loadData(const std::vector<vertex> &vertices, GLenum usage);
    public:
		// vertex attribute data buffer
		std::vector<float> vbuf;
};

#endif // GLVBO_H
