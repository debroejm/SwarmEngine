#include "../../Render.h"

namespace Swarm {
    namespace Texture {

        SingleTexture::SingleTexture(GLuint texID) { texIDs[MapType::DIFFUSE] = texID; }
        SingleTexture::SingleTexture(const char * texName) { texIDs[MapType::DIFFUSE] = loadPNGTexture(texName); }

        void SingleTexture::setTex(const MapType::Type &type, GLuint texID) { texIDs[type] = texID; }
        void SingleTexture::setTex(const MapType::Type &type, const char * texName) { texIDs[type] = loadPNGTexture(texName); }

        GLuint SingleTexture::getTex(const MapType::Type &type) const {
            if(texIDs.count(type)) return texIDs.at(type);
            else return 0;
        }

        GLuint SingleTexture::getTex(GLuint activeID) const {
            MapType::Type type(GL_TEXTURE_2D, activeID, ""); // Dummy type; only activeID is used for keys.
            if(texIDs.count(type)) return texIDs.at(type);
            else return 0;
        }

        void SingleTexture::bind() const {
            for(auto && iter : texIDs) {
                glActiveTexture(GL_TEXTURE0 + iter.first.getActiveID());
                glBindTexture(iter.first.getTarget(), iter.second);
            }
        }

        SingleTexture &SingleTexture::operator=(const SingleTexture &rhs) {
            texIDs = rhs.texIDs;
            return *this;
        }

        SingleTexture &SingleTexture::operator=(const GLuint &rhs) {
            texIDs.clear();
            texIDs[MapType::DIFFUSE] = rhs;
            return *this;
        }

        int SingleTexture::compareTo(const Texture &rhs) const {
            GLint max;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max);
            for(GLuint i = 0; i < max; i++) {
                int diff = getTex(i) - rhs.getTex(i);
                if(diff == 0 && i < max-1) continue;
                return diff;
            }
            return 0;
        }
    }
}