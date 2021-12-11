// #define GL_GLEXT_PROTOTYPES
// #include<GL/gl.h>
// #include<GL/glext.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
// #include<cglm/>
#include<iostream>
#include<cassert>
#include<fstream>
#include<exception>

void frameBufferSizeCallback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

void runtimeError() noexcept{
    try{
        throw;
    }
    catch(const std::exception& e){
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

// get the shader source from path, load it into the given shader name and compile it
void loadShader(const std::string& path, unsigned shader){
    std::fstream file{path};
    assert(file.is_open());
    std::string temp, ssrc;
    while(getline(file, temp)){
        ssrc += temp;
    }
    const char* src = ssrc.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if(compile_status){
        char infolog[256];
        glGetShaderInfoLog(shader, 256, NULL, infolog);
        std::cout << infolog << std::endl;
        std::terminate();
    }
}

void loadProgram(unsigned &program, unsigned *shader, const size_t numshader){
    program = glCreateProgram();
    for(size_t i = 0; i < numshader; i++){
        glAttachShader(program, shader[i]);
    }
    glLinkProgram(program);
    int link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if(link_status){
        char infolog[256];
        glGetProgramInfoLog(program, 256, NULL, infolog);
        std::cout << infolog << std::endl;
        std::terminate();
    }
}

GLFWwindow* initOpenGL(){
    // init opengl context
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "window", nullptr, nullptr);
    if(!window){
        std::cout << "Can not create window" << std::endl;
        std::terminate();
    }
    // associate current opengl with the window
    glfwMakeContextCurrent(window);
    // expose opengl api
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to init glad" << std::endl;
        std::terminate();
    }
    // set viewport and callback function when viewport is changed
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    return window;
}

void cleanUp(){
    // glDetachShader();
}

int main(){
    GLFWwindow *window{initOpenGL()};

    // triangle
    unsigned int vao;

    // triangle pos, triangle color
    unsigned int vbo;

    // shader and program
    unsigned int vertexShader, fragShader, program;

    // storing data into buffer and enable vao vertex attribute indices
    const float triangle[]={
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // load shader and program
    loadShader("/home/hungvu/Archive/progs/opengl/src/vertex.glsl", vertexShader);
    loadShader("/home/hungvu/Archive/progs/opengl/src/fragment.glsl", fragShader);
    unsigned shader[] = {vertexShader, fragShader};
    loadProgram(program, shader, sizeof(shader)/sizeof(unsigned));
    glUseProgram(program);

    // rendering loop
    while(!glfwWindowShouldClose(window)){
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanUp();
    glfwTerminate();
    return 0;
}
