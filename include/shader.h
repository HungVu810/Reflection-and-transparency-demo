#ifndef SHADER_H
#define SHADER_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<string>

// #ifndef __glad_h
// #error Glad is not initialized
// #else
// include shader.h at the end of the main.cpp preprocesser macros
enum SHADER_TYPE {VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER};

class shader{
    public:
        shader(SHADER_TYPE shader_type);
        ~shader();
        unsigned int getID() const;
        // get the shader source from path, load it into the given shader name and compile it
        void compileSource(const std::string& path);

    private:
        std::string src;
        unsigned int id;
        SHADER_TYPE type;
        const char* getSource(const std::string& path);
        void checkCompileStatus();
};

#endif // SHADER_H
