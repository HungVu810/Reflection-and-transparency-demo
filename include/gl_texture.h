#ifndef GLTEXTURE_H
#define GLTEXTURE_H
#include "gl_object.h"
#include "glm/vec3.hpp"
#include "assimp/material.h"
#include <string>


// texture info for a specific texture
struct tex_info{
	// path to texture
	std::string path;
	aiTextureType type;
};

class gl_texture : public gl_object{
	public:
		gl_texture();

		// no copy of texture, use reference/pointer instead
		gl_texture(const gl_texture &t) = delete;
		gl_texture& operator=(const gl_texture &t) = delete;

		// no moving of texture resource
		gl_texture(gl_texture &&t) = delete;
		gl_texture& operator=(gl_texture &&t) = delete;

		~gl_texture();

		void bind(GLenum GL_TEXTUREI);

		void loadData(const tex_info &t);

		// Return the texture unit of this gl_texture assigned via bind(),
		// minus GL_TEXTURE0. Used with glUniform to assign the texture unit to
		// the sampler in the fragment shader
		int getUnit() const;

		aiTextureType getType() const;

		// Return the sampler variable name. Used with glUniform to assign the
		// texture unit to the sampler in the fragment shader
		// std::string getSamplerName() const;

	private:
		// The static member count the total number of each type of gl_texture
		// constructed. Add more if needed. The total number is used for
		// assigning uniform variable string through the convention textypei.
		// static unsigned ambient,
		// 				diffuse,
		// 				specular,
		// 				emission;
						// shininess

		int width, height, channel;

		GLenum tex_unit;

		unsigned char *data;

		aiTextureType type;

		// std::string sampler_name;

		// void assignSamplerName(aiTextureType type);
};

#endif // GLTEXTURE_H
