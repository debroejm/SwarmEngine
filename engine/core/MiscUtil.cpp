#include "../Core.h"

#include "glm/gtx/norm.hpp"

namespace Swarm {
    namespace Util {

        glm::vec3 movePoint(glm::vec3 point, glm::vec3 target, float distance) {
            glm::vec3 vecDiff = target - point;
            float distDiff = sqrt(glm::length2(vecDiff));
            return point + ( vecDiff * (distance / distDiff) );
        }

    }
}