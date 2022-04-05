#include "../include/handleInput.h"
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <array>
#include <cassert>

void handleFunc::keystroke(GLFWwindow *window, int key, int scancode, int actions, int mods){
	using namespace camera;
	if(actions == GLFW_PRESS || actions == GLFW_REPEAT){
		switch(key){
			case GLFW_KEY_Q:{
				glfwSetWindowShouldClose(window, 1);
				return;
			} break;
			// move foward
			case GLFW_KEY_W: position += move_rate * direction; break;
			// move backward
			case GLFW_KEY_S: position -= move_rate * direction; break;
			// move left
			case GLFW_KEY_A: position -= move_rate * side; break;
			// move right
			case GLFW_KEY_D: position += move_rate * side; break;
			// move up
			case GLFW_KEY_R: position += move_rate * up; break;
			// move down
			case GLFW_KEY_F: position -= move_rate * up; break;
			// rotate camera with mouse
			case GLFW_KEY_LEFT_SHIFT: rotation_enabled = 1; break;
			// increase vertical field of view
			case GLFW_KEY_Z:{
				if (yfov < 100.0f) yfov++;
				break;
			}
			// decrease vertical field of view
			case GLFW_KEY_X:{
				if (yfov > 20.0f) yfov--;
				break;
			}
			// reset vertical field of view to 60 (default)
			case GLFW_KEY_C: yfov = 60.0f; break;
			default:{
				std::cerr << "Unprocess pressed keycode: " << key << std::endl;
				return;
			}
		}
	}

	// GLFW_RELEASE
	if(actions == GLFW_RELEASE){
		switch(key){
			case GLFW_KEY_LEFT_SHIFT: rotation_enabled = 0; break;
		}
	}
};

void handleFunc::cursorPos(GLFWwindow *window, double xpos, double ypos){
	using namespace camera;

	mouse_callback = true;
	prev_direction = direction;
	prev_side = side;
	prev_up = up;

	float diff_x = xpos - prev_xpos;
	float diff_y = ypos - prev_ypos;
	float yaw = (-std::acos(-1) / win_width) * diff_x;
	float pitch = (-std::acos(-1) / win_height) * diff_y;
	yaw_angle = yaw;
	pitch_angle = pitch;

	// yaw
	direction = glm::rotate(direction, yaw, up);
	side = glm::rotate(side, yaw, up);
	// assert(glm::dot(side, direction) < 0.01);

	// pitch
	direction = glm::rotate(direction, pitch, side);
	up = glm::rotate(up, pitch, side);
	// assert(glm::dot(direction, up) < 0.01);

	// roll
	if(rotation_enabled){
		// reuse the pitch angle for rotation
		up = glm::rotate(up, yaw, direction);
		side = glm::rotate(side, yaw, direction);
	}

	prev_xpos = xpos;
	prev_ypos = ypos;
};

