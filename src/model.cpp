#include "../include/model.h"
#include "../include/runtime_except.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cassert>
#include <iostream>

// public =================================
// ctor with material, 'path' is the path to a model file. 'mat_info' contains
// a list of different path to the omdel textures and their type
model::model(const std::string &path, const std::vector<tex_info> &mat_info, bool flipUV){
	loadData(path, mat_info, flipUV);
};

// ctor without material
model::model(const std::string &path, const glm::vec4 &vcolor){
	loadData(path, vcolor);
};

// copy ctor
model::model(const model&m){
	// don't copy model_mat, the object will always spawn at the center
	part_struct = m.part_struct;
	material = m.material;
	numTex = m.numTex;
	vertex_color = m.vertex_color;
};

// move ctor
model::model(model&&m){
	part_struct = m.part_struct;
	if(material){
		material = m.material;
		numTex = m.numTex;
	}
	model_mat = m.model_mat;
	vertex_color = m.vertex_color;
}

// copy assigment op
model& model::operator=(const model&m){
	// don't copy model_mat, the object will always spawn at the center
	part_struct = m.part_struct;
	material = m.material;
	numTex = m.numTex;
	vertex_color = m.vertex_color;
	return *this;
};

// move assignment op
model& model::operator=(model&&m){
	// don't copy model_mat, the object will always spawn at the center
	part_struct = m.part_struct;
	if(material){
		material = m.material;
		numTex = m.numTex;
	}
	numTex = m.numTex;
	vertex_color = m.vertex_color;
	return *this;
};

// dtor
model::~model() noexcept {};

// loadData with material
void model::loadData(const std::string &path, const std::vector<tex_info> &mat_info, bool flipUV){
	loadModelData(path, flipUV);
	loadMaterial(mat_info);
};

// loadData without material
void model::loadData(const std::string &path, const glm::vec4 &vcolor){
	loadModelData(path, 0);
	vertex_color = std::move(vcolor);
};

// Render the model with the currently attached fragment shader
void model::draw(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view, GLenum usage){
	program.assignUniform("material.use_vertex_color", glUniform1i, (int)!material);
	program.assignUniform("material.vertex_color", glUniform4fv, 1, const_cast<const float*>(glm::value_ptr(vertex_color)));
	assignVertexUniformTransMat(program, projection, view);
	assignFragmentUniformSampler(program);
	vao.bind();
	vbo.bind();
	vao.attribFormat(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
	vao.attribFormat(1, 3, GL_FLOAT, GL_FALSE, 32, (void*)12);
	vao.attribFormat(2, 2, GL_FLOAT, GL_FALSE, 32, (void*)24);
	for(const part &p : part_struct){
		vbo.loadData(p.vbuf, usage);
		vao.eboData(p.ebuf, usage);
		// batch drawing ?
		glDrawElements(GL_TRIANGLES, p.ebuf.size(), GL_UNSIGNED_INT, (void*)0);
	}
};

// Replace the currently attached fragment shader with the provided
// 'fshader', relink the program and render the model
void model::draw(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view, const gl_shader *fshader, GLenum usage){
	program.attachCompiledShader(fshader);
	program.link();
	draw(program, projection, view, usage);
};

// uniformly scale the model
void model::scale(float factor){
	model_mat = glm::scale(model_mat, glm::vec3{factor, factor, factor});
};

// translate the model to the given vector position
void model::translate(const glm::vec3 &v){
	model_mat = glm::translate(model_mat, v);
};

// rotate the model 'radian' angle around the given vector
void model::rotate(float radian, const glm::vec3 &v){
	model_mat = glm::rotate(model_mat, radian, v);
};

glm::vec4 model::getVertexColor() const{
	return vertex_color;
};

// change the current vertex color
void model::assignVertexColor(const glm::vec4 &vcolor){
	vertex_color = vcolor;
};

// private =================================
// load the material information into this material vector
void model::loadMaterial(const std::vector<tex_info> &mat_info){
	// use reserve with allocator
	// material.reserve(mat_info.size());
	numTex = mat_info.size();
	try{
		// material = std::make_shared<gl_texture[]>();
		material = std::shared_ptr<gl_texture[]>(new gl_texture[mat_info.size()]);
	}
	catch(...){
		runtimeException();
	}
	for(size_t i = 0; i < numTex; i++){
		// push back and use the gl_texture at the end of the material vector
		// instead of making a local gl_texture object because the local
		// texture will be destroyed when out-of-scope
		material[i].bind(GL_TEXTURE0 + i);
		material[i].loadData(mat_info[i]);
	}
};

// get the scene pointer and process model data
void model::loadModelData(const std::string &path, bool flipUV){
	Assimp::Importer imp;
	const aiScene *scene = nullptr;
	if(flipUV)
		scene = imp.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	else
		scene = imp.ReadFile(path, aiProcess_Triangulate);
	assert(scene);
	processModelData(scene, scene->mRootNode);
};

// iterating through each node (part) in the scene (model) and process
// all the meshes data within
void model::processModelData(const aiScene *scene, aiNode *n){
	// process meshes within in node n
	for(unsigned i = 0; i < n->mNumMeshes; i++){
		// a mesh is a part within a model
		loadMeshData(*scene->mMeshes[n->mMeshes[i]]);
	}
	// process children nodes, 4 max
	for(unsigned i = 0; i < n->mNumChildren; i++){
		processModelData(scene, n->mChildren[i]);
	}
};

// load mesh data into the part vbo buffer (vbuf) and ebo buffer (ebuf)
void model::loadMeshData(const aiMesh &mesh){
	part p;
	loadVertices(mesh, p.vbuf);
	loadFaces(mesh, p.ebuf);
	part_struct.push_back(p);
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
	if(!material) return;
	unsigned ambient = 0, diffuse = 0, specular = 0, emission = 0;
	for(size_t i = 0; i < numTex; i++){
		std::string sampler_name;
		switch(material[i].getType()){
			case aiTextureType_AMBIENT: sampler_name = std::string{"ambient" + std::to_string(ambient++)}; break;
			case aiTextureType_DIFFUSE: sampler_name = std::string{"diffuse" + std::to_string(diffuse++)}; break;
			case aiTextureType_SPECULAR: sampler_name = std::string{"specular" + std::to_string(specular++)}; break;
			case aiTextureType_EMISSIVE: sampler_name = std::string{"emission" + std::to_string(emission++)}; break;
			default: {
				std::cerr << "unknown aiTextureType" << std::endl;
				std::exit(EXIT_FAILURE);
			}
		}
		program.assignUniform(sampler_name.c_str(), glUniform1i, material[i].getUnit());
	}
}

void model::assignVertexUniformTransMat(gl_program &program, const glm::mat4 &projection, const glm::mat4 &view){
	glm::mat4 vm_mat = view * model_mat;
	glm::mat4 pvm_mat = projection * vm_mat;
	program.assignUniform(
			"vm_mat",
			glUniformMatrix4fv,
			1,
			static_cast<unsigned char>(GL_FALSE),
			const_cast<const float*>(glm::value_ptr(vm_mat)));
	program.assignUniform(
			"pvm_mat",
			glUniformMatrix4fv,
			1,
			static_cast<unsigned char>(GL_FALSE),
			const_cast<const float*>(glm::value_ptr(pvm_mat)));
};
