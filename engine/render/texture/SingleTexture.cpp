#include "../../Render.h"

namespace Swarm {
    namespace Texture {

        SingleTexture::SingleTexture(GLuint textureID) : textureID(textureID) {}
        SingleTexture::SingleTexture(const char * textureName) {
            textureID = loadPNGTexture(textureName);
        }

        GLuint SingleTexture::getID() { return textureID; }

        SingleTexture &SingleTexture::operator=(const SingleTexture &rhs) {
            textureID = rhs.textureID;
            return *this;
        }

        SingleTexture &SingleTexture::operator=(const GLuint &rhs) {
            textureID = rhs;
            return *this;
        }

        bool SingleTexture::operator==(const SingleTexture &rhs) {
            return textureID == rhs.textureID;
        }

        bool SingleTexture::operator==(const GLuint &rhs) {
            return textureID == rhs;
        }
    }
}