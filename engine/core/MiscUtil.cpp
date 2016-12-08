#include "../Core.h"

namespace Swarm {
    namespace Util {

        glm::vec3 movePoint(glm::vec3 point, glm::vec3 target, float distance) {
            glm::vec3 vecDiff = target - point;
            float distDiff = sqrt(pow(glm::length(vecDiff),2.0f));
            return point + ( vecDiff * (distance / distDiff) );
        }

    }
}