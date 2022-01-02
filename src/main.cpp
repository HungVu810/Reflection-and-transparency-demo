// #define GL_GLEXT_PROTOTYPES
// #include<GL/gl.h>
// #include<GL/glext.h>
#include"../include/gl_context.h"
#include"../include/gl_object.h"
#include"../include/gl_vao.h"
#include"../include/gl_vbo.h"
#include"../include/gl_shader.h"
#include"../include/gl_program.h"
#include"../include/gl_texture.h"
// #include"../include/worldSpace_camera.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/vec3.hpp>
#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>
#include<cassert>
#include<fstream>
#include<exception>
#include<memory>
#include<cmath>
#include<array>

namespace worldSpace_camera{
    glm::vec3 position{0.0f};
    // the direction is the same unless spinned with roll or pitch then the
    // whole camera frame (direction, up, side) vectors basis is rotated
    glm::vec3 direction{0.0f, 0.0f, -1.0f}; // -z axis
    glm::vec3 up{0.0f, 1.0f, 0.0f}; // y axis
    glm::vec3 side{1.0f, 0.0f, 0.0f}; // x axis
    double pitch_angle = 0.0;
    double yaw_angle = 0.0;
    double roll_angle = 0.0;
    float win_width, win_height;
    float moveRate = 0.5f;
    double sensitivity = 0.000002;
    glm::vec3 lookAt(){
        // keep look at the specified direction and not a specific object
        return position + direction;
    }
}

void handleInputGeneral(GLFWwindow *window, int key, int scancode, int actions, int mods){
    using namespace worldSpace_camera;
    if(actions == GLFW_PRESS){
        switch(key){
            case GLFW_KEY_Q: glfwSetWindowShouldClose(window, 1); break;
            // movements
            case GLFW_KEY_W: position += moveRate * direction; break;
            case GLFW_KEY_S: position -= moveRate * direction; break;
            case GLFW_KEY_A: position -= moveRate * side; break;
            case GLFW_KEY_D: position += moveRate * side; break;
        }
    }
}

void handleCursorPos(GLFWwindow *window, double xpos, double ypos){
    using namespace worldSpace_camera;
    if (xpos >= 0 && xpos <= win_width && ypos >= 0 && ypos <= win_height){
        yaw_angle = (-std::acos(-1) / win_width) * xpos + std::acos(0);
        pitch_angle = (-std::acos(-1) / win_height) * ypos + std::acos(0);
        side = glm::vec3(std::cos(yaw_angle), side.y, -std::sin(yaw_angle));
        up = glm::vec3(up.x, std::cos(pitch_angle), std::sin(pitch_angle));
        direction = glm::vec3(
                -std::sin(yaw_angle),
               std::sin(pitch_angle),
               -std::cos(yaw_angle)
               );
    }
}

int main(){
    gl_context contx{1920, 1080, "window"};

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
    glm::vec3 model_lightPosition{0.0f};
    glm::vec3 object_color(1.0f, 0.5f, 0.5f);
    glm::vec3 white_light(1.0f, 1.0f, 1.0f);
    glm::mat4 basis{1.0f};
    glm::mat4 model{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 projection = glm::perspective(glm::radians(100.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

    // misc rendering modes
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // rendering loop
    while(!glfwWindowShouldClose(contx.getWindow())){
        // clear framebuffer from previous rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // handle input and update data
        glfwSetInputMode(contx.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        worldSpace_camera::win_width = contx.width();
        worldSpace_camera::win_height = contx.height();
        glfwSetCursorPosCallback(contx.getWindow(), handleCursorPos);
        glfwSetKeyCallback(contx.getWindow(), handleInputGeneral);

        float time = glfwGetTime();
        model_lightPosition = glm::vec3(2.0f, 2 * std::cos(time), -2.5f);
        // camera_position = model_objectPosition + glm::vec3(2.5 * std::cos(time * 1/2), 2, 2.5 * std::sin(time * 1/2));

        for(size_t i = 0; i < arr_vao.size(); i++){
            arr_vao[i].bind();
            if(!i && !light_fshader.getAttachStatus()){ // attach light_fshader for vao[0], light source
                program.changeShader(object_fshader, light_fshader);
                program.assignUniform<GLsizei, const GLfloat*>("white_light", glUniform3fv, 1, glm::value_ptr(white_light));
                model = glm::translate(basis, model_lightPosition);
            }
            else if(i && !object_fshader.getAttachStatus()){ //  attach object_fshader for vao[1], normal object
                program.changeShader(light_fshader, object_fshader);
                program.assignUniform("object_color", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(object_color)));
                program.assignUniform("white_light", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(white_light)));
                model = glm::translate(basis, model_objectPosition);
            }
            view = glm::lookAt(worldSpace_camera::position, worldSpace_camera::lookAt(), worldSpace_camera::up);
            // light_position depended on the view matrix
            program.assignUniform("light_position", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(glm::vec3(view * glm::vec4(model_lightPosition, 1.0f)))));
            program.assignUniform("model", glUniformMatrix4fv, 1, static_cast<unsigned char>(GL_FALSE), const_cast<const float*>(glm::value_ptr(model)));
            program.assignUniform("view", glUniformMatrix4fv, 1, static_cast<unsigned char>(GL_FALSE), const_cast<const float*>(glm::value_ptr(view)));
            program.assignUniform("projection", glUniformMatrix4fv, 1, static_cast<unsigned char>(GL_FALSE), const_cast<const float*>(glm::value_ptr(projection)));

            glDrawElements(GL_TRIANGLES, arr_vao[i].eboNumIndices(), GL_UNSIGNED_INT, (void*)0);
        }

        glfwSwapBuffers(contx.getWindow());
        glfwPollEvents();
    }
    return 0;
}
