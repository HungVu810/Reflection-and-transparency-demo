#ifndef GLVAO_H
#define GLVAO_H
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"./gl_object.h"

// wrapper for a VAO
class gl_vao: public gl_object{
    public:
        gl_vao();

        ~gl_vao() noexcept;

        // active the current vao and its ebo
        void bind() const;

        // load formating data for attrib index i with glVertexAttribPointer
        // and enable that attrib with glEnableVertexAttribArray. The parameter
        // is the same as glVertexAttribPointer
        void attribData(unsigned index, unsigned component, GLenum type, bool normalized, size_t stride, const void *offset);

        // load data into the named ebo and its byte size for indexed rendering
        void eboData(const unsigned* index_buffer, size_t size, GLenum usage);

        unsigned eboNumIndices() const;

    private:
        // ebo name and number of elements the ebo buffer contains
        unsigned ebo, indices;
};

#endif // GLVAO_H
