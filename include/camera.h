#ifndef CAMERA_H
#define CAMERA_H
#include "./model.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace camera{
	// default win_width and win_height. These variables are assigned to a
	// more specific value during the start of main()
	unsigned win_width = 1920, win_height = 1080;

	// projection and view matrix fir the camera.
	glm::mat4 projection{1.0f}, view{1.0f};

	// camera position relative to the world space origin
	glm::vec3 position{0.0f};

	// previous position of the mouse cursor (0,0 top left to the x,y
	// resolution at bottom right). The previous positions are used to
	// calculate the offset against the current position (xpos, ypos in
	// handleCursorPos) for the direction transformation. The prev_xpos and
	// y_xpos are double instead of float like the angle variables because the
	// angles are restricted to float due to the glm::vec3 is a 3-float and
	// glm function works based on the glm::vec3 var type.
	double prev_xpos = 1920 / 2, prev_ypos = 1080 / 2;

	 // -z axis
	glm::vec3 direction{0.0f, 0.0f, -1.0f};

	 // y axis
	glm::vec3 up{0.0f, 1.0f, 0.0f};

	 // x axis
	glm::vec3 side{1.0f, 0.0f, 0.0f};

	// THE DEFAULT ORDER IS YAW, PITCH THEN ROLL
	// rotate about the x-axis (up, down)
	float pitch_angle = 0.0f;

	// rotate about the y-axis (left, right)
	float yaw_angle = 0.0f;

	// rotate about the z-axis (counter-clockwise, clockwise). The roll angle is based on the pitch_angle
	float roll_angle = 0.0f;

	// enable rotation when leftshift is pressed
	bool rotation_enabled = 0;

	// control how fast the camera will move in the vertical/ horizontal
	// directions
	float move_rate = 0.5f;

	// control the mouse sensitivity rate
	float sensitivity = 1.0f;

	// DEBUGGING CAM SECTION ========================

	// enable debugging camera will enable another static pov that look at
	// that main camera. The main camera is included with 3 axis models to
	// visualize the up, directional and side axis.
	bool enable_debug_cam = 0;

	// update the debugging cam variables below if the handleKeystroke and the
	// handleCursorPos are called by the callback function for the mouse,
	// keyboard movement.
	bool mouse_callback = 0;

	// variables for debugging camera. The debugging vars are used to render
	// the main camera 3-axis model.
	glm::vec3 prev_direction{0.0f, 0.0f, -1.0f};

	glm::vec3 prev_up{0.0f, 1.0f, 0.0f};

	glm::vec3 prev_side{1.0f, 0.0f, 0.0f};

	float prev_pitch_angle = 0.0f;
}

// view = glm::lookAt(
// 		glm::vec3{1.0f, 1.0f, -1.0f},
// 		glm::vec3{0.0f, 0.0f, 0.0f},
// 		glm::vec3{-0.5f, 2.0f, -0.5f});

// // if(camera::callback){
// // 	using namespace camera;
// // 	// yaw
// // 	axis[0].rotate(yaw_angle, prev_up.x, prev_up.y, prev_up.z);
// // 	axis[2].rotate(yaw_angle, prev_up.x, prev_up.y, prev_up.z);
// // 	prev_dir = glm::rotate(prev_dir, yaw_angle, prev_up);
// // 	prev_side = glm::rotate(prev_side, yaw_angle, prev_up);

// // 	// pitch
// // 	axis[1].rotate(pitch_angle, prev_side.x, prev_side.y, prev_side.z);
// // 	axis[2].rotate(pitch_angle, prev_side.x, prev_side.y, prev_side.z);
// // 	prev_dir = glm::rotate(prev_dir, pitch_angle, prev_side);
// // 	prev_up = glm::rotate(prev_up, pitch_angle, prev_side);

// // 	for(auto &m : axis){
// // 		m.draw(program, projection, view, GL_STATIC_DRAW);
// // 	}
// // 	camera::callback = false;
// // }



#endif // CAMERA_H
