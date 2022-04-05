#include "../include/model.h"
#include "../include/runtime_except.h"
#include "../include/camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>
#include <iostream>

// init statics ==================================================================
GLenum model::current_tex_unit = GL_TEXTURE0;
std::unordered_map<std::string, std::shared_ptr<gl_texture>> model::textures_map;
std::unordered_map<std::string, std::shared_ptr<std::vector<model::mesh>>> model::meshes_map;

// public ==================================================================

// normal ctor with path to the model. flipUV to flip the uv mapping of texture
// coord to OpenGL top-left convention, default is enabled
model::model(const std::string &path, bool flipUV){
	loadData(path, flipUV);
};

// TODO: ------------------------- maintain these functions
// copy ctor
model::model(const model &m){
	// don't copy model mat, the object will be at its default state
	model_meshes = m.model_meshes;
	vertex_color = m.vertex_color;
	direction = m.direction;
	modelmatx = glm::mat4{1.0f};
};

// copy assigment op
model& model::operator=(const model &m){
	// don't copy model_mat, the object will be at its defauly state
	model_meshes = m.model_meshes;
	vertex_color = m.vertex_color;
	direction = m.direction;
	modelmatx = glm::mat4{1.0f};
	return *this;
};
// TODO: ------------------------- maintain these functions

// dtor
model::~model() noexcept {};

// Render the model with the currently attached fragment shader
void model::draw(gl_program &program, const gl_shader *fshader, const glm::mat4 &projection, const glm::mat4 &view, GLenum usage){
	assert(fshader && fshader->getType() == GL_FRAGMENT_SHADER);
	program.attachCompiledShader(fshader);
	assignUniforms(program, projection, view);

	// TODO: MAKE VAO, VBO IN MAIN AND PASS AS ARGYMENT
	vao.bind();
	vbo.bind();
	vao.attribFormat(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
	vao.attribFormat(1, 3, GL_FLOAT, GL_FALSE, 32, (void*)12);
	vao.attribFormat(2, 2, GL_FLOAT, GL_FALSE, 32, (void*)24);
	assert(model_meshes);

	// per-mesh drawing
	for(const mesh &m : *model_meshes){
		vbo.loadData(m.vbuf, usage);
		vao.eboData(m.ebuf, usage);
		if(m.shininess != -1)
			program.assignUniform("material.shininess", glUniform1f, m.shininess);
		glDrawElements(GL_TRIANGLES, m.ebuf.size(), GL_UNSIGNED_INT, (void*)0);
	}

	// batch drawing
	//std::vector<float> meshes_vbuf;
	//std::vector<std::vector<unsigned>> meshes_ebuf;
	//// use for multidraw call
	//std::vector<GLsizei> meshes_ebuf_count;
	//// size_t meshes_ebuf_total_unsigned = 0;
	//for(const mesh &m : *model_meshes){
	//	meshes_vbuf.insert(m.vbuf.begin(), m.vbuf.begin(), m.vbuf.end());
	//	// meshes_ebuf_total_unsigned += m.ebuf.size();
	//	meshes_ebuf.push_back(m.ebuf);
	//	meshes_ebuf_count.push_back(m.ebuf.size());
	//	// TODO: shininess for each meshes in the shader like shiniess0, shininess1 ?
	//	// if(m.shininess != -1)
	//	// 	program.assignUniform("material.shininess", glUniform1f, m.shininess);
	//	// TODO: shininess for each meshes in the shader like shiniess0, shininess1 ?
	//}

	////TODO: change vbo to stream draw for dyamic object, and static draw for static object (1 vbo or static, 1 vbo or dynamic, do not reload data for the static one)
	//vbo.loadData(meshes_vbuf, usage);
	////TODO: ebo is always static
	//// TODO: construct ebuf_multidraw during loadFaces and pass in here as argument
	//vao.eboData(meshes_ebuf, usage);
	//// glDrawElements(GL_TRIANGLES, m.ebuf.size(), GL_UNSIGNED_INT, (void*)0);
	//glMultiDrawElements(GL_TRIANGLES, meshes_ebuf_count.data(), GL_UNSIGNED_INT, (void *const *)(0), model_meshes->size());
};


// the first operation applied to the identity model matrix. Uniformly scale
// the model. Default is 1.0f (no scaling of the model) if no argument is
// given.
void model::scale(float factor){
	modelmatx_op[0] = glm::scale(glm::mat4{1.0f}, glm::vec3{factor});
	modelmatx_op_modified = 1;
};

// the second operation applied to the identity model matrix. Rotate the
// model 'radian' angle around the given vector. Default is axis =
// glm::vec3{1.0f, 0.0f, 0.0f}, radian = 0 (no rotation of the model)
// if no arguments are given.
void model::rotate(const glm::vec3 &axis, float radian){
	modelmatx_op[1] = glm::rotate(glm::mat4{1.0f}, radian, axis);
	modelmatx_op_modified = 1;
};

// the third operation applied to the identity model matrix. Translate
// the model to the given vector position. Default is glm::vec3{0.0f,
// 0.0f, 0.0f} (no translating of the model) if no argument is given.
void model::translate(const glm::vec3 &v){
	modelmatx_op[2] = glm::translate(glm::mat4{1.0f}, v);
	modelmatx_op_modified = 1;
};

// change the current vertex color
void model::assignVertexColor(const glm::vec4 &vcolor){
	vertex_color = vcolor;
};

// change the model's original looking direction.
void model::assignDirection(const glm::vec3 &dir){
	direction = dir;
};

glm::vec4 model::getVertexColor() const{
	return vertex_color;
};

glm::vec3 model::getDirection() const{
	return direction;
};

// the position of the object based on the constructed model matrix from
// scale(), rotate() and tranlate() operation
glm::vec3 model::getPosition(){
	// extract the final translation vector colume and discard the w-component
	constructModelMatrix();
	return modelmatx[3];
};


float model::getViewDepth(const glm::mat4 &view){
	constructModelMatrix();
	// multiply with the view matrix to get a glm:;vec4, then return the depth
	// component at index 2
	return (view * modelmatx[3])[2];
};


// PRIVATE ==================================================================

// get the scene pointer and process model data
void model::loadData(const std::string &path, bool flipUV){
	model_path = path;
	// look up the meshes map to check for preloaded model's meshes
	auto meshes_iter = model::meshes_map.find(path);
	std::shared_ptr<std::vector<mesh>> mptr;
	if(meshes_iter != model::meshes_map.end()){
		mptr = meshes_iter->second;
	}
	else{
		// the meshes is new, we load and insert it into meshes_map. Otherwise
		// pointer our meshes to the value in the map
		Assimp::Importer imp;
		const aiScene *scene {nullptr};
		if(flipUV)
			scene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		else
			scene = imp.ReadFile(path, aiProcess_Triangulate);
		if(!scene){
			std::cerr << path.c_str() << " is not a proper path to generate aiScene*" << std::endl;
			assert(0);
		}
		// create an new ptr to new meshes, load the data and use the iter
		// pointed to path to assign the to our meshes
		mptr = std::shared_ptr<std::vector<mesh>>{new std::vector<mesh>};
		model::meshes_map.insert({path, mptr});
		processNodes(*mptr, scene, scene->mRootNode);
	}
	model_meshes = mptr;
};

// iterating through each node in the scene (model) and process all the meshes
// data reference by each node
void model::processNodes(std::vector<mesh> &meshes, const aiScene *scene, const aiNode *n){
	// process meshes within in node n
	for(unsigned i = 0; i < n->mNumMeshes; i++){
		// a mesh is a part within a model
		unsigned mesh_index {n->mMeshes[i]};
		// load data for the meshes referenced by node n
		loadMeshesData(meshes, scene, *scene->mMeshes[mesh_index]);
	}
	// process children nodes, 4 max
	for(unsigned i = 0; i < n->mNumChildren; i++){
		processNodes(meshes, scene, n->mChildren[i]);
	}
};

// load mesh data into the part vbo buffer (vbuf) and ebo buffer (ebuf)
void model::loadMeshesData(std::vector<mesh> &meshes, const aiScene *scene, const aiMesh &unproc_mesh){
	// process the data in the unproc_mesh (unprocessed mesh) and load it into
	// our mesh m's vbuf and ebuf for the vao and vbo
	mesh m;
	loadVertices(unproc_mesh, m.vbuf);
	loadFaces(unproc_mesh, m.ebuf);
	loadMaterial(scene->mMaterials[unproc_mesh.mMaterialIndex], m);
	meshes.push_back(m);
};

// load into vbo buffer
void model::loadVertices(const aiMesh &unproc_mesh, std::vector<float> &vbuf){
	// vbuf.reserve(unproc_mesh.mNumVertices);
	for(size_t i = 0; i < unproc_mesh.mNumVertices; i++){
		// vertex
		vbuf.push_back(unproc_mesh.mVertices[i].x);
		vbuf.push_back(unproc_mesh.mVertices[i].y);
		vbuf.push_back(unproc_mesh.mVertices[i].z);
		// normal
		vbuf.push_back(unproc_mesh.mNormals[i].x);
		vbuf.push_back(unproc_mesh.mNormals[i].y);
		vbuf.push_back(unproc_mesh.mNormals[i].z);
		// texcoord
		vbuf.push_back(unproc_mesh.mTextureCoords[0][i].x);
		vbuf.push_back(unproc_mesh.mTextureCoords[0][i].y);
	}
};

// load into ebo buffer
void model::loadFaces(const aiMesh &unproc_mesh, std::vector<unsigned> &ebuf){
	// ebuf.reserve(unproc_mesh.mNumFaces * 3);
	for(size_t i = 0; i < unproc_mesh.mNumFaces; i++){
		for(size_t j = 0; j < unproc_mesh.mFaces[i].mNumIndices; j++){
			ebuf.push_back(unproc_mesh.mFaces[i].mIndices[j]);
		}
	}
};

void model::loadMaterial(aiMaterial *mat, mesh &m){
	if(mat){
		// load mesh shininess if available
		mat->Get(AI_MATKEY_SHININESS, m.shininess);
		// load gl_texture used with this mesh
		aiString tex_fname;
		std::array<aiTextureType, 2> tex_type {aiTextureType_DIFFUSE, aiTextureType_SPECULAR};
		// extract the path to parent directiony from the given model_path
		std::string tex_path = model_path;
		tex_path.erase(model_path.rfind('/') + 1);
		for(unsigned i = 0; i < tex_type.size(); i++){
			for(unsigned j = 0; j < mat->GetTextureCount(tex_type[i]); j++){
				// get file name of the texture
				if(AI_SUCCESS == mat->Get(AI_MATKEY_TEXTURE(tex_type[i], j), tex_fname)){
					// process tex_fname in case of Window-formated path
					// (ie: D:\\home) convention
					std::string proc_texFname = tex_fname.C_Str();
					size_t cindex = proc_texFname.rfind('\\');
					proc_texFname = proc_texFname.substr(cindex + 1, proc_texFname.size() - cindex);
					// create a full path to the texture by appending the path
					// to parent directory to the texture file name
					std::string tex_fullpath {tex_path};
					tex_fullpath += proc_texFname;
					loadTexture(tex_fullpath, tex_type[i]);
				}
			}
		}
	}
};

void model::loadTexture(const std::string &tex_fullpath, aiTextureType tex_type){
	// search the model textures map first, if not found then search in the
	// static textures map and push back into the model if avail. Otherwise,
	// create a new texture map and put it both in the model textures map and
	// the static textures map
	if(model_textures.find(tex_fullpath) == model_textures.end()){
		auto iter = textures_map.find(tex_fullpath);
		std::shared_ptr<gl_texture> tptr;
		if(iter != textures_map.end()){
			tptr = iter->second;
		}
		else{
			// this texture is unique (can't be found in textures_map), we create
			// new shared pointer to gl_texture with the key tex_fullpath in textures_map
			// and load the texture data
			try{
				tptr = std::shared_ptr<gl_texture>{new gl_texture};
				textures_map.insert({tex_fullpath, tptr});
			}
			catch(...){
				runtimeException();
			}
			// make sure there are still tex unit left to assign to this texture
			assert(model::current_tex_unit < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
			tptr->bind(model::current_tex_unit++);
			tptr->loadData(tex_fullpath, tex_type);
		}
		model_textures.insert({tex_fullpath, tptr});
	}

	// auto texture_iter {textures_map.find(tex_fullpath)};
	// std::shared_ptr<gl_texture> tptr;
	// if(texture_iter != textures_map.end()){
	// 	tptr = texture_iter->second;
	// }
	// else{
	// 	// this texture is unique (can't be found in textures_map), we create
	// 	// new shared pointer to gl_texture with the key tex_fullpath in textures_map
	// 	// and load the texture data
	// 	try{
	// 		tptr = std::shared_ptr<gl_texture>{new gl_texture};
	// 		model::textures_map.insert({tex_fullpath, tptr});
	// 	}
	// 	catch(...){
	// 		runtimeException();
	// 	}
	// 	// make sure there are still tex unit left to assign to this texture
	// 	assert(model::current_tex_unit < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
	// 	tptr->bind(model::current_tex_unit++);
	// 	tptr->loadData(tex_fullpath, tex_type);
	// }
	// m.material.push_back(tptr);
};

void model::assignUniforms(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view){
	assignVertexUniformTransMat(program, projection, view);
	program.assignUniform("vertex_color", glUniform4fv, 1, fval_ptr(vertex_color));
	assignFragmentUniformSamplers(program);
}


void model::assignVertexUniformTransMat(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view){
	constructModelMatrix();
	glm::mat4 vm_mat = view * modelmatx;
	glm::mat4 pvm_mat = projection * vm_mat;
	program.assignUniform(
			"vm_mat",
			glUniformMatrix4fv,
			1,
			static_cast<unsigned char>(GL_FALSE),
			fval_ptr(vm_mat)
			);
	program.assignUniform(
			"pvm_mat",
			glUniformMatrix4fv,
			1,
			static_cast<unsigned char>(GL_FALSE),
			const_cast<const float*>(glm::value_ptr(pvm_mat))
			);
};

// looping through each texture in the material and assign it texture unit
// to a sampler in the fragment shader.
// void model::assignFragmentUniformSamplers(gl_program &program, const std::vector<std::shared_ptr<gl_texture>> &material){
// 	// only assign sampler vars if the model is textured
// 	if(material.size()){
// 		static unsigned ambient = 0, diffuse = 0, specular = 0, emission = 0;
// 		for(size_t i = 0; i < material.size(); i++){
// 			std::string sampler_name;
// 			assert(material[i]);
// 			switch(material[i]->getType()){
// 				case aiTextureType_AMBIENT: sampler_name = std::string{"material.ambient" + std::to_string(ambient++)}; break;
// 				case aiTextureType_DIFFUSE: sampler_name = std::string{"material.diffuse" + std::to_string(diffuse++)}; break;
// 				case aiTextureType_SPECULAR: sampler_name = std::string{"material.specular" + std::to_string(specular++)}; break;
// 				case aiTextureType_EMISSIVE: sampler_name = std::string{"material.emission" + std::to_string(emission++)}; break;
// 				default: {
// 					std::cerr << "unknown aiTextureType" << std::endl;
// 					assert(0);
// 				}
// 			}
// 			program.assignUniform(sampler_name.c_str(), glUniform1i, material[i]->getUnit());
// 		}
// 	}
// };

void model::assignFragmentUniformSamplers(gl_program &program){
	// only assign sampler vars if the model is textured
	if(model_textures.size()){
		unsigned ambient = 0, diffuse = 0, specular = 0, emission = 0;
		for(auto &pair : model_textures){
			std::string sampler_name;
			switch(pair.second->getType()){
				case aiTextureType_AMBIENT: sampler_name = std::string{"material.ambient" + std::to_string(ambient++)}; break;
				case aiTextureType_DIFFUSE: sampler_name = std::string{"material.diffuse" + std::to_string(diffuse++)}; break;
				case aiTextureType_SPECULAR: sampler_name = std::string{"material.specular" + std::to_string(specular++)}; break;
				case aiTextureType_EMISSIVE: sampler_name = std::string{"material.emission" + std::to_string(emission++)}; break;
				default: {
					std::cerr << "unknown aiTextureType" << std::endl;
					assert(0);
				}
			}
			program.assignUniform(sampler_name.c_str(), glUniform1i, pair.second->getUnit());
		}
	}
};

// reconstruct model matrix is the matrix list is modified
void model::constructModelMatrix(){
	if(modelmatx_op_modified){
		modelmatx = glm::mat4{1.0f};
		// create and push a directional rotating billboard matrix into matx_list
		for(const glm::mat4& m : modelmatx_op){
			// model_mat = m * model_mat, not model_mat *= m
			modelmatx = m * modelmatx;
		}
		modelmatx_op_modified = 0;
	}
}
