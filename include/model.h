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
#include <list>
#include <unordered_map>

class model{
	 
	public:

		model() {};

		// ctor with material
		model(const std::string &path, const std::vector<tex_info> &mat_info, bool flipUV, const gl_shader *frag_shader);

		// ctor without material
		model(const std::string &path, const glm::vec4 &vcolor, const gl_shader *frag_shader);

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

		// loadData with material
		void loadData(const std::string &path, const std::vector<tex_info> &mat_info, bool flipUV, const gl_shader *frag_shader);

		// loadData without material
		void loadData(const std::string &path, const glm::vec4 &vcolor, const gl_shader *frag_shader);

		// Renader the model with the currently attached fragment shader
		void draw(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view, GLenum usage);

		// uniformly scale the model
		void scale(float factor);

		// translate the model to the given vector position
		void translate(const glm::vec3 &v);

		// rotate the model 'radian' angle around the given vector
		void rotate(float radian, const glm::vec3 &v);

		// Undo all transformation(s) on the model matrix
		void undoAllTransform();

		// Undo the last transformation on the model matrix.
		void undoLastTransform();

		// change the current vertex color
		void assignVertexColor(const glm::vec4 &vcolor);

		glm::vec4 getVertexColor() const;

		glm::vec3 getPosition();

	private:

		// each part of the model are drawn independantly since drawing the whole model messed up the texture
		struct part{
			// store continous vertices data. position (3), normal (3), texcoord(2) per vertex
			std::vector<float> vbuf;
			// ebo buffer, stores continous faces indicies. 3 indices per face
			std::vector<unsigned> ebuf;
			// shininess level for the part's phong-specularity calculation
			float shininess = -1;
		};

		static GLenum current_tex_unit;

		// texture manager. A map contains pairs of texture info and a shared
		// ptr to a gl_texture object. Used to get already loaded texture
		// object for the material vector. We use the shared ptr to share the
		// loaded texture with other model objects and avoid problem with
		// creating a local texture object, assign it to texmap (dtor invoked
		// on local texture) and do reference
		static std::unordered_map<std::string, std::shared_ptr<gl_texture>> texmap;

		// part manager. A map contains pairs of path to model data and a ptr
		// to a vector of parts related to the model. Used to get an already
		// loaded model parts for the *parts. Use shared ptr due to the reason
		// identicial to the texmap
		static std::unordered_map<std::string, std::shared_ptr<std::vector<part>>> partmap;

		 // material contains all texture types and texture data needed for the
		 // model
		std::vector<std::shared_ptr<const gl_texture>> material;

		// a structure contains each part, we render part of the model
		// independently
		std::shared_ptr<const std::vector<part>> parts = nullptr;

		// default grey color for untextured model
		glm::vec4 vertex_color{0.5f, 0.5f, 0.5f, 0.0f};

		gl_vao vao;

		gl_vbo vbo;

		// model matrix for transforming local vertex to world vertex, default
		// is an identity matrix. glm::mat4 projection and view matricies are in
		// the camera file.
		glm::mat4 modelmatx;
		std::list<glm::mat4> matx_list;
		bool matx_list_modified = 0;

		// fragment shader used for render the model
		const gl_shader *fshader = nullptr;

		// load the material information into this material vector
		void loadMaterial(const std::vector<tex_info> &mat_info);

		// get the scene pointer and process model data
		void loadModelData(const std::string &model_path, bool flipUV);

		// iterating through each node (part) in the scene (model) and process
		// all the meshes data within
		void processModelData(std::vector<part> &parts, const aiScene *scene, aiNode *n);

		// load mesh data into the part vbo buffer (vbuf) and ebo buffer (ebuf)
		void loadMeshData(std::vector<part> &parts, const aiScene *scene, const aiMesh &mesh);

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

		void constructModelMatrix();

		// construct a model matrix from the variables gathered in the
		// model_matrix namespace
		// glm::mat4 constructModelMatrix();

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
