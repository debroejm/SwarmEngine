#include "../../Render.h"

#include "../../Core.h"

#include <lodepng.h>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Texture {

        std::vector<GLuint> registeredTextures;

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

            registerTexture(textureID);
            return textureID;
        }

        void registerTexture(GLuint textureID)
        {
            for(int i = 0; i < registeredTextures.size(); i++)
            {
                if( registeredTextures[i] == textureID ) return;
            }
            registeredTextures.push_back(textureID);

            Log::log_render(INFO) << "Texture ID Registered: " << textureID;
        }

        void cleanupTextures()
        {
            for(int i = 0; i < registeredTextures.size(); i++)
            {
                Log::log_render(INFO) << "Deleting Texture [" << registeredTextures[i] << "]";
                // Yes, thats formatted properly. Pointers...
                glDeleteTextures(1, &registeredTextures[i]);
            }
        }
    }
}