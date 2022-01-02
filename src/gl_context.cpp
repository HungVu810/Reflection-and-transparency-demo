#include"../include/gl_object.h"
#include"../include/gl_context.h"
#include<iostream>
#include<cassert>

void frameBufferSizeCallback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

std::stack<gl_object> gl_context::st_glObject;

gl_context::gl_context(unsigned int width, unsigned int height, std::string wname){
    // init OpengGL context with glfw framework
    glfwInit();
    window = glfwCreateWindow(width, height, wname.c_str(), nullptr, nullptr);
    if(!window){
        std::cout << "Can not create window" << std::endl;
        std::terminate();
    }
    // associate window to context before loading OpenGL API
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to init glad" << std::endl;
        std::terminate();
    }
    // set viewport and callback function when viewport is changed
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    win_width = width;
    win_height = height;
}

gl_context::~gl_context() noexcept{
    while(!gl_context::st_glObject.empty()){
        gl_context::st_glObject.pop(); // delete, detach object at the end before terminate context to minize opengl state changes
    }
    glfwTerminate();
}

GLFWwindow* gl_context::getWindow(){
    return window;
}

unsigned gl_context::width() const {
    return win_width;
}

unsigned gl_context::height() const {
    return win_height;
}
