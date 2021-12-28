#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include"gl_object.h"

// TEXTURE UNIT

class gl_texture : public gl_object{
    public:
        gl_texture();
        ~gl_texture();
        void bind();
        void loadData(const char *texture_path);
    private:
        int width, height, channel;
        unsigned char *data;
};

#endif // GLTEXTURE_H
