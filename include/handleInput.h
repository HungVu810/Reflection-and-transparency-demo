#ifndef HANDLEINPUT_H
#define HANDLEINPUT_H
#include "camera.h"
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>
#include <array>
#include <cassert>

void handleKeystroke(GLFWwindow *window, int key, int scancode, int actions, int mods){
	using namespace camera;
	if(actions == GLFW_PRESS || actions == GLFW_REPEAT){
		switch(key){
			case GLFW_KEY_Q:{
				glfwSetWindowShouldClose(window, 1);
				return;
			} break;
			case GLFW_KEY_W: position += move_rate * direction; break;
			case GLFW_KEY_S: position -= move_rate * direction; break;
			case GLFW_KEY_A: position -= move_rate * side; break;
			case GLFW_KEY_D: position += move_rate * side; break;
			case GLFW_KEY_K: position += move_rate * up; break;
			case GLFW_KEY_J: position -= move_rate * up; break;
			case GLFW_KEY_LEFT_SHIFT: rotation_enabled = 1; break;
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
}

void handleCursorPos(GLFWwindow *window, double xpos, double ypos){
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
}

#endif // HANDLEINPUT_H
