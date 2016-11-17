#include "../Render.h"

#include <glm/gtx/transform.hpp>

namespace Swarm {
    namespace Render {

        void RenderObjectSimple::translate(glm::vec3 amount) {
            matrix = matrix * glm::translate(amount);
        }

        void RenderObjectSimple::rotate(float amount, glm::vec3 angle) {
            matrix = matrix * glm::rotate(amount, angle);
        }

        void RenderObjectSimple::scale(glm::vec3 amount) {
            matrix = matrix * glm::scale(amount);
        }

        void RenderObjectSimple::resetMatrix() {
            matrix = glm::mat4(1);
        }
    }
}