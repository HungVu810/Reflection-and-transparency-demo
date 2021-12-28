#ifndef GLOBJECT_H
#define GLOBJECT_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>

class gl_object{
    public:
        gl_object();

        // can't be pure because of st_glObject will fail to instantiate gl_object abstract base class
        // keep virtual for the derived classes override it with their own glDelete function in their dtor when st_glObject pop polymorphic derive gl_object
        virtual ~gl_object();

        // bind object and make it active
        virtual void bind();

        GLuint getID() const;

    protected:
        GLuint name;
};

// class gl_ebo: public gl_object{
//     private:
//     public:
// };


// abstract base class object
// derive class: shader, vao, buffer object, program
// buffer object has a print function (custom file output location, print the buffer data) for debugging purpose 
// derive class shader: vertex_shader, frag_shader
// derive class buffer object: vbo, vao

// construct (enum ?)
// dtor (delete object)
// buffer object has a struct that store the attribute index, the stride value and other parameter when associate with an attribute index for a member function (updateBuffer) that allow modifying of component in the buffer with a custome time function inside the that member function parameter
// 2d access type, override operator[][] ? ([i][j] = [i * num_component + j])

// delete, detach (object from program)
// print

#endif // GLOBJECT_H
