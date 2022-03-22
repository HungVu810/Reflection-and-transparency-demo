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
#include "../include/light_model.h"
#include "../include/handleInput.h"
#include "../include/camera.h"
#include "../include/scene.h"
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

// void initAxis(const glm::vec4 &xAxisColor, const glm::vec4 &yAxisColor, const glm::vec4 &zAxisColor, std::array<model, size_t{3}> &axis);

int main(){
	// init context and misc.
	gl_context contx{1920, 1080, "window"};
	glfwSetInputMode(contx.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	camera::win_width = contx.width();
	camera::win_height = contx.height();

	// shaders
	gl_shader vertex_shader{GL_VERTEX_SHADER};
	gl_shader model_fshader{GL_FRAGMENT_SHADER};
	gl_shader light_fshader{GL_FRAGMENT_SHADER};
	vertex_shader.loadData("/home/hungvu/Archive/progs/opengl/shader/vertex.glsl");
	vertex_shader.compile();
	model_fshader.loadData("/home/hungvu/Archive/progs/opengl/shader/fragment.glsl");
	model_fshader.compile();
	light_fshader.loadData("/home/hungvu/Archive/progs/opengl/shader/light_frag.glsl");
	light_fshader.compile();

	// init model objects
	std::array<model, size_t(2)> models;
	std::array<model, size_t(3)> axis;
	std::array<light_model, size_t(3)> lights;

	std::string backpack_model = "/home/hungvu/Archive/progs/opengl/model/backpack/backpack.obj";
	std::string skull_model = "/home/hungvu/Archive/progs/opengl/model/skull/12140_Skull_v3_L2.obj";
	std::string axis_model = "/home/hungvu/Archive/progs/opengl/model/arrow/sun-dial-arrow.obj";
	std::vector<tex_info> backpack_mat{
		tex_info{"/home/hungvu/Archive/progs/opengl/model/backpack/diffuse.jpg", aiTextureType_DIFFUSE},
		tex_info{"/home/hungvu/Archive/progs/opengl/model/backpack/specular.jpg", aiTextureType_SPECULAR}
	};
	std::vector<tex_info> skull_mat{
		tex_info{"/home/hungvu/Archive/progs/opengl/model/skull/diffuse.jpg", aiTextureType_DIFFUSE}
	};

	// models
	// backpack
	models[0].loadData(backpack_model, backpack_mat, 0, &model_fshader);
	models[0].translate(glm::vec3{2.0f, 0.0f, 0.0f});

	// skull
	models[1].loadData(skull_model, skull_mat, 1, &model_fshader);
	models[1].translate(glm::vec3{-10.0f, 0.0f, 0.0f});
	models[1].rotate(-std::acos(0), glm::vec3{1.0f, 0.0f, 0.0f});
	models[1].scale(0.05f);

	// axis
	axis[0].loadData(axis_model, glm::vec4{1.0f, 0.0f, 0.0f, 0.0f}, &model_fshader);
	axis[2] = axis[1] = axis[0];
	// x side axis
	axis[0].rotate(glm::radians(90.0f), glm::vec3{0.0f, 1.0f, 0.0f});
	axis[0].scale(0.5f);
	// y up axis
	axis[1].assignVertexColor(glm::vec4{0.0f, 1.0f, 0.0f, 0.0f});
	axis[1].rotate(glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f});
	axis[1].scale(0.5f);
	// -z directional axis
	axis[2].assignVertexColor(glm::vec4{0.0f, 0.0f, 1.0f, 0.0f});
	axis[2].scale(-0.5f); // point in -z axis due to OpenGL right hand rule

	// light
	lights[0].loadData("/home/hungvu/Archive/progs/opengl/model/sphere/globe-sphere.obj", glm::vec4{1.0f, 0.0f, 0.0f, 0.0f}, &light_fshader);
	lights[0].translate(glm::vec3{5.0f, 0.0f, 0.0f});
	lights[0].assignInnerConeAngle(10.0f);
	lights[0].assignOuterConeAngle(20.0f);
	lights[0].assignDirection(glm::vec3{0.0f, 0.0f, 0.0f});

	lights[2] = lights[1] = lights[0];

	lights[1].translate(glm::vec3{0.0f, 5.0f, 0.0f});
	lights[1].assignVertexColor(glm::vec4{0.0f, 1.0f, 0.0f, 0.0f});

	lights[2].translate(glm::vec3{0.0f, 0.0f, -5.0f});
	lights[2].assignVertexColor(glm::vec4{0.0f, 0.0f, 1.0f, 0.0f});

	// program
	gl_program program;

	// load shader and program
	program.attachCompiledShader(&vertex_shader);
	program.attachCompiledShader(&model_fshader);
	program.link();
	program.use();
	program.printDebugUniform(1);

	// misc rendering modes
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
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

		for(model &m : axis){
			m.draw(program, camera::projection, camera::view, GL_STATIC_DRAW);
		}

		// if(camera::enable_debug_cam)
		lights[0].rotate(time, glm::vec3{0.0f, 0.0f, -1.0f});

		lights[1].rotate(time, glm::vec3{1.0f, 0.0f, 0.0f});

		lights[2].rotate(time, glm::vec3{0.0f, 1.0f, 0.0f});

		scene::draw(program, camera::projection, camera::view, models, lights, GL_STATIC_DRAW);

		lights[0].undoLastTransform();

		lights[1].undoLastTransform();

		lights[2].undoLastTransform();

		glfwSwapBuffers(contx.getWindow());
		glfwPollEvents();
	}
	return 0;
}
