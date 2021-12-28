#include"../include/gl_texture.h"
// header to extract texture data
#define STB_IMAGE_IMPLEMENTATION
#include<../include/stb_image.h>
#include<cassert>

// TEXTURE UNIT
// reload texture ?

gl_texture::gl_texture() : gl_object(){
    // glActiveTexture()
    glGenTextures(1, &name);
}

gl_texture::~gl_texture(){
    glDeleteTextures(1, &name);
}

void gl_texture::bind(){
    glBindTexture(GL_TEXTURE_2D, name);
}

void gl_texture::loadData(const char *texture_path){
    data = stbi_load(texture_path, &width, &height, &channel, 0);
    assert(data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // default rules for texture wrapping and texture filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
