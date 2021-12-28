#ifndef GLVBO_H
#define GLVBO_H
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"./gl_object.h"

// vbos manager
class gl_vbo: public gl_object{
    public:
        // ctor and dtor
        gl_vbo();
        ~gl_vbo() noexcept override;

        // make the current vertex buffer active
        void bind() override;

        // associate the vbo with the provided buffer and its byte size and
        // usage (glBufferData). The last 3 parameter specify the number of
        // component of the corresponding attributes, which are optional and
        // used to specify the offset and stride for vao faster.
        void loadData(const float *buffer, size_t size, GLenum usage, unsigned pos, unsigned color, unsigned texture);

        // auxillary functions for vao indexed attrib's format specification
        unsigned positionCompNum() const;
        unsigned colorCompNum() const;
        unsigned textureCompNum() const;
        size_t stride() const;
        const void* positionOffset() const;
        const void* colorOffset() const;
        const void* textureOffset() const;

    private:
        unsigned pos_comp, color_comp, tex_comp;
};

#endif // GLVBO_H
