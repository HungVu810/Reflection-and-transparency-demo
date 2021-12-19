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
#include<array>
#include<memory>
#include<math.h>
#include<../include/shader.h>
#include<../include/runtime_except.h>
#define STB_IMAGE_IMPLEMENTATION
#include<../include/stb_image.h>

void frameBufferSizeCallback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

void loadProgram(unsigned &program, unsigned *shader, const size_t numshader){
    program = glCreateProgram();
    for(size_t i = 0; i < numshader; i++){
        glAttachShader(program, shader[i]);
    }
    glLinkProgram(program);
    int link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if(!link_status){
        char infolog[256];
        glGetProgramInfoLog(program, 256, NULL, infolog);
        std::cout << "\nPROGRAM LINK STATUS LOG ======\n" << infolog << std::endl;
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

    // ebo for indexed drawing
    unsigned int ebo;

    // shader and program
    shader vertexShader{VERTEX_SHADER};
    shader fragShader{FRAGMENT_SHADER};
    unsigned int program;

    // storing data into buffer and enable vao vertex attribute indices
    constexpr unsigned NUM_COMPONENT = 6; // xyz + rgb
    float vertex_data[][NUM_COMPONENT] = {
        {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
        {0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f}
    };
    size_t bufsize = (sizeof(vertex_data) / sizeof(float*)) * NUM_COMPONENT * sizeof(float);

    // format buffer data into vao attribute index
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, bufsize, vertex_data, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, NUM_COMPONENT / 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, NUM_COMPONENT / 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0); // postion format
    glEnableVertexAttribArray(1); // color format

    // load shader and program
    vertexShader.compileSource("/home/hungvu/Archive/progs/opengl/src/vertex.glsl");
    fragShader.compileSource("/home/hungvu/Archive/progs/opengl/src/fragment.glsl");
    unsigned shader[] = {vertexShader.getID(), fragShader.getID()};
    loadProgram(program, shader, sizeof(shader) / sizeof(unsigned));

    // get uniform variable location
    int uniform_time = glGetUniformLocation(program, "time");
    glUseProgram(program);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // rendering loop
    while(!glfwWindowShouldClose(window)){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1f(uniform_time, glfwGetTime());
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteShader(vertexShader.getID());
    glDeleteShader(fragShader.getID());
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
