/* GLW means wrapper using gl headers */
#ifndef MODEL_H
#define MODEL_H
#include "./gl_vao.h"
#include "./gl_vbo.h"
#include "./gl_texture.h"
#include "./gl_shader.h"
#include "./gl_program.h"
#include "./camera.h"
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>
#include <array>
#include <unordered_map>

typedef void (*ImGuiMarkerCallback)(const char* file, int line, const char* section, void* user_data);
// global cross-unit variable, initizalied in model.cpp
extern ImGuiMarkerCallback GImGuiMarkerCallback;

// global macros
#define IMGUI_MARKER(section)  do { if (GImGuiMarkerCallback != nullptr) GImGuiMarkerCallback(__FILE__, __LINE__, section, nullptr); } while (0)
#define fval_ptr(arg) const_cast<const float*>(glm::value_ptr(arg))
#define tocstr(arg) std::string(arg).c_str()

class model{
	public:

		model() = default;

		// normal ctor with path to the model. flipUV to flip the uv mapping of texture
		// coord to OpenGL top-left convention, default is enabled
		model(const std::string &path, bool flipUV = 1);

		// copy ctor
		model(const model &m);

		// copy assigment op
		model& operator=(const model &m);

		// no moving of model object since there are many mem vars for explicit
		// construction through args as well as no need to move one object resource to
		// another
		model(model &&m) = delete;
		model& operator=(model &&m) = delete;

		// dtor
		virtual ~model() noexcept;

		// load data with path to the model. flipUV to flip the uv mapping of texture
		// coord to OpenGL top-left convention, default is enabled
		void loadData(const std::string &path, bool flipUV = 1);

		// render the model with the currently attached fragment shader
		void draw(gl_program &program, const gl_shader *fshader, const glm::mat4 &projection = camera::projection, const glm::mat4 &view = camera::view, GLenum usage = GL_STATIC_DRAW);

		// the first operation applied to the identity model matrix.
		// Uniformly scale the model. Default is 1.0f (no scaling of the
		// model) if no argument is given.
		void scale(float factor);

		void nonUniformScale(glm::vec3 factor);

		void reflectZ(); // reflect over Z axis

		// the second operation applied to the identity model matrix. Rotate the
		// model 'radian' angle around the given vector. Default is axis =
		// glm::vec3{1.0f, 0.0f, 0.0f}, radian = 0 (no rotation of the model)
		// if no arguments are given.
		// void rotate(const glm::vec3 &axis, float radian);
		void rotate(float angle_x, float angle_y, float angle_z);

		// the third operation applied to the identity model matrix. Translate
		// the model to the given vector position. Default is glm::vec3{0.0f,
		// 0.0f, 0.0f} (no translating of the model) if no argument is given.
		void translate(const glm::vec3 &vec);

		// change the current vertex color
		void assignVertexColor(const glm::vec4 &vcolor);

		// change the model's original looking direction
		void assignDirection(const glm::vec3 &dir);

		glm::vec4 getVertexColor() const;

		glm::vec3 getDirection() const;

		glm::vec3 getPosition();

		// return the depth value of the model in the view space
		float getViewDepth(const glm::mat4 &view = camera::view);

		// *ImGui function exposes the private member variables to ImGuifor 
		// modification. Only using these functions in the rendering loop and 
		// after glClear
		void exposeModelMatrixOpImGui();


	private:

		// each model's mesh are drawn independantly since drawing the whole model messed up the texture
		struct mesh{
			// store continous vertices data. position (3), normal (3), texcoord(2) per vertex
			std::vector<float> vbuf;
			// ebo buffer, stores continous faces indicies. 3 indices per face
			std::vector<unsigned> ebuf;
			// textures for this mesh
			std::vector<std::shared_ptr<gl_texture>> material;
			// shininess level for the mesh's phong-specularity calculation
			float shininess = -1.0f;
		};

		static GLenum current_tex_unit;

		// texture manager. A map contains pairs of texture info and a shared
		// ptr to a gl_texture object. Used to get already loaded texture
		// object for the material vector. We use the shared ptr to share the
		// loaded texture with other model objects and avoid problem of making
		// a new openGL object name for a local object and assign it to a
		// non-local object, making the dtor called on the non-local object.
		static std::unordered_map<std::string, std::shared_ptr<gl_texture>> textures_map;

		 // textures used by this model
		std::unordered_map<std::string, std::shared_ptr<gl_texture>> model_textures;

		// mesh manager. A map contains pairs of path to model data and a ptr
		// to a vector of meshes related to the model. Used to get an already
		// loaded model meshes for the *meshes. Use shared ptr due to the reason
		// identicial to the texmap
		static std::unordered_map<std::string, std::shared_ptr<std::vector<mesh>>> meshes_map;

		// a structure contains each mesh, we render mesh of the model
		// independently (local meshes)
		std::shared_ptr<const std::vector<mesh>> model_meshes {nullptr};

		// default grey color for untextured model
		glm::vec4 vertex_color {0.5f, 0.5f, 0.5f, 0.0f};

		// default looking direction
		glm::vec3 direction {0.0f, 0.0f, 0.0f};

		gl_vao vao;

		gl_vbo vbo;

		std::string model_path;

		// the model's assigned parent directory name in the model path
		// ie: ".../backpack/backpack.obj" -> model_name = backpack
		std::string model_name;

		// model matrix for transforming local vertex to world vertex, default
		// is an identity matrix. glm::mat4 projection and view matricies are in
		// the camera file.
		glm::mat4 modelmatx{1.0f};
		std::array<glm::mat4, size_t(3)> modelmatx_op{glm::mat4{1.0f}, glm::mat4{1.0f}, glm::mat4{1.0f}};
		float scale_factor;
		glm::vec3 rotate_angle, translate_vec;
		bool modelmatx_op_modified = 0;

		// iterating through each node (mesh) in the scene (model) and process
		// all the meshes data within
		void processNodes(std::vector<mesh> &meshes, const aiScene *scene, const aiNode *n);

		// load mesh data into the part vbo buffer (vbuf) and ebo buffer (ebuf)
		void loadMeshesData(std::vector<mesh> &meshes, const aiScene *scene, const aiMesh &unproc_mesh);

		// load vertex attrib data into vbo buffer
		void loadVertices(const aiMesh &mesh, std::vector<float>& vbuf);

		// load indices data into ebo buffer
		void loadFaces(const aiMesh &mesh, std::vector<unsigned>& ebuf);

		// load material data into a mesh m
		void loadMaterial(aiMaterial *mat, mesh &m);

		// load texture data into a mesh m, used inside loadMaterial
		void loadTexture(const std::string &tex_path, aiTextureType tex_type);

		// assign uniform in all shader stages (attached vertex shader outside 
		// draw(), and supplied fragment shader)
		void assignUniforms(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view);

		// assign the uniform mat vm_mat (view * model), and pvm_mat
		// (projection * vm_mat) in vertex shader for transformation of vertex
		// postion
		void assignVertexUniformTransMat(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view);

		// looping through each texture in the material and assign it texture unit
		// to a sampler in the fragment shader.
		// void assignFragmentUniformSamplers(gl_program &program, const std::vector<std::shared_ptr<gl_texture>> &material);
		void assignFragmentUniformSamplers(gl_program &program);

		// reconstruct model matrix is the matrix list is modified
		void constructModelMatrix();
};

#endif // MODEL_H
