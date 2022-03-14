/* GLW means wrapper using gl headers */
#ifndef MODEL_H
#define MODEL_H
#include "./gl_vao.h"
#include "./gl_vbo.h"
#include "./gl_texture.h"
#include "./gl_shader.h"
#include "./gl_program.h"
#include "./runtime_except.h"
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>

// static model manager
// class model_manager{
// 	public:
// 		model_manager() = default;
// 		model_manager(size_t s){
// 			try{
// 				model_array = std::unique_ptr<model[]>(new model[s]);
// 			}
// 		};
// 		model_manager(){};
// 	private:
// 		std::unique_ptr<model[]> model_array;
// };

class model{
	 
	public:

		model() = default;

		// ctor with material
		model(const std::string &path, const std::vector<tex_info> &mat_info, bool flipUV);

		// ctor without material
		model(const std::string &path, const glm::vec4 &vcolor = {0.5f, 0.5f, 0.5f, 0.0f});

		// copy ctor
		model(const model&m);

		// move ctor
		model(model&&m);

		// copy assigment op
		model& operator=(const model&m);

		// move assignment op
		model& operator=(model&&m);

		// dtor
		~model() noexcept;

		// loadData with material
		void loadData(const std::string &path, const std::vector<tex_info> &mat_info, bool flipUV);

		// loadData without material
		void loadData(const std::string &path, const glm::vec4 &vcolor = {0.5f, 0.5f, 0.5f, 0.0f});

		// Renader the model with the currently attached fragment shader
		void draw(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view, GLenum usage);

		// Replace the currently attached fragment shader with the provided
		// 'fshader', relink the program and render the model
		void draw(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view, const gl_shader *fshader, GLenum usage);

		// uniformly scale the model
		void scale(float factor);

		// translate the model to the given vector position
		void translate(const glm::vec3 &v);

		// rotate the model 'radian' angle around the given vector
		void rotate(float radian, const glm::vec3 &v);

		glm::vec4 getVertexColor() const;

		// change the current vertex color
		void assignVertexColor(const glm::vec4 &vcolor);

	private:

		// each part of the model are drawn independantly since drawing the whole model messed up the texture
		struct part{
			// store continous vertices data. position (3), normal (3), texcoord(2) per vertex
			std::vector<float> vbuf;
			// ebo buffer, stores continous faces indicies. 3 indices per face
			std::vector<unsigned> ebuf;
		};

		// default grey color for untextured model
		glm::vec4 vertex_color{0.5f, 0.5f, 0.5f, 0.0f};

		 // material (contains all texture types and data)
		std::shared_ptr<gl_texture[]> material;
		size_t numTex = 0;

		// a structure contains each part, we render each of them independentlj
		std::vector<part> part_struct;

		gl_vao vao;

		gl_vbo vbo;

		// model matrix for transforming local vertex to world vertex, default
		// is an identity matrix. glm::mat4 projection and view matricies are in
		// the camera file.
		glm::mat4 model_mat{1.0f};

		// load the material information into this material vector
		void loadMaterial(const std::vector<tex_info> &mat_info);

		// get the scene pointer and process model data
		void loadModelData(const std::string &model_path, bool flipUV);

		// iterating through each node (part) in the scene (model) and process
		// all the meshes data within
		void processModelData(const aiScene *scene, aiNode *n);

		// load mesh data into the part vbo buffer (vbuf) and ebo buffer (ebuf)
		void loadMeshData(const aiMesh &mesh);

		// load into vbo buffer
		void loadVertices(const aiMesh &mesh, std::vector<float>& vbuf);

		// load into ebo buffer
		void loadFaces(const aiMesh &mesh, std::vector<unsigned>& ebuf);

		// assign the uniform mat vm_mat (view * model), and pvm_mat
		// (projection * vm_mat) in vertex shader for transformation of vertex
		// postion
		void assignVertexUniformTransMat(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view);

		// looping through each texture in the material and assign it texture unit
		// to a sampler in the fragment shader.
		void assignFragmentUniformSampler(gl_program &program);

		// CHECK IF THE TEXTURE IS ALREADY LOADED SO IT DOESN'T HAVE TO RELOAD THE SAME TEXTURE OBJECT
		// void loadMaterial(const aiMesh &mesh){
		// 	aiMaterial *material = scene->mMaterials[mesh.mMaterialIndex];
		// 	std::string mPath{"/home/hungvu/Archive/progs/opengl/model/"};
		// 	std::vector<aiTextureType> texType = {
		// 		/*aiTextureType_AMBIENT,*/
		// 		aiTextureType_DIFFUSE,
		// 		aiTextureType_SPECULAR,
		// 		/*aiTextureType_EMISSIVE*/
		// 	};
		// 	for(size_t i = 0; i < texType.size(); i++){
		// 		mbuf.push_back(material->GetTextureCount(texType[i]));
		// 		for(size_t j = 0; j < *mbuf.back(); j++){
		// 			gl_texture tex;
		// 			// std::string tPath = mPath + texTypeName[i].c_str();
		// 			// aiString texPath{tPath};
		// 			aiString texPath;
		// 			material->GetTexture(texType[i], j, &texPath);
		// 			// texPath.C_Str() is diffuse0 for example, rewrite the loadMaterial function to be more concrete
		// 			// TODO: std::string{texPath.C_Str()}).c_str(), rewrite
		// 			tex.loadData((mPath + std::string{texPath.C_Str()}).c_str(), texType[i]);
		// 			mat.push_back(tex);
		// 		}
		// 	}
		// };
};

#endif // MODEL_H
