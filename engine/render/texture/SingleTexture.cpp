#include "../../Render.h"

namespace Swarm {
    namespace Texture {

        SingleTexture::SingleTexture(GLuint texID) : texID_diffuse(texID) {}
        SingleTexture::SingleTexture(const char * texName) : texID_diffuse(loadPNGTexture(texName)) {}

        void SingleTexture::setDiffuse  (GLuint texID) { texID_diffuse  = texID; }
        void SingleTexture::setSpecular (GLuint texID) { texID_specular = texID; }

        void SingleTexture::setDiffuse  (const char * texName) { texID_diffuse  = loadPNGTexture(texName); }
        void SingleTexture::setSpecular (const char * texName) { texID_specular = loadPNGTexture(texName); }

        GLuint SingleTexture::getID() { return texID_diffuse; }
        void SingleTexture::bind() {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texID_diffuse);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texID_specular);
        }

        SingleTexture &SingleTexture::operator=(const SingleTexture &rhs) {
            texID_diffuse = rhs.texID_diffuse;
            return *this;
        }

        SingleTexture &SingleTexture::operator=(const GLuint &rhs) {
            texID_diffuse = rhs;
            return *this;
        }

        bool SingleTexture::operator==(const SingleTexture &rhs) {
            return texID_diffuse == rhs.texID_diffuse;
        }

        bool SingleTexture::operator==(const GLuint &rhs) {
            return texID_diffuse == rhs;
        }
    }
}