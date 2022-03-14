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
#include "../include/model.h"
#include "../include/handleInput.h"
#include "../include/camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/material.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include <exception>
#include <memory>
#include <cmath>
#include <array>

void initAxis(const glm::vec4 &xAxisColor, const glm::vec4 &yAxisColor, const glm::vec4 &zAxisColor, std::array<model, size_t{3}> &axis);

int main(){
	// init context and misc.
	gl_context contx{1920, 1080, "window"};
	glfwSetInputMode(contx.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	camera::win_width = contx.width();
	camera::win_height = contx.height();
	std::array<model, size_t{3}> main_axis;
	// std::array<model, size_t{3}> camera_axis;
	initAxis(glm::vec4{1.0f, 0.0f, 0.0f, 0.0f}, glm::vec4{0.0f, 1.0f, 0.0f, 0.0f}, glm::vec4{0.0f, 0.0f, 1.0f, 0.0f}, main_axis);
	// initAxis(glm::vec3{0.5f, 0.0f, 0.0f}, glm::vec3{0.0f, 0.5f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.5f}, camera_axis);
	bool render_main_axis = 1;

	// models
	std::array<model, size_t(8)> obj_list;
	std::string skull_path = "/home/hungvu/Archive/progs/opengl/model/skull/12140_Skull_v3_L2.obj";
	std::vector<tex_info> skull_mat = {
		tex_info{"/home/hungvu/Archive/progs/opengl/model/skull/diffuse.jpg", aiTextureType_DIFFUSE}
	};
	obj_list[0].loadData(skull_path, skull_mat, 1);
	obj_list[0].scale(0.1f);
	obj_list[0].translate(glm::vec3{-20.0f, 0.0f, 0.0f});

	obj_list[1].loadData("/home/hungvu/Archive/progs/opengl/model/sphere/globe-sphere.obj");
	obj_list[1].translate(glm::vec3{2.0f, 0.0f, 0.0f});


	// shaders
	gl_shader vertex_shader{GL_VERTEX_SHADER};
	gl_shader object_fshader{GL_FRAGMENT_SHADER};
	gl_shader light_fshader{GL_FRAGMENT_SHADER};

	// program
	gl_program program;

	// load shader and program
	vertex_shader.loadData("/home/hungvu/Archive/progs/opengl/shader/vertex.glsl");
	vertex_shader.compile();
	program.attachCompiledShader(&vertex_shader);
	object_fshader.loadData("/home/hungvu/Archive/progs/opengl/shader/fragment.glsl");
	object_fshader.compile();
	program.attachCompiledShader(&object_fshader);
	program.link();
	program.use();

	// light_fshader.compileSource("/home/hungvu/Archive/progs/opengl/shader/light_frag.glsl");
	// gl_shader* shaders[] = {&vertex_shader, &light_fshader};

	// misc rendering modes
	glEnable(GL_DEPTH_TEST);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	camera::projection = glm::perspective(glm::radians(100.0f), (const float)contx.width() / contx.height(), 0.1f, 100.0f);

	// rendering loop
	while(!glfwWindowShouldClose(contx.getWindow())){
		// clear framebuffer from previous rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// handle input and update data
		glfwSetCursorPosCallback(contx.getWindow(), handleCursorPos);
		glfwSetKeyCallback(contx.getWindow(), handleKeystroke);

		float time = glfwGetTime();

		camera::view = glm::lookAt(camera::position, camera::position + camera::direction, camera::up);

		// program.assignUniform<GLsizei, const GLfloat*>("light_color", glUniform3fv, 1, glm::value_ptr(light_color));
		// program.assignUniform("light_position", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(glm::vec3(view * glm::vec4(model_lightPosition, 1.0f)))));
		// program.assignUniform("light_direction", glUniform3fv, 1, const_cast<const float*>(glm::value_ptr(glm::vec3(view * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)))));
		// program.assignUniform("light_inner_cone", glUniform1f, 10.0f);
		// program.assignUniform("light_outer_cone", glUniform1f, 20.0f);
		// if(camera::enable_debug_cam) 

		for(auto &obj : main_axis){
			obj.draw(program, camera::projection, camera::view, GL_STATIC_DRAW);
		}

		for(auto &obj : obj_list){
			obj.draw(program, camera::projection, camera::view, GL_STATIC_DRAW);
		}
		glfwSwapBuffers(contx.getWindow());
		glfwPollEvents();

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
	}
	return 0;
}

void initAxis(const glm::vec4 &xAxisColor, const glm::vec4 &yAxisColor, const glm::vec4 &zAxisColor, std::array<model, size_t{3}> &axis){
	axis[0].loadData("/home/hungvu/Archive/progs/opengl/model/arrow/sun-dial-arrow.obj");
	axis[2] = axis[1] = axis[0];

	// x/ side axis
	axis[0].assignVertexColor(xAxisColor);
	axis[0].rotate(glm::radians(90.0f), glm::vec3{0.0f, 1.0f, 0.0f});
	axis[0].scale(0.5f);

	// y/ up axis
	axis[1].assignVertexColor(yAxisColor);
	axis[1].rotate(glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f});
	axis[1].scale(0.5f);

	// z/ directional axis
	axis[2].assignVertexColor(zAxisColor);
	axis[2].scale(-0.5f); // point in -z axis due to OpenGL right hand rule
}
