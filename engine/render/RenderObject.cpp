#include "../Render.h"

#include <glm/gtx/transform.hpp>

namespace Swarm {
    namespace Render {

        void SimpleROS::translate(glm::vec3 amount) {
            matrix = matrix * glm::translate(amount);
        }

        void SimpleROS::rotate(float amount, glm::vec3 angle) {
            matrix = matrix * glm::rotate(amount, angle);
        }

        void SimpleROS::scale(glm::vec3 amount) {
            matrix = matrix * glm::scale(amount);
        }

        void SimpleROS::resetMatrix() {
            matrix = glm::mat4(1);
        }
    }
}