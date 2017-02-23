#define SWARM_INCLUDE_GLEW
#include "../../Render.h"

#include "../../Core.h"

#include <lodepng.h>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Texture {

        std::set<GLuint> registeredTextures;

        GLuint loadPNGTexture(const char* filename) {
            std::vector<unsigned char> image;
            unsigned width, height;
            unsigned error = lodepng::decode(image, width, height, filename);

            if(error != 0) {
                Log::log_render(ERR) << "Texture Error [" << filename << "]: " << lodepng_error_text(error);
                return 0;
            }

            GLuint textureID;
            glGenTextures(1, &textureID);

            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
            glGenerateMipmap(GL_TEXTURE_2D); // Is this really needed?

            glBindTexture(GL_TEXTURE_2D, 0);

            registeredTextures.insert(textureID);
            Log::log_render(INFO) << "Texture ID Registered: " << textureID;

            return textureID;
        }

        void cleanupTextures()
        {
            for(GLuint texID : registeredTextures)
            {
                Log::log_render(INFO) << "Deleting Texture [" << texID << "]";
                glDeleteTextures(1, &texID);
            }
        }
    }
}