#define SWARM_INCLUDE_GLEW
#include "api/Render.h"

#include "api/Exception.h"

#include <lodepng.h>

namespace Swarm {
    namespace Texture {

        std::set<GLuint> registered_textures;

        void cleanup() {
            for(GLuint tex : registered_textures) glDeleteTextures(1, &tex);
        }

        GLuint loadTexInternal_PNG(const std::string &path) {
            std::vector<unsigned char> image;
            unsigned width, height;
            unsigned error = lodepng::decode(image, width, height, path.c_str());

            if(error != 0) throw Exception::TextureException::FileLoadPNG(path, std::string(lodepng_error_text(error)));

            GLuint textureID;
            glGenTextures(1, &textureID);

            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
            glGenerateMipmap(GL_TEXTURE_2D); // Is this really needed?

            glBindTexture(GL_TEXTURE_2D, 0);

            return textureID;
        }

        TexMap loadTexFromFile(const std::string &path, FileType type) {
            GLuint texID = 0;
            switch(type) {
                case PNG: texID = loadTexInternal_PNG(path); break;
                default: texID = 0; break;
            }
            registered_textures.insert(texID);
            return TexMap(Type::TargetType::TEX_2D, texID);
        }
    }
}