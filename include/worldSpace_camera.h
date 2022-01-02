#ifndef WORLDSPACE_CAMERA_H
#define WORLDSPACE_CAMERA_H
#include<glm/vec3.hpp>
#include<glm/geometric.hpp>

struct worldSpace_camera{
    glm::vec3 position{0.0f};
    glm::vec3 direction{0.0f, 0.0f, -1.0f};
    // glm::vec3 up(0.0f, 1.0f, 0.0f);
    float moveRate = 1.0f;
    glm::vec3 up(){
        return glm::cross(position, direction);
    }
    glm::vec3 side(){
        return glm::cross(direction, up());
    }
    glm::vec3 lookAt(){
        // keep look at the specified direction and not a specific object
        return position + direction;
    }
    void handleInputSpecific(GLFWwindow *window, int key, int scancode, int actions, int mods){
        if(actions == GLFW_PRESS){
            switch(key){
                // movements
                case GLFW_KEY_W: position += direction; break;
                case GLFW_KEY_A: position += side(); break;
                case GLFW_KEY_S: position -= side(); break;
                case GLFW_KEY_D: position -= direction; break;
                 // pitch up
                case GLFW_KEY_Z: break;
                 // pitch down
                case GLFW_KEY_X: break;
                // yaw left
                case GLFW_KEY_C: break;
                // yaw right
                case GLFW_KEY_V: break;
                // roll counterclockwise
                case GLFW_KEY_R: break;
                // roll clockwise
                case GLFW_KEY_F: break;
            }
        }
    }
};
#endif // WORLDSPACE_CAMERA_H
