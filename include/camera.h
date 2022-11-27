#ifndef CAMERA_H
#define CAMERA_H
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace camera{
	// default win_width and win_height. These variables are assigned to a
	// more specific value during the start of main()
	extern unsigned win_width;

	extern unsigned win_height;

	// projection and view matrix for the camera.
	extern glm::mat4 projection;

	extern glm::mat4 view;

	// camera y-x plane fov
	extern float yfov;

	// camera position relative to the world space origin
	extern glm::vec3 position;

	// previous position of the mouse cursor (0,0 top left to the x,y
	// resolution at bottom right). The previous positions are used to
	// calculate the offset against the current position (xpos, ypos in
	// handleCursorPos) for the direction transformation. The prev_xpos and
	// y_xpos are double instead of float like the angle variables because the
	// angles are restricted to float due to the glm::vec3 is a 3-float and
	// glm function works based on the glm::vec3 var type.
	extern double prev_xpos;

	extern double prev_ypos;

	 // -z axis
	extern glm::vec3 direction;

	 // y axis
	extern glm::vec3 up;

	 // x axis
	extern glm::vec3 side;

	// THE DEFAULT ORDER IS YAW, PITCH THEN ROLL
	// rotate about the x-axis (up, down)
	extern float pitch_angle;

	// rotate about the y-axis (left, right)
	extern float yaw_angle;

	// rotate about the z-axis (counter-clockwise, clockwise). The roll angle is based on the pitch_angle
	extern float roll_angle;

	// enable roll when leftshift is pressed
	extern bool enable_roll;

	// enable rotation when left mouse is pressed (yaw, pitch and roll)
	extern bool enable_rotation;

	// control how fast the camera will move in the vertical/ horizontal
	// directions
	extern float move_rate;

	// control the mouse sensitivity rate
	extern float sensitivity;

	// DEBUGGING CAM SECTION ========================

	// enable debugging camera will enable another static pov that look at
	// that main camera. The main camera is included with 3 axis models to
	// visualize the up, directional and side axis.
	extern bool enable_debug_cam;

	// update the debugging cam variables below if the handleKeystroke and the
	// handleCursorPos are called by the callback function for the mouse,
	// keyboard movement.
	extern bool mouse_callback;

	// variables for debugging camera. The debugging vars are used to render
	// the main camera 3-axis model.
	extern glm::vec3 prev_direction;

	extern glm::vec3 prev_up;

	extern glm::vec3 prev_side;

	extern float prev_pitch_angle;

	// TOGGLING MIRROR/X-RAY FOR THE WALL
	extern bool isMirror;
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
