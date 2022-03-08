#ifndef GLVAO_H
#define GLVAO_H
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"./gl_object.h"
#include<vector>

struct face{
	std::vector<unsigned> indices;
};

// wrapper for a VAO
class gl_vao: public gl_object{
    public:
        gl_vao();
        ~gl_vao() noexcept override;

        // active the current vao and its ebo
        void bind() const;
        // load formating data for attrib index i with glVertexAttribPointer
        // and enable that attrib with glEnableVertexAttribArray. The parameter
        // is the same as glVertexAttribPointer
        void attribFormat(unsigned index, unsigned component, GLenum type, bool normalized, size_t stride, const void *offset);
        // load data into the named ebo and its byte size for indexed rendering
		void eboData(const std::vector<unsigned> &ebuf, GLenum usage);
    public:
        // ebo name and number of elements the ebo buffer contains
        unsigned ebo;
};

#endif // GLVAO_H
