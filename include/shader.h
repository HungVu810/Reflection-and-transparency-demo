#ifndef SHADER_H
#define SHADER_H

// #ifndef __glad_h
// #error Glad is not initialized
// #else
// include shader.h at the end of the main.cpp preprocesser macros
enum SHADER_TYPE {VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER};

class shader{
    public:
        shader(SHADER_TYPE shader_type);
        ~shader();
        unsigned int getID() const{
            return id;
        }
        // get the shader source from path, load it into the given shader name and compile it
        void compileSource(const std::string& path) {
            const char* csrc = getSource(path);
            glShaderSource(id, 1, &csrc, nullptr);
            glCompileShader(id);
            checkCompileStatus();
        }
    private:
        std::string src;
        unsigned int id;
        SHADER_TYPE type;
        const char* getSource(const std::string& path);
        void checkCompileStatus();
};

shader::shader(SHADER_TYPE shader_type){
    type = shader_type;
    switch(type){
        case VERTEX_SHADER: id = glCreateShader(GL_VERTEX_SHADER); break;
        case GEOMETRY_SHADER: id = glCreateShader(GL_GEOMETRY_SHADER); break;
        case FRAGMENT_SHADER: id = glCreateShader(GL_FRAGMENT_SHADER); break;
    };
};
shader::~shader(){
    glDeleteShader(id);
};
const char* shader::getSource(const std::string& path){
    std::fstream file{path};
    assert(file.is_open());
    std::string temp;
    while(getline(file, temp)){
        src += (temp += '\n');
    }
    const char* csrc = src.c_str();
    return csrc;
};
void shader::checkCompileStatus(){
    int compile_status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status);
    if(!compile_status){
        char infolog[256];
        glGetShaderInfoLog(id, 256, NULL, infolog);
        std::string which_shader;
        switch(type){
            case VERTEX_SHADER: which_shader = "\nVERTEX SHADER LOG ======\n"; break;
            case GEOMETRY_SHADER: which_shader = "\nGEOMETRY SHADER LOG ======\n "; break;
            case FRAGMENT_SHADER: which_shader = "\nFRAGMENT SHADER LOG ======\n "; break;
        }
        std::cout << which_shader << infolog << std::endl;
        std::terminate();
    }
}
// #endif

#endif // SHADER_H
