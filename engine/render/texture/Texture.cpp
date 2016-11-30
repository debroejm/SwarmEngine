#include "../../Render.h"
#include "../../Core.h"

namespace Swarm {
    namespace Texture {

        namespace MapType {

            std::hash<unsigned int> Type::hasher;

            Type::Type(GLenum target, GLuint activeID, string uniform)
                    : target(target), active(activeID), uniform(uniform) {}

        }
    }
}