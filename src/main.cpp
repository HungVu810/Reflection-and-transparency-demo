// #define GL_GLEXT_PROTOTYPES
// #include<GL/gl.h>
// #include<GL/glext.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"../include/gl_context.h"
#include"../include/gl_object.h"
#include"../include/gl_vao.h"
#include"../include/gl_vbo.h"
#include"../include/gl_shader.h"
#include"../include/gl_program.h"
#include"../include/gl_texture.h"
#include<iostream>
#include<cassert>
#include<fstream>
#include<exception>
#include<memory>
#include<cmath>
#include<array>

int main(){
    gl_context contx{600, 800, "window"};

    // 2 boxes
    std::array<gl_vao, 2> arr_vao;

    // object attributes data
    gl_vbo vbo;
    // boxes buffer data, pos + normal + tex coord
    float vertex_data[] = {
        // back side
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,

        // front side
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        // left side
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        // right side
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        // top
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // bottom
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f
    };
    // index buffer
    unsigned int index[] = {
        0, 1, 2, 0, 3, 2, 4, 5, 6, 4, 7, 6, 8, 9, 10, 8, 11, 10, 12, 13, 14, 12, 15, 14, 16, 17, 18, 16, 19, 18, 20, 21, 22, 20, 23, 22
    };
    vbo.bind();
    vbo.loadData(vertex_data, sizeof(vertex_data), GL_DYNAMIC_DRAW, 3, 3, 2);

    for(auto &vao : arr_vao){
        vao.bind();
        // formatting data with attrib index and assign index buffer
        vao.attribData(0, vbo.attribOneCompNum(), GL_FLOAT, GL_FALSE, vbo.stride(), vbo.attribOneOffset());
        vao.attribData(1, vbo.attribTwoCompNum(), GL_FLOAT, GL_FALSE, vbo.stride(), vbo.attribTwoOffset());
        vao.attribData(2, vbo.attribThreeCompNum(), GL_FLOAT, GL_FALSE, vbo.stride(), vbo.attribThreeOffset());
        vao.eboData(index, sizeof(index), GL_DYNAMIC_DRAW);
    }

    // texture
    gl_texture brickwall;
    brickwall.bind();
    brickwall.loadData("/home/hungvu/Archive/progs/opengl/texture/brickwall.png");

    // shader
    gl_shader vertex_shader{VERTEX_SHADER};
    gl_shader object_fshader{FRAGMENT_SHADER};
    gl_shader light_fshader{FRAGMENT_SHADER};

    // program
    gl_program program;

    // load shader and program
    vertex_shader.compileSource("/home/hungvu/Archive/progs/opengl/shader/vertex.glsl");
    object_fshader.compileSource("/home/hungvu/Archive/progs/opengl/shader/fragment.glsl");
    light_fshader.compileSource("/home/hungvu/Archive/progs/opengl/shader/light_frag.glsl");
    gl_shader* shaders[] = {&vertex_shader, &light_fshader};
    program.loadCompiledShaders(shaders, sizeof(shaders) / sizeof(gl_shader*));

    // uniform variable values and transformation matrices
    // program.assignUniform<GLfloat>("time", glUniform1f, (float)glfwGetTime());
    glm::vec3 model_objectPosition(0.0f, 0.0f, -2.5f);
    glm::vec3 object_color(1.0f, 0.5f, 0.5f);
    glm::vec3 white_light(1.0f, 0.5f, 0.5f);
    glm::mat4 basis{1.0f};
    glm::mat4 model{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 projection = glm::perspective(glm::radians(100.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

    // misc rendering modes
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // rendering loop
    while(!glfwWindowShouldClose(contx.getWindow())){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        float time = glfwGetTime();

        glm::vec3 model_lightPosition{time, 2 * glm::sin(time), -2.5f};
        for(size_t i = 0; i < arr_vao.size(); i++){
            arr_vao[i].bind();
            if(!i && !light_fshader.getAttachStatus()){ // attach light_fshader for vao[0], light source
                program.changeShader(object_fshader, light_fshader);
                program.assignUniform<GLsizei, const GLfloat*>("white_light", glUniform3fv, 1, glm::value_ptr(white_light));
                model = glm::translate(basis, model_lightPosition);
            }
            else if(i && !object_fshader.getAttachStatus()){ //  attach object_fshader for vao[1], normal object
                program.changeShader(light_fshader, object_fshader);
                program.assignUniform<GLsizei, const GLfloat*>("object_color", glUniform3fv, 1, glm::value_ptr(object_color));
                program.assignUniform<GLsizei, const GLfloat*>("white_light", glUniform3fv, 1, glm::value_ptr(white_light));
                model = glm::translate(basis, model_objectPosition);
            }
            view = glm::translate(model, glm::vec3(-1.0f, 0.0f, 1.0f));
            // assign uniform variable
            program.assignUniform("light_position", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(model_lightPosition)));
            program.assignUniform<GLsizei, GLboolean, const GLfloat*>("model", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(model));
            program.assignUniform<GLsizei, GLboolean, const GLfloat*>("view", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(view));
            program.assignUniform<GLsizei, GLboolean, const GLfloat*>("projection", glUniformMatrix4fv, 1, GL_FALSE, glm::value_ptr(projection));

            glDrawElements(GL_TRIANGLES, arr_vao[i].eboNumIndices(), GL_UNSIGNED_INT, (void*)0);
        }

        glfwSwapBuffers(contx.getWindow());
        glfwPollEvents();
    }

    return 0;
}
