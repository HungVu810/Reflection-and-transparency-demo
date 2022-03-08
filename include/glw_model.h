/* GLW means wrapper using gl headers */
#ifndef GLW_MODEL_H
#define GLW_MODEL_H
#include "./gl_shader.h"
#include "./gl_program.h"
#include "./gl_vao.h"
#include "./gl_vbo.h"
#include "./gl_texture.h"
#include "./runtime_except.h"
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <cassert>

class glw_model{
	public:
		// ctor with all mapping, the mapings are optional (assuming the user want to assign the color via fragment shader)
		glw_model(const std::string &model_path, std::vector<gl_texture> &model_material, bool flipUV){
			Assimp::Importer imp;
			importSceneModel(imp, model_path, flipUV);
			processModelData(scene->mRootNode);
			loadMaterial(model_material);
		};

		glw_model(const std::string &model_path, const glm::vec4 &vcolor){
			Assimp::Importer imp;
			vertex_color = std::move(vcolor);
			importSceneModel(imp, model_path, 0);
			processModelData(scene->mRootNode);
		};
		// move ctor
		// copy ctor
		// move assignment
		// copy assigment
		// dtor
		~glw_model(){};

		// Unload model data from vbo and ebo. Only do this when necessary
		// because changing OpenGL state will slow down the rendering
		// void unload(){ model_loaded = false; }

		void draw(gl_program &program, GLenum usage){
			// boolean to check for if the model is already loaded
			// If draw more than one model, load all to the vao and vbo
			// If a model is not needed, render it somewhere else but don't delete it 
			// program.changeShader(..., m.fshader);
			// FSHADER DURING THE LOADMESHDATA FUNCTION ?
			// program.attachShader(m.fshader);

			// uniform structure to manage assigned uniform without having to reload (static, and dynamic structure)

			// draw(gl_vao &vao, gl_vbo &vbo, gl_program &progam, GLenum usage) to share vao, vbo with multiple model 
			// or different model have their own unique vao and vbo

			program.assignUniform("material.use_vertex_color", glUniform1i, (int)material.empty());
			program.assignUniform("material.vertex_color", glUniform4fv, 1, const_cast<const float*>(glm::value_ptr(vertex_color)));
			for(const gl_texture &t : material){
				program.assignUniform(t.getSamplerName().c_str(), glUniform1i, t.getUnit());
			}

			for(const part &p : model){
				vao.bind();
				vbo.bind();
				vbo.loadData(p.vbuf, usage);
				vao.eboData(p.ebuf, usage);
				// why 64, not 32 
				vao.attribFormat(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
				vao.attribFormat(1, 3, GL_FLOAT, GL_FALSE, 32, (void*)12);
				vao.attribFormat(2, 2, GL_FLOAT, GL_FALSE, 32, (void*)24);

				// GLGETTEXTURE AND GLGETBUFFERIV
				// batch drawing ?
				glDrawElements(GL_TRIANGLES, p.ebuf.size(), GL_UNSIGNED_INT, (void*)0);
			}
		};
	private:
		//TODO; load each object/part (aiNode) of the model into the buffer independantly and draw them (tested by isolating each i) will load each part of the model fully. While load all aiNode object into the buffer all at once messed up the model. Tested by process a certain i children node without looping through everything in mChildren.
		//TODO: how to deal with model that is not mapped to a texture. Make sure that the vertex does have an normal vector, otherwise assign (0, 0, 0) to the normal
		struct part{
			std::vector<float> vbuf; // vbo buffer, store continous vertices data. position (3), normal (3), texcoord(2) per vertex
			std::vector<unsigned> ebuf; // ebo buffer, stores continous faces indicies. 3 indices per face
		};
		glm::vec4 vertex_color = glm::vec4{0.0f, 0.0f, 0.0f, 0.0f};
		const aiScene *scene = nullptr;
		// map<string, gl_texture> material, run the original loadMaterial() on each mesh and look up the texture path as key
		std::vector<gl_texture> material;  // material (contains all texture types and data)
		std::vector<part> model;
		gl_vao vao;
		gl_vbo vbo;

		void loadMaterial(std::vector<gl_texture> &model_material){
			material = std::move(model_material);
			for(size_t i = 0; i < material.size(); i++){
				material[i].bind(GL_TEXTURE0 + i);
				material[i].loadData();
			}
		};

		void importSceneModel(Assimp::Importer &imp, const std::string &model_path, bool flipUV){
			// TODO: skull uv need to be fliped while the guitar doesn't (flip texture if the texture look mess up, flip/don't flip the texture)
			if(flipUV)
				scene = imp.ReadFile(model_path, aiProcess_Triangulate | aiProcess_FlipUVs);
			else scene = imp.ReadFile(model_path, aiProcess_Triangulate);
			assert(scene);
		};

		void processModelData(aiNode *n){
			// process meshes within in node n
			for(unsigned i = 0; i < n->mNumMeshes; i++){
				// a mesh is a part within a model
				loadMeshData(*scene->mMeshes[n->mMeshes[i]]);
			}
			// process children nodes, 4 max
			for(unsigned i = 0; i < n->mNumChildren; i++){
				processModelData(n->mChildren[i]);
			}
		};

		void loadMeshData(const aiMesh &mesh){
			part p;
			loadVertices(mesh, p.vbuf);
			loadFaces(mesh, p.ebuf);
			model.push_back(p);
		};

		void loadVertices(const aiMesh &mesh, std::vector<float>& vbuf){
			for(size_t i = 0; i < mesh.mNumVertices; i++){
				// vertex
				vbuf.push_back(mesh.mVertices[i].x);
				vbuf.push_back(mesh.mVertices[i].y);
				vbuf.push_back(mesh.mVertices[i].z);
				// normal
				vbuf.push_back(mesh.mNormals[i].x);
				vbuf.push_back(mesh.mNormals[i].y);
				vbuf.push_back(mesh.mNormals[i].z);
				// texcoord
				vbuf.push_back(mesh.mTextureCoords[0][i].x);
				vbuf.push_back(mesh.mTextureCoords[0][i].y);
			}
		};

		void loadFaces(const aiMesh &mesh, std::vector<unsigned>& ebuf){
			for(size_t i = 0; i < mesh.mNumFaces; i++){
				for(size_t j = 0; j < mesh.mFaces[i].mNumIndices; j++){
					ebuf.push_back(mesh.mFaces[i].mIndices[j]);
				}
			}
		};
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

		// void loadMaterial(){
		// 	// texture loading, binding process slow down the program at lot, not vertex data loading
		// 	// destructed gl_texture obj cause sampler to lost its data (blacked texture color)
		// 	diff.loadData("/home/hungvu/Archive/progs/opengl/model/diffuse.jpg", aiTextureType_DIFFUSE);
		// 	// spec.loadData("/home/hungvu/Archive/progs/opengl/model/specular.jpg", aiTextureType_SPECULAR);
		// 	diff.bind(GL_TEXTURE0);
		// 	// spec.bind(GL_TEXTURE1);
		// };
};

#endif // GLW_MESH_H
