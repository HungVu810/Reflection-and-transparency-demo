// #define GL_GLEXT_PROTOTYPES
// #include<GL/gl.h>
// #include<GL/glext.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"../include/gl_context.h"
#include"../include/gl_object.h"
#include"../include/gl_vao.h"
#include"../include/gl_vbo.h"
#include"../include/shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include<../include/stb_image.h>
#include<iostream>
#include<cassert>
#include<fstream>
#include<exception>
#include<memory>
#include<math.h>

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

int main(){
    gl_context contx{600, 800, "window"};

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
    constexpr unsigned NUM_COMPONENT = 8; // xyz + rgb + st
    float vertex_data[][NUM_COMPONENT] = {
        {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f}
    };
    unsigned int index[] = {
        0, 1, 2, 0, 3, 2
    };
    size_t bufsize = (sizeof(vertex_data) / sizeof(float*)) * NUM_COMPONENT * sizeof(float);

    // format buffer data into vao attribute index
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, bufsize, vertex_data, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, NUM_COMPONENT - 5, GL_FLOAT, GL_FALSE, NUM_COMPONENT * sizeof(float), (void*)0);
    glVertexAttribPointer(1, NUM_COMPONENT - 5, GL_FLOAT, GL_FALSE, NUM_COMPONENT * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, NUM_COMPONENT - 6, GL_FLOAT, GL_FALSE, NUM_COMPONENT * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0); // position format
    glEnableVertexAttribArray(1); // color format
    glEnableVertexAttribArray(2); // texture format

    // indexed buffer
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_DYNAMIC_DRAW);

    // texture
    unsigned texture;
    int w, h, n;
    unsigned char *data = stbi_load("/home/hungvu/Archive/progs/opengl/texture/brickwall.jpeg", &w, &h, &n, 0);
    assert(data);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load shader and program
    vertexShader.compileSource("/home/hungvu/Archive/progs/opengl/shader/vertex.glsl");
    fragShader.compileSource("/home/hungvu/Archive/progs/opengl/shader/fragment.glsl");
    unsigned shader[] = {vertexShader.getID(), fragShader.getID()};
    loadProgram(program, shader, sizeof(shader) / sizeof(unsigned));

    // get uniform variable location
    int uniform_time = glGetUniformLocation(program, "time");
    glUseProgram(program);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // rendering loop

    while(!glfwWindowShouldClose(contx.getWindow())){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        glUniform1f(uniform_time, glfwGetTime());
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(contx.getWindow());
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
