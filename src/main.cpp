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
// #include "../../imgui/imgui.h"
// #include "../../imgui/backends/imgui_impl_glfw.h"
// #include "../../imgui/backends/imgui_impl_opengl3.h"
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
#include <list>

 typedef  struct {
	uint  count;
	uint  instanceCount;
	uint  firstIndex;
	uint  baseVertex;
	uint  baseInstance;
} DrawElementsIndirectCommand;

template<size_t N>
void drawWithLights(model &m, std::array<light_model, N> &lights, gl_program &program, const gl_shader *fshader, GLenum usage = GL_STATIC_DRAW, const glm::mat4 &projection = camera::projection, const glm::mat4 &view = camera::view){
	program.attachCompiledShader(fshader);
	for(unsigned i = 0; i < lights.size(); i++){
		program.assignUniform(gl_program::creatUnifName("light[", i, "].direction").c_str(), glUniform3fv, 1, fval_ptr(lights[i].getDirection()));
		program.assignUniform(gl_program::creatUnifName("light[", i, "].position").c_str(), glUniform3fv, 1, fval_ptr(lights[i].getPosition()));
		program.assignUniform(gl_program::creatUnifName("light[", i, "].color").c_str(), glUniform3fv, 1, fval_ptr(lights[i].getVertexColor()));
		program.assignUniform(gl_program::creatUnifName("light[", i, "].inner_cone").c_str(), glUniform1f, lights[i].getInnerConeAngle());
		program.assignUniform(gl_program::creatUnifName("light[", i, "].outer_cone").c_str(), glUniform1f, lights[i].getOuterConeAngle());
	}
	m.draw(program, fshader);
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string source_str, type_str, severity_str;
	switch(source){
		case GL_DEBUG_SOURCE_API: source_str = "GL_DEBUG_SOURCE_API"; break;
		case GL_DEBUG_SOURCE_APPLICATION: source_str = "GL_DEBUG_SOURCE_APPLICATION"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "GL_DEBUG_SOURCE_WINDOW_SYSTEM"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "GL_DEBUG_SOURCE_THIRD_PARTY"; break;
		case GL_DEBUG_SOURCE_OTHER: source_str = "GL_DEBUG_SOURCE_OTHER"; break;
	}
	switch(type){
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"; break;
		case GL_DEBUG_TYPE_ERROR: type_str = "GL_DEBUG_TYPE_ERROR"; break;
		case GL_DEBUG_TYPE_MARKER: type_str = "GL_DEBUG_TYPE_MARKER"; break;
		case GL_DEBUG_TYPE_OTHER: type_str = "GL_DEBUG_TYPE_OTHER"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: type_str = "GL_DEBUG_TYPE_PERFORMANCE"; break;
		case GL_DEBUG_TYPE_POP_GROUP: type_str = "GL_DEBUG_TYPE_POP_GROUP"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: type_str = "GL_DEBUG_TYPE_PUSH_GROUP"; break;
		case GL_DEBUG_TYPE_PORTABILITY: type_str = "GL_DEBUG_TYPE_PORTABILITY"; break;
	}
	switch(severity){
		case GL_DEBUG_SEVERITY_HIGH: severity_str = "GL_DEBUG_SEVERITY_HIGH"; break;
		case GL_DEBUG_SEVERITY_LOW: severity_str = "GL_DEBUG_SEVERITY_LOW"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: severity_str = "GL_DEBUG_SEVERITY_MEDIUM"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "GL_DEBUG_SEVERITY_NOTIFICATION"; break;
	}
	std::cerr << "Debug output from " << source_str << ", type " << type_str << ", id " << id << ", severity " << severity_str << ":\n" << message << std::endl;
	// fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
}

int main(){
	// enable debug context (put this in gl_context cstr before creating window context)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	// init context and misc.
	gl_context contx{1920, 1080, "window"};
	glfwSetInputMode(contx.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	camera::win_width = contx.width();
	camera::win_height = contx.height();
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if(flags & GL_CONTEXT_FLAG_DEBUG_BIT){
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		// debug callback
	}

	// init imgui
	// ImGui::CreateContext();
	// ImGuiIO& io = ImGui::GetIO();
	// ImGui_ImplGlfw_InitForOpenGL(contx.getWindow(), 1);
	// ImGui_ImplOpenGL3_Init();

	// shaders
	gl_shader vertex_shader {"/home/hungvu/Archive/progs/opengl/shader/generic.vert", 1};
	gl_shader mat_fshader {"/home/hungvu/Archive/progs/opengl/shader/generic_mat.frag" ,1};
	gl_shader nomat_fshader {"/home/hungvu/Archive/progs/opengl/shader/generic_noMat.frag", 1};
	gl_shader grass_fshader {"/home/hungvu/Archive/progs/opengl/shader/grass.frag", 1};

	// init model objects
	std::array<model, size_t(2)> models;
	std::array<model, size_t(3)> axis;
	std::array<light_model, size_t(3)> lights;

	std::string backpack_model = "/home/hungvu/Archive/progs/opengl/model/backpack/backpack.obj";
	std::string skull_model = "/home/hungvu/Archive/progs/opengl/model/skull/12140_Skull_v3_L2.obj";
	std::string axis_model = "/home/hungvu/Archive/progs/opengl/model/arrow/sun-dial-arrow.obj";
	std::string tree_model =  "/home/hungvu/Archive/progs/opengl/model/tree/Tree1/Tree1.obj";
	std::string brickwall_model = "/home/hungvu/Archive/progs/opengl/model/brickwall/wall.obj";
	std::string mirror_model =  "/home/hungvu/Archive/progs/opengl/model/mirror/14321_Rectangular_Mirror-white_v1_l1.obj";
	std::string globe_model = "/home/hungvu/Archive/progs/opengl/model/sphere/globe-sphere.obj";

	// models
	// backpack
	models[0].loadData(backpack_model, 0);
	models[0].scale(2.0f);
	models[0].translate(glm::vec3{10.0f, 0.0f, -8.0f});
	models[0].assignVertexColor(glm::vec4{1.0f, 0.2f, 0.5f, 0.8f});

	// skull
	models[1].loadData(skull_model, 1);
	models[1].scale(0.25f);
	models[1].rotate(glm::vec3{1.0f, 0.0f, 0.0f}, -std::acos(0));
	models[1].translate(glm::vec3(-5.0f, -2.0f, -1.0f));
	models[1].assignVertexColor(glm::vec4{0.5f, 0.3f, 0.5f, 0.0f});

	// axis
	axis[0].loadData(axis_model);
	axis[0].assignVertexColor(glm::vec4{1.0f, 0.0f, 0.0f, 0.0f});
	axis[2] = axis[1] = axis[0];
	// x side axis
	axis[0].scale(0.5f);
	axis[0].rotate(glm::vec3{0.0f, 1.0f, 0.0f}, glm::radians(90.0f));
	// y up axis
	axis[1].scale(0.5f);
	axis[1].rotate(glm::vec3{1.0f, 0.0f, 0.0f}, glm::radians(-90.0f));
	axis[1].assignVertexColor(glm::vec4{0.0f, 1.0f, 0.0f, 0.0f});
	// -z directional axis
	axis[2].assignVertexColor(glm::vec4{0.0f, 0.0f, 1.0f, 0.0f});
	axis[2].scale(-0.5f); // point in -z axis due to OpenGL right hand rule

	// lights
	lights[0].loadData(globe_model);
	lights[0].assignVertexColor(glm::vec4{1.0f, 0.0f, 0.0f, 0.0f});
	lights[0].assignInnerConeAngle(10.0f);
	lights[0].assignOuterConeAngle(20.0f);
	lights[0].assignDirection(glm::vec3{0.0f, 0.0f, 0.0f});
	lights[2] = lights[1] = lights[0];
	lights[1].assignVertexColor(glm::vec4{0.0f, 1.0f, 0.0f, 0.0f});
	lights[2].assignVertexColor(glm::vec4{0.0f, 0.0f, 1.0f, 0.0f});

	// brickwall 
	model brickwall{brickwall_model};
	brickwall.assignVertexColor(glm::vec4{0.0f, 0.0f, 0.0f, 0.0f});
	brickwall.scale(5.0f);
	brickwall.translate(glm::vec3{-5.0f, 0.0f, -5.0f});

	// grass (using brickwall 2d vertices)
	gl_texture grass_tex;
	grass_tex.bind(GL_TEXTURE20);
	grass_tex.loadData("/home/hungvu/Archive/progs/opengl/texture/grass.png", aiTextureType_DIFFUSE);
	model grass{brickwall_model};

	// mirror 
	model mirror{mirror_model, 1};

	// program
	gl_program program;

	// load shader and program
	program.attachCompiledShader(&vertex_shader);
	program.link();
	program.use();
	program.printDebugUniform(0);
	program.printDebugAttachedShadersID(0);

	// misc rendering modes
	// stencil test apply on another skull mother
	// problem
	// model tree;
	// colors are visible on top the of the skull where lights are not suppose to be visible (no scale of the skull model), because the normal are normalized ?

	// stride = 0
	// std::vector<DrawElementsIndirectCommand> command;
	// gl_vao static_vao;
	// static_vao.attribFormat(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
	// static_vao.attribFormat(1, 3, GL_FLOAT, GL_FALSE, 32, (void*)12);
	// static_vao.attribFormat(2, 2, GL_FLOAT, GL_FALSE, 32, (void*)24);
	// gl_vbo static_vbo;
	// static_vao.bind();
	// static_vbo.bind();
	// model tree;
	// tree.loadData("/home/hungvu/Archive/progs/opengl/model/tree/Tree1/Tree1.obj", 1);


	// rendering loop
	double time, prevtime = 0;
	while(!glfwWindowShouldClose(contx.getWindow())){
		time = glfwGetTime();

		// if(camera::enable_debug_cam)
		// handle input and update data
		glfwSetCursorPosCallback(contx.getWindow(), handleFunc::cursorPos);
		glfwSetKeyCallback(contx.getWindow(), handleFunc::keystroke);

		// errors callback
		glDebugMessageCallback(MessageCallback, 0);

		// feed inputs to imgui and render imgui onto the screen
		// ImGui_ImplOpenGL3_NewFrame();
		// ImGui_ImplGlfw_NewFrame();
		// ImGui::NewFrame();
		// ImGui::Text("hello,asdfasdfadsf");
		// ImGui::Render();
		// ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// clear framebuffer from previous rendering, reset stencil/ depth test parameter to default
		glEnable(GL_STENCIL_TEST);
		glStencilMask(0xFF); // mask all 8 bits to 1 to clear stencil buffer to 0
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClearStencil(0x00);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// camera
		camera::projection = glm::perspective(glm::radians(camera::yfov), (const float)contx.width() / contx.height(), 0.1f, 100.0f);
		camera::view = glm::lookAt(camera::position, camera::position + camera::direction, camera::up);

		// STENCILLING STAGE ------------------------------------------------
		// draw object of interest normally
		for(model &m : models)
			drawWithLights(m, lights, program, &mat_fshader);
		//  draw the wireframe viewer
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		brickwall.draw(program, &nomat_fshader);

		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
		for(model &m : models)
			drawWithLights(m, lights, program, &mat_fshader);
		// reset tests to default
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glStencilFunc(GL_KEEP, GL_KEEP, GL_KEEP);
		// STENCILLING STAGE ------------------------------------------------

		// manually draw grass
		program.attachCompiledShader(&grass_fshader);
		program.assignUniform("material.diffuse0", glUniform1i, grass_tex.getUnit());
		grass.draw(program, &grass_fshader);

		for(model &m : axis){
			m.draw(program, &nomat_fshader);
		}

		lights[0].translate(20.0f * glm::vec3{std::cos(time), std::sin(time), 0.0f});
		lights[1].translate(20.0f * glm::vec3{0.0f, std::cos(time), std::sin(time)});
		lights[2].translate(20.0f * glm::vec3{std::cos(time), 0.0f, std::sin(time)});
		for(light_model& light : lights){
			light.draw(program, &nomat_fshader);
		}

		program.printDebugUniform(0);
		program.printDebugAttachedShadersID(0);

		glfwSwapBuffers(contx.getWindow());
		glfwPollEvents();

		// display frame/s in the window's title
		// round to the third decimal place
		std::cout.precision(3);
		std::ostringstream sstr;
		sstr << "Window Title. " << 1 / (time - prevtime) << " fps";
		contx.assignWindowTitle(sstr.str());
		prevtime = time;
	}
	// Shutdown imgui
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();

	return 0;
}
