#ifndef GLVBO_H
#define GLVBO_H
#include"./gl_object.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/vec3.hpp>
#include<glm/vec2.hpp>
#include<vector>

// struct vertex{
// 	glm::vec3 position;
// 	glm::vec3 normal;
// 	glm::vec2 texcoord;
// };

// vbos manager
class gl_vbo: public gl_object{
    public:
        // ctor and dtor
        gl_vbo();
        ~gl_vbo() noexcept;

        // make the current vertex buffer active
        void bind() const;
        void loadData(const std::vector<float> &vbuf, GLenum usage);
    private:
};

#endif // GLVBO_H
