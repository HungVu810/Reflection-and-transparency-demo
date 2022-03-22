#ifndef GLOBJECT_H
#define GLOBJECT_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>

class gl_object{
    public:
        gl_object();

		// 2 objects can't share the same name id
		// gl_object(const gl_object &obj) = delete;
		// gl_object& operator=(const gl_object &&obj) = delete;

		// one object can't inherit the properties of another object
		// gl_object(gl_object &&obj) = delete;
		// gl_object& operator=(gl_object &&obj) = delete;

        // can't be pure because of st_glObject will fail to instantiate gl_object abstract base class
        // keep virtual for the derived classes override it with their own glDelete function in their dtor when st_glObject pop polymorphic derive gl_object
        virtual ~gl_object();

        // bind object and make it active
        virtual void bind();

        GLuint getID() const;

    protected:
        GLuint name;
};

#endif // GLOBJECT_H
