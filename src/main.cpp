// #define GL_GLEXT_PROTOTYPES
// #include<GL/gl.h>
// #include<GL/glext.h>
#include "../include/gl_context.h"
#include "../include/gl_object.h"
#include "../include/gl_vao.h"
#include "../include/gl_vbo.h"
#include "../include/gl_shader.h"
#include "../include/gl_program.h"
#include "../include/gl_texture.h"
#include "../include/glw_model.h"
// #include"../include/worldSpace_camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/material.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include <exception>
#include <memory>
#include <cmath>
#include <array>

// TODO:clean this up handle() and camera(), cpp https://isocpp.org/wiki/faq/pointers-to-members#memfnptr-vs-fnptr
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
		return position + (0.5f * direction);
	}
}
void handleInputGeneral(GLFWwindow *window, int key, int scancode, int actions, int mods){
	using namespace worldSpace_camera;
	if(actions == GLFW_PRESS || actions == GLFW_REPEAT){
		if(key == GLFW_KEY_Q){
			glfwSetWindowShouldClose(window, 1);
			return;
		}
		if(key == GLFW_KEY_W){ position += moveRate * direction; }
		if(key == GLFW_KEY_S){ position -= moveRate * direction; }
		if(key == GLFW_KEY_A){ position -= moveRate * side; }
		if(key == GLFW_KEY_D){ position += moveRate * side; }
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
// TODO:clean this up handle() and camera(), cpp https://isocpp.org/wiki/faq/pointers-to-members#memfnptr-vs-fnptr

int main(){
	gl_context contx{1920, 1080, "window"};

	std::string skull_model = "/home/hungvu/Archive/progs/opengl/model/skull/12140_Skull_v3_L2.obj";
	std::vector<gl_texture> skull_mat = {
		gl_texture{"/home/hungvu/Archive/progs/opengl/model/skull/diffuse.jpg", aiTextureType_DIFFUSE}
	};
	glw_model skull_obj{skull_model, skull_mat, 1};
	skull_obj.scale(0.1f);

	std::string sphere_model = "/home/hungvu/Archive/progs/opengl/model/sphere/globe-sphere.obj";
	glw_model sphere_obj{sphere_model, glm::vec4{0.5f, 0.5f, 0.5f, 0.0f}};

	// shader
	gl_shader vertex_shader{VERTEX_SHADER};
	gl_shader object_fshader{FRAGMENT_SHADER};
	gl_shader light_fshader{FRAGMENT_SHADER};

	// program
	gl_program program;

	// load shader and program
	vertex_shader.compileSource("/home/hungvu/Archive/progs/opengl/shader/vertex.glsl");
	object_fshader.compileSource("/home/hungvu/Archive/progs/opengl/shader/fragment.glsl");
	// light_fshader.compileSource("/home/hungvu/Archive/progs/opengl/shader/light_frag.glsl");
	// gl_shader* shaders[] = {&vertex_shader, &light_fshader};
	gl_shader* shaders[] = {&vertex_shader, &object_fshader};
	program.loadCompiledShaders(shaders, sizeof(shaders) / sizeof(gl_shader*));

	// uniform variable values and transformation matrices
	glm::vec3 model_objectPosition(0.0f, 0.0f, -2.5f);
	glm::vec3 model_lightPosition{0.0f};
	// glm::vec3 object_color(0.5f, 0.5f, 0.5f);
	glm::vec3 light_color(0.0f, 1.0f, 1.0f);
	glm::mat4 basis{1.0f};
	glm::mat4 model{1.0f};
	glm::mat4 view{1.0f};
	glm::mat4 projection = glm::perspective(glm::radians(100.0f), (const float)contx.width() / contx.height(), 0.1f, 100.0f);

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
		// model_lightPosition = glm::vec3(std::cos(time), std::sin(time), 1.5f);

		// the vector is constant length, does not modifead based on the surface

		// camera_position = model_objectPosition + glm::vec3(2.5 * std::cos(time * 1/2), 2, 2.5 * std::sin(time * 1/2));
		view = glm::lookAt(worldSpace_camera::position, worldSpace_camera::lookAt(), worldSpace_camera::up);

		program.assignUniform("model", glUniformMatrix4fv, 1, static_cast<unsigned char>(GL_FALSE), const_cast<const float*>(glm::value_ptr(model)));
		program.assignUniform("view", glUniformMatrix4fv, 1, static_cast<unsigned char>(GL_FALSE), const_cast<const float*>(glm::value_ptr(view)));
		program.assignUniform("projection", glUniformMatrix4fv, 1, static_cast<unsigned char>(GL_FALSE), const_cast<const float*>(glm::value_ptr(projection)));

		// program.assignUniform<GLsizei, const GLfloat*>("light_color", glUniform3fv, 1, glm::value_ptr(light_color));
		// program.assignUniform("light_position", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(glm::vec3(view * glm::vec4(model_lightPosition, 1.0f)))));
		// program.assignUniform("light_direction", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(glm::vec3(view * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)))));
		// program.assignUniform("light_inner_cone", glUniform1f, 10.0f);
		// program.assignUniform("light_outer_cone", glUniform1f, 20.0f);
		skull_obj.draw(program, GL_STATIC_DRAW);
		sphere_obj.draw(program, GL_STATIC_DRAW);

		// for(size_t i = 0; i < arr_vao.size(); i++){
		// 	arr_vao[i].bind();
		// 	if(!i){ // attach light_fshader for vao[0], light source
		// 		program.changeShader(object_fshader, light_fshader);
		// 		model = glm::scale(basis, glm::vec3(0.2f, 0.2f, 0.2f));
		// 		model = glm::translate(model, model_lightPosition);
		// 	}
		// 	else{ //  attach object_fshader for vao[i], i not 0, normal object
		// 		if(i == 1) program.changeShader(light_fshader, object_fshader);
		// 		// glm::vec4 debug_pos = view * glm::vec4(model_lightPosition, 1.0f);
		// 		// glm::vec4 debug_dir = view * (glm::vec4(model_lightPosition, 1.0f) + glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
		// 		// std::cerr << "(" << debug_pos.x << ", " << debug_pos.y << ", " << debug_pos.z << ") -- (" << debug_dir.x << ", " << debug_dir.y << ", " << debug_dir.z << ")" << std::endl;
		// 		program.assignUniform("light_position", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(glm::vec3(view * glm::vec4(model_lightPosition, 1.0f)))));
		// 		program.assignUniform("light_direction", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(glm::vec3(view * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)))));
		// 		program.assignUniform("light_inner_cone", glUniform1f, 10.0f);
		// 		program.assignUniform("light_outer_cone", glUniform1f, 20.0f);
		// 		program.assignUniform("material.wood_box", glUniform1i, wood_box.getUnit());
		// 		program.assignUniform("material.steel_frame", glUniform1i, steel_frame.getUnit());
		// 		program.assignUniform("material.matrix", glUniform1i, matrix.getUnit());
		// 		program.assignUniform("material.matrix_emission_strength", glUniform1f, 0.0f);
		// 		program.assignUniform("material.steel_frame_ambient", glUniform1f, 0.5f);
		// 		program.assignUniform("material.diffuse_tol", glUniform1f, 10.0f);
		// 		program.assignUniform("material.specular_tol", glUniform1f, 1.0f);
		// 		program.assignUniform("material.shininess", glUniform1f, 32.0f);
		// 		if(i == 1) model = glm::rotate(glm::translate(basis, model_objectPosition + glm::vec3(-1.0f, -1.0f, 0.0f)), glm::radians(time * 50), glm::vec3(1.0f, 1.0f, 0.0f));
		// 		if(i == 2) model = glm::translate(basis, model_objectPosition + glm::vec3(-1.0f, 1.0f, 0.0f));
		// 		if(i == 3) model = glm::translate(basis, model_objectPosition + glm::vec3(1.0f, -1.0f, 0.0f));
		// 		if(i == 4) model = glm::rotate(glm::translate(basis, model_objectPosition + glm::vec3(1.0f, 1.0f, 0.0f)), glm::radians(time *50), glm::vec3(-1.0f, -1.0f, 0.0f));
		// 	}
		// 	// light_position depended on the view matrix
		// 	program.assignUniform("time", glUniform1f, time);
		// 	program.assignUniform<GLsizei, const GLfloat*>("light_color", glUniform3fv, 1, glm::value_ptr(light_color));
		// 	program.assignUniform("model", glUniformMatrix4fv, 1, static_cast<unsigned char>(GL_FALSE), const_cast<const float*>(glm::value_ptr(model)));
		// 	program.assignUniform("view", glUniformMatrix4fv, 1, static_cast<unsigned char>(GL_FALSE), const_cast<const float*>(glm::value_ptr(view)));
		// 	program.assignUniform("projection", glUniformMatrix4fv, 1, static_cast<unsigned char>(GL_FALSE), const_cast<const float*>(glm::value_ptr(projection)));

		// 	glDrawElements(GL_TRIANGLES, arr_vao[i].eboNumIndices(), GL_UNSIGNED_INT, (void*)0);
		// }

		glfwSwapBuffers(contx.getWindow());
		glfwPollEvents();
	}
	return 0;
}
