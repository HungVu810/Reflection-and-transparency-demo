#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include"./gl_object.h"
#include"./gl_shader.h"
#include<array>
#include<functional>
#include<cassert>

class gl_program : public gl_object{
    public:
        gl_program();

        ~gl_program();

        // attach shaders, link and use program
        void loadCompiledShaders(gl_shader** shaders, size_t size);

        void link();

        void use();

        void changeShader(gl_shader &old_shader, gl_shader &new_shader);

        // assign uniform variable, the first parameter for gl_uniform_fn (int
        // location get with glGetUniformLocation) can be skipped, variadic the
        // template type and the parameters follow the gl_uniform_fn pointer
        // are after the location arguments, for instance
        // assignUniform<float>("time", glUniform1f, 1.0f);
        template<typename ... Tp>
        void assignUniform(const char *uniform_name, void (*gl_uniform_fn)(GLint, Tp...), Tp... xp){
            // make sure glLinkProgram and glUseProgram is called before assign uniforms
            int location = glGetUniformLocation(name, uniform_name);
            gl_uniform_fn(location, xp...);
        }
        // seperate into 2 functions, static (once) and dynamic assign unform (different var each time)
        // use std::function better or keep normal c-function pointer?

    private:
        void checkLinkStatus();
};

#endif // GLPROGRAM_H
