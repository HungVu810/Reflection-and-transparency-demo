#include "../include/model.h"
#include "../include/runtime_except.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>
#include <iostream>

// init statics ==================================================================
GLenum model::current_tex_unit = GL_TEXTURE0;
std::unordered_map<std::string, std::shared_ptr<gl_texture>> model::texmap;
std::unordered_map<std::string, std::shared_ptr<std::vector<model::part>>> model::partmap;

// public ==================================================================

// ctor with material, 'path' is the path to a model file. 'mat_info' contains
// a list of different path to the omdel textures and their type
model::model(const std::string &path, const std::vector<tex_info> &mat_info, bool flipUV, const gl_shader *frag_shader){
	loadData(path, mat_info, flipUV, frag_shader);
};

// ctor without material
model::model(const std::string &path, const glm::vec4 &vcolor, const gl_shader *frag_shader){
	loadData(path, vcolor, frag_shader);
};

// TODO: ------------------------- maintain these functions
// copy ctor
model::model(const model &m){
	parts = m.parts;
	material = m.material;
	// numTex = m.numTex;
	vertex_color = m.vertex_color;
	fshader = m.fshader;
	// keep modelmatx, matx_list and matx_list_modified to default
};

// copy assigment op
model& model::operator=(const model &m){
	// don't copy model_mat, the object will always spawn at the center
	parts = m.parts;
	material = m.material;
	// numTex = m.numTex;
	vertex_color = m.vertex_color;
	fshader = m.fshader;
	return *this;
};
// TODO: ------------------------- maintain these functions

// dtor
model::~model() noexcept {};

// loadData with material
void model::loadData(const std::string &path, const std::vector<tex_info> &mat_info, bool flipUV, const gl_shader *frag_shader){
	loadModelData(path, flipUV);
	loadMaterial(mat_info);
	fshader = frag_shader;
};

// loadData without material
void model::loadData(const std::string &path, const glm::vec4 &vcolor, const gl_shader *frag_shader){
	loadModelData(path, 0);
	vertex_color = vcolor;
	fshader = frag_shader;
};

// Render the model with the currently attached fragment shader
void model::draw(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view, GLenum usage){
	program.attachCompiledShader(fshader);
	assert(parts);
	program.assignUniform("material.use_vertex_color", glUniform1i, (int)!material.size());
	program.assignUniform("material.vertex_color", glUniform4fv, 1, const_cast<const float*>(glm::value_ptr(vertex_color)));
	assignVertexUniformTransMat(program, projection, view);
	assignFragmentUniformSampler(program);
	vao.bind();
	vbo.bind();
	vao.attribFormat(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
	vao.attribFormat(1, 3, GL_FLOAT, GL_FALSE, 32, (void*)12);
	vao.attribFormat(2, 2, GL_FLOAT, GL_FALSE, 32, (void*)24);
	for(const part &p : *parts){
		vbo.loadData(p.vbuf, usage);
		vao.eboData(p.ebuf, usage);
		if(p.shininess != -1)
			program.assignUniform("material.shininess", glUniform1f, p.shininess);
		// batch drawing ?
		glDrawElements(GL_TRIANGLES, p.ebuf.size(), GL_UNSIGNED_INT, (void*)0);
	}
};

// uniformly scale the model
void model::scale(float factor){
	matx_list.push_back(glm::scale(glm::identity<glm::mat4>(), glm::vec3{factor, factor, factor}));
	matx_list_modified = 1;
};

// translate the model from the origin to the given vector position
void model::translate(const glm::vec3 &v){
	matx_list.push_back(glm::translate(glm::identity<glm::mat4>(), v));
	matx_list_modified = 1;
};

// rotate the model 'radian' angle around the given vector
void model::rotate(float radian, const glm::vec3 &v){
	matx_list.push_back(glm::rotate(glm::identity<glm::mat4>(), radian, v));
	matx_list_modified = 1;
};

// Undo all transformation on the model matrix
void model::undoAllTransform(){
	matx_list.clear();
}

// Undo the last transformation on the model matrix.
void model::undoLastTransform(){
	assert(!matx_list.empty());
	matx_list.pop_back();
}

// change the current vertex color
void model::assignVertexColor(const glm::vec4 &vcolor){
	vertex_color = vcolor;
};

glm::vec4 model::getVertexColor() const{
	return vertex_color;
};

glm::vec3 model::getPosition(){
	// extract the final translation vector colume and discard the w-component
	constructModelMatrix();
	return modelmatx[3];
};

// PRIVATE ==================================================================

// load the material information into this material vector
void model::loadMaterial(const std::vector<tex_info> &mat_info){
	for(const tex_info &texinfo : mat_info){
		// look up the material map to check for preloaded material(s)
		auto tex_iter = model::texmap.find(texinfo.path);
		if(tex_iter == model::texmap.end()){
			assert(model::current_tex_unit < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
			model::texmap.insert({texinfo.path, std::shared_ptr<gl_texture>{new gl_texture}});
			tex_iter = model::texmap.find(texinfo.path);
			tex_iter->second->bind(model::current_tex_unit++);
			tex_iter->second->loadData(texinfo);
			// // use reserve with allocator
			// numTex = mat_info.size();
			// try{
			// 	// material = std::make_shared<gl_texture[]>();
			// 	material = std::shared_ptr<gl_texture[]>(new gl_texture[mat_info.size()]);
			// }
			// catch(...){
			// 	runtimeException();
			// }
			// for(size_t i = 0;
			// 	i < mat_info.size() && model::current_tex_unit < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS;
			// 	model::current_tex_unit++, i++){
			// 	// push back and use the gl_texture at the end of the material vector
			// 	// instead of making a local gl_texture object because the local
			// 	// texture will be destroyed when out-of-scope
			// 	material[i].bind(current_tex_unit);
			// 	material[i].loadData(mat_info[i]);
		}
		else{
			// make sure that the type of texinfo in texmap matched with the current texinfo type
			assert(texinfo.type == tex_iter->second->getType());
		}
		material.push_back(tex_iter->second);
	}
};

// get the scene pointer and process model data
void model::loadModelData(const std::string &path, bool flipUV){
	// look up the part map to check for preloaded model's parts
	auto vecpart_iter = model::partmap.find(path);
	if(vecpart_iter == model::partmap.end()){
		Assimp::Importer imp;
		const aiScene *scene = nullptr;
		if(flipUV)
			scene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		else
			scene = imp.ReadFile(path, aiProcess_Triangulate);
		assert(scene);
		model::partmap.insert({path, std::shared_ptr<std::vector<part>>{new std::vector<part>}});
		vecpart_iter = model::partmap.find(path);
		processModelData(*(vecpart_iter->second), scene, scene->mRootNode);
	}
	parts = vecpart_iter->second;
};

// iterating through each node (part) in the scene (model) and process
// all the meshes data within
void model::processModelData(std::vector<part> &parts, const aiScene *scene, aiNode *n){
	// process meshes (parts) within in node n
	for(unsigned i = 0; i < n->mNumMeshes; i++){
		// a mesh is a part within a model
		loadMeshData(parts, scene, *scene->mMeshes[n->mMeshes[i]]);
	}
	// process children nodes, 4 max
	for(unsigned i = 0; i < n->mNumChildren; i++){
		processModelData(parts, scene, n->mChildren[i]);
	}
};

// load mesh data into the part vbo buffer (vbuf) and ebo buffer (ebuf)
void model::loadMeshData(std::vector<part> &parts,const aiScene *scene, const aiMesh &mesh){
	part p;
	loadVertices(mesh, p.vbuf);
	loadFaces(mesh, p.ebuf);
	scene->mMaterials[mesh.mMaterialIndex]->Get(AI_MATKEY_SHININESS, p.shininess);
	parts.push_back(p);
};

// load into vbo buffer
void model::loadVertices(const aiMesh &mesh, std::vector<float>& vbuf){
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

// load into ebo buffer
void model::loadFaces(const aiMesh &mesh, std::vector<unsigned>& ebuf){
	for(size_t i = 0; i < mesh.mNumFaces; i++){
		for(size_t j = 0; j < mesh.mFaces[i].mNumIndices; j++){
			ebuf.push_back(mesh.mFaces[i].mIndices[j]);
		}
	}
};

// looping through each texture in the material and assign it texture unit
// to a sampler in the fragment shader.
void model::assignFragmentUniformSampler(gl_program &program){
	// TODO: unsigned ambient, diffuse, specular, emission = 0 instead of static vars in gl_texture
	// pros: reset after each rendering of a model. Can do texture per model
	// cons: the texture vars of a model in the frag shader are not reserved once rendered

	// only assign sampler vars if the model is textured
	if(material.size()){
		unsigned ambient = 0, diffuse = 0, specular = 0, emission = 0;
		for(size_t i = 0; i < material.size(); i++){
			std::string sampler_name;
			assert(material[i]);
			switch(material[i]->getType()){
				case aiTextureType_AMBIENT: sampler_name = std::string{"material.ambient" + std::to_string(ambient++)}; break;
				case aiTextureType_DIFFUSE: sampler_name = std::string{"material.diffuse" + std::to_string(diffuse++)}; break;
				case aiTextureType_SPECULAR: sampler_name = std::string{"material.specular" + std::to_string(specular++)}; break;
				case aiTextureType_EMISSIVE: sampler_name = std::string{"material.emission" + std::to_string(emission++)}; break;
				default: {
					std::cerr << "unknown aiTextureType" << std::endl;
					std::exit(EXIT_FAILURE);
				}
			}
			program.assignUniform(sampler_name.c_str(), glUniform1i, material[i]->getUnit());
		}
	}
};

void model::assignVertexUniformTransMat(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view){
	constructModelMatrix();
	glm::mat4 vm_mat = view * modelmatx;
	glm::mat4 pvm_mat = projection * vm_mat;
	program.assignUniform(
			"vm_mat",
			glUniformMatrix4fv,
			1,
			static_cast<unsigned char>(GL_FALSE),
			const_cast<const float*>(glm::value_ptr(vm_mat))
			);
	program.assignUniform(
			"pvm_mat",
			glUniformMatrix4fv,
			1,
			static_cast<unsigned char>(GL_FALSE),
			const_cast<const float*>(glm::value_ptr(pvm_mat)));
};

void model::constructModelMatrix(){
	if(matx_list_modified){
		modelmatx = glm::identity<glm::mat4>();
		for(const glm::mat4& m : matx_list){
			// model_mat = m * model_mat, not model_mat *= m
			modelmatx = m * modelmatx;
		}
		matx_list_modified = 0;
	}
}

// operation[i] = 1 is translate, 2 is rotate and 3 is scale
// glm::mat4 model::constructModelMatrix(){
// 	glm::mat4 model_mat {1.0f};
// 	for(size_t i = 0; i < 3; i++){
// 		switch(transform_order[i]){
// 			case 1: model_mat = glm::translate(model_mat, translate_vector); break;
// 			case 2: model_mat = glm::rotate(model_mat, rotate_angle, rotate_axis); break;
// 			case 3: model_mat = glm::scale(model_mat, glm::vec3{scale_factor}); break;
// 			default:{
// 				std::cerr << "model matrix operation order is not properly specified" << std::endl;
// 				std::exit(EXIT_FAILURE);
// 				};
// 		}
// 	}
// 	return model_mat;
// };
