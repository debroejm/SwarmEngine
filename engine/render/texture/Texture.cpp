#include "../../Render.h"
#include "../../Core.h"

namespace Swarm {
    namespace Texture {

        namespace MapType {

            std::hash<unsigned int> Type::hasher;

            Type::Type(GLenum target, GLuint activeID)
                    : target(target), active(activeID) {}

        }

        void Texture::bind() const {
            for(auto && iter : data) {
                glActiveTexture(GL_TEXTURE0 + iter.first);
                glBindTexture(iter.second.target, iter.second.ID);
            }
        }

        void Texture::setTex(GLuint active, GLuint ID, GLenum target) {
            data[active] = TexData{ ID, target };
        }

        GLuint Texture::getTex(GLuint active) const {
            if(data.count(active)) return data.at(active).ID;
            else return 0;
        }

        GLuint Texture::operator[](GLuint active) const {
            return getTex(active);
        }

        int Texture::compareTo(const Texture &rhs) const {
            // TODO: Faster Compare Method for Textures
            if(data.size() != rhs.data.size()) return false; // Early Out Check
            GLint max;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max);
            for(GLuint i = 0; i < max; i++) {
                int diff = getTex(i) - rhs.getTex(i);
                if(diff == 0) continue;
                return diff;
            }
            return 0;
        }

        Texture &Texture::operator=(const Texture &rhs) {
            this->data = rhs.data;
            return *this;
        }
    }
}