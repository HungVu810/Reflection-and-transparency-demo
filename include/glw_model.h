/* GLW means wrapper using gl headers */
#ifndef GLW_MODEL_H
#define GLW_MODEL_H
#include "./gl_shader.h"
#include "./gl_program.h"
#include "./gl_vao.h"
#include "./gl_vbo.h"
#include "./gl_texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

class glw_model{
	public:
		// enum TEXTURE_TYPE = {AMBIENT, EMMISION, DIFFUSE, SPECULAR};
		glw_model(std::string &model_path){
			Assimp::Importer imp;
			scene = imp.ReadFile(model_path, aiProcess_FlipUVs);
			if(!scene){
				std::cerr << imp.GetErrorString() << std::endl;
				std::exit(EXIT_FAILURE);
				std::string x;
			}
			processModelData(scene->mRootNode);
		};
		~glw_model(){
		};

		// TODO: seperate the code into different modules after testing
		// private
		void processModelData(aiNode* n){
			// process meshes within in node n
			for(size_t i = 0; i < n->mNumMeshes; i++){
				// aiMesh m = scene->mMeshes[n->mMeshes[i]]
				loadMeshData(*scene->mMeshes[n->mMeshes[i]]);
			}
			// process children nodes
			for(size_t i = 0; i < n->mNumChildren; i++){
				processModelData(n->mChildren[i]);
			}
		}
		// private
		// loadMeshData(const aiMesh &mesh, const gl_shader &mesh_shader){
		void loadMeshData(const aiMesh &mesh){
			glw_mesh m;
			// how to deal with model that is not mapped to a texture. Make sure that the vertex does have an normal vector, otherwise assign (0, 0, 0) to the normal
			// loadMeshVertices(mesh);
			// loadMeshFaces(mesh);
			// loadMeshMaterial
			// vertices
			vertex v;
			for(size_t i = 0; i < mesh.mNumVertices; i++){
				v.position.x = mesh.mVertices[i].x;
				v.position.y = mesh.mVertices[i].y;
				v.position.z = mesh.mVertices[i].z;
				v.normal.x = mesh.mNormals[i].x;
				v.normal.y = mesh.mNormals[i].y;
				v.normal.z = mesh.mNormals[i].z;
				v.texcoord.x = mesh.mTextureCoords[0][i].x;
				v.texcoord.y = mesh.mTextureCoords[0][i].y;
				m.vertices.push_back(v);
			}
			// faces
			face f;
			for(size_t i = 0; i < mesh.mNumFaces; i++){
				for(size_t j = 0; j < mesh.mFaces[i].mNumIndices; j++){
					f.indices.push_back(mesh.mFaces[i].mIndices[j]); // implicit convert mIndices to f without having to goes through j index
				}
				m.faces.push_back(f);
			}
			// material
			aiMaterial *material = scene->mMaterials[mesh.mMaterialIndex];
			std::string mPath{"/home/hungvu/Archive/progs/opengl/model/"};
			aiTextureType texTypeArray[]={/*aiTextureType_AMBIENT,*/ aiTextureType_DIFFUSE, aiTextureType_SPECULAR, /*aiTextureType_EMISSIVE*/};
			for(size_t i = 0; i < sizeof(texTypeArray) / sizeof(aiTextureType); i++){
				for(size_t j = 0; j < material->GetTextureCount(texTypeArray[i]); j++){
					gl_texture tex;
					// std::string tPath = mPath + texTypeName[i].c_str();
					// aiString texPath{tPath};
					aiString texPath;
					material->GetTexture(texTypeArray[i], j, &texPath);
					tex.loadData((mPath + std::string{texPath.C_Str()}).c_str(), texTypeArray[i]);
					m.textures.push_back(tex);
				}
			}
			// fshader ?

			// add mesh to the model structure
			meshes.push_back(m);
		}

        void draw(gl_program &program, GLenum usage){
			// boolean to check for if the model is already loaded
			// If draw more than one model, load all to the vao and vbo
			// If a model is not needed, render it somewhere else but don't delete it 
			gl_vao vao;
			gl_vbo vbo;
			vao.bind();
			vbo.bind();
			for(glw_mesh &m : meshes){
				vbo.loadData(m.vertices, usage);
				vao.eboData(m.faces, usage);
				vao.attribData(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)offsetof(struct vertex, position));
				vao.attribData(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)offsetof(struct vertex, normal));
				vao.attribData(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)offsetof(struct vertex, texcoord));
				// program.changeShader(..., m.fshader);
				// FSHADER DURING THE LOADMESHDATA FUNCTION ?
				// program.attachShader(m.fshader);
				GLenum tex_unit = GL_TEXTURE0; // reset texture unit for the next mesh texture
				unsigned mAmb = 0, mDiff = 0, mSpec = 0;
				for(size_t i = 0; i < m.textures.size(); i++, tex_unit++){
					m.textures[i].bind(tex_unit);
					std::string shaderMatId;
					switch(m.textures[i].getType()){
						case aiTextureType_AMBIENT: shaderMatId = std::string{"material.ambient" + std::to_string(mAmb++)}; break; // append i to material.x string
						// case aiTextureType_EMISSIVE: program.assignUniform(std::string{"material.emission", std::to_string(i)}, glUniform1i, tex_unit); break;
						case aiTextureType_DIFFUSE: shaderMatId = std::string{"material.diffuse" + std::to_string(mDiff++)}; break;
						case aiTextureType_SPECULAR: shaderMatId = std::string{"material.specular" + std::to_string(mSpec++)}; break;
					}
					// assign actived texture unit to the sampler in the fragment shader
					program.assignUniform(shaderMatId.data(), glUniform1i, (int)(tex_unit - GL_TEXTURE0));
				}

				// GLGETTEXTURE AND GLGETBUFFERIV

				// batch drawing ?
				glDrawElements(GL_TRIANGLES, vao.eboNumIndices(), GL_UNSIGNED_INT, (void*)0);
			}
		}
	public:
		const aiScene *scene;
		struct glw_mesh{
			// ctor for implicit cast from ai3Dvector to vector[3]
			std::vector<vertex> vertices;
			std::vector<face> faces;
			std::vector<gl_texture> textures;
			// gl_shader fshader;
		};
		std::vector<glw_mesh> meshes;
		// one model have multiple meshes, each mesh may have different fragment shader style -> batching draw call, prepare all draw call for each mesh and call once for the whole model
		// std::vector<gl_texture> material;
};

#endif // GLW_MESH_H
