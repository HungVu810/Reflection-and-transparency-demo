#ifndef GLSHADER_H
#define GLSHADER_H

#include"./gl_object.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<string>

// #ifndef __glad_h
// #error Glad is not initialized
// #else
// include shader.h at the end of the main.cpp preprocesser macros
enum SHADER_TYPE {VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER};

// wrapper of shader object
class gl_shader : public gl_object{
    public:
        gl_shader(SHADER_TYPE shader_type);
        ~gl_shader();

        // get the shader source from path, load it into the given shader name and compile it
        void compileSource(const std::string& path);

    private:
        std::string src;
        unsigned int id;
        SHADER_TYPE type;
        const char* getSource(const std::string& path);
        void checkCompileStatus();
};

#endif // GLSHADER_H
