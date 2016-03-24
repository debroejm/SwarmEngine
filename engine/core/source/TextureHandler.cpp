#include "../headers/TextureHandler.h"

using namespace ENGINE_NAMESPACE::ENGINE_NAMESPACE_LOG;

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_TEXTURE {

        std::vector<GLuint> registeredTextures;

        GLuint loadPNGTexture(const char* filename) {
            std::vector<unsigned char> image;
            unsigned width, height;
            unsigned error = lodepng::decode(image, width, height, filename);

            if(error != 0) {
                char errorMsg[256];
                sprintf(errorMsg, "'%s': %s", filename, lodepng_error_text(error));
                Log(LOGGING_ERROR, "Textures", errorMsg);
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

            char logString[256];
            sprintf(logString, "Texture ID Registered: %i", textureID);
            Log(LOGGING_INFO, "Textures", logString);
        }

        void cleanupTextures()
        {
            for(int i = 0; i < registeredTextures.size(); i++)
            {
                char infoMsg[256];
                sprintf(infoMsg, "Deleting Texture [%i]", registeredTextures[i]);
                Log(LOGGING_INFO, "Cleanup", infoMsg);
                // Yes, thats formatted properly. Pointers...
                glDeleteTextures(1, &registeredTextures[i]);
            }
        }
    }
}