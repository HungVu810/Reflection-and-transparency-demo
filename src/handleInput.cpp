#include "../include/handleInput.h"
//#include "../../imgui/imgui.h"
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <array>
#include <cassert>

void handleFunc::keystroke(GLFWwindow *window, int key, int scancode, int actions, int mods){
	using namespace camera;
	if(actions == GLFW_PRESS || actions == GLFW_REPEAT){
		switch(key){
			case GLFW_KEY_Q: glfwSetWindowShouldClose(window, 1); break; // close window
			case GLFW_KEY_W: position += move_rate * direction; break;   // move foward
			case GLFW_KEY_S: position -= move_rate * direction; break;   // move backward
			case GLFW_KEY_A: position -= move_rate * side; break;        // move left
			case GLFW_KEY_D: position += move_rate * side; break;        // move right
			case GLFW_KEY_R: position += move_rate * up; break;          // move up
			case GLFW_KEY_F: position -= move_rate * up; break;          // move down
			case GLFW_KEY_M: isMirror = !isMirror;						 // toggling wall's option (mirror or x-ray)
			case GLFW_KEY_LEFT_SHIFT: enable_roll = 1; break;            // roll camera (leftshift + mouse)
			case GLFW_KEY_Z: if (yfov < 100.0f) yfov++; break;           // increase vertical field of view
			case GLFW_KEY_X: if (yfov > 20.0f) yfov--; break;            // decrease vertical field of view
			case GLFW_KEY_C: yfov = 60.0f; break;                        // reset vertical field of view to 60 (default)
			default:{
				std::cerr << "Unprocess pressed keycode: " << key << std::endl;
				return;
			}
		}
	}
	
	else{ // GLFW_RELEASE
		switch(key){
			case GLFW_KEY_LEFT_SHIFT: enable_roll = 0; break;
			default:{
				std::cerr << "Unprocess released keycode: " << key << std::endl;
				return;
			}
		}
	}
};

void handleFunc::mousebutton(GLFWwindow *window, int button, int actions, int mods){
	using namespace camera;
	//ImGuiIO& io = ImGui::GetIO();            // forward input to ImGui
	//io.AddMouseButtonEvent(button, actions); // let ImGui handle the input

	//if(!io.WantCaptureMouse){ // if ImGui is unfocused, we process the mouse input instead
		if(actions == GLFW_PRESS || actions == GLFW_REPEAT){
			switch(button){
				case GLFW_MOUSE_BUTTON_RIGHT: enable_rotation = 1; break; // enable camera rotation (yaw, pitch and roll)
				default:{
					std::cerr << "Unprocess pressed mouse button: " << button << std::endl;
					return;
				}
			}
		}
		else{ // GLFW_RELEASE
			switch(button){
				case GLFW_MOUSE_BUTTON_RIGHT: enable_rotation = 0; break;
				default:{
					std::cerr << "Unprocess released mouse button: " << button << std::endl;
					return;
				}
			}
		}
	//}
}

void handleFunc::cursorPos(GLFWwindow *window, double xpos, double ypos){
	using namespace camera;
	// mouse_callback = true;

	// forward input to ImGui
	//ImGuiIO& io = ImGui::GetIO();
	//io.AddMousePosEvent(xpos, ypos);

	//if(!io.WantCaptureMouse){ // if ImGui is unfocused, we process the mouse input instead
	//	if(enable_rotation){
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
			if(enable_roll){
				// reuse the pitch angle for roll
				up = glm::rotate(up, yaw, direction);
				side = glm::rotate(side, yaw, direction);
			}

			prev_xpos = xpos;
			prev_ypos = ypos;
	//	}
	//}
};

