#ifndef GLTEXTURE_H
#define GLTEXTURE_H
#include "gl_object.h"
#include "glm/vec3.hpp"
#include "assimp/material.h"
#include <string>

// TEXTURE UNIT

class gl_texture : public gl_object{
	public:
		gl_texture(const char *texture_path, aiTextureType type);
		~gl_texture();
		void bind(GLenum GL_TEXTUREI);
		void loadData();
		// Return the texture unit of this gl_texture assigned via bind(),
		// minus GL_TEXTURE0. Used with glUniform to assign the texture unit to
		// the sampler in the fragment shader
		int getUnit() const;
		// Return the sampler variable name. Used with glUniform to assign the
		// texture unit to the sampler in the fragment shader
		std::string getSamplerName() const;
	private:
		// The static member count the total number of each type of gl_texture
		// constructed. Add more if needed. The total number is used for
		// assigning uniform variable string through the convention textypei.
		static unsigned ambient,
						diffuse,
						specular,
						emission;
						// shininess
		int width, height, channel;
		GLenum tex_unit;
		std::string sampler_name;
		unsigned char *data;
		void assignSamplerName(aiTextureType type);
};

#endif // GLTEXTURE_H
