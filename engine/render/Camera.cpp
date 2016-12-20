#include "../Render.h"

#include "../Core.h"


namespace Swarm {
    namespace Render {

        Camera::Camera(Window &window, float speed, CameraMovementMode mode)
                : window(window), moveSpeed(speed), movementMode(mode) {
            currentPos.position = glm::vec3(0);
            currentPos.lookAt = glm::vec3(0,0,1);
            currentPos.up = glm::vec3(0,1,0);
            targetPos = currentPos;
        }

        Camera::Camera(Window &window, glm::vec3 position, glm::vec3 lookAt, glm::vec3 up, float speed, CameraMovementMode mode)
                : window(window), moveSpeed(speed), movementMode(mode) {
            currentPos.position = position;
            currentPos.lookAt = lookAt;
            currentPos.up = up;
            targetPos = currentPos;
        }

        void Camera::update(double deltaTime) {
            switch(movementMode) {
                case INSTANT:
                    currentPos = targetPos;
                    break;
                case SMOOTH:
                case LERP:
                    // TODO: Introduce Actual Lerp
                    if(currentPos.position != targetPos.position)
                        currentPos.position = Util::movePoint(currentPos.position, targetPos.position, moveSpeed * deltaTime);
                    if(currentPos.lookAt != targetPos.lookAt)
                        currentPos.lookAt = Util::movePoint(currentPos.lookAt, targetPos.lookAt, moveSpeed * deltaTime);
                    if(currentPos.up != targetPos.up)
                        currentPos.up = Util::movePoint(currentPos.up, targetPos.up, moveSpeed * deltaTime);
                    break;
            }
            viewMatrix = glm::lookAt(currentPos.position, currentPos.lookAt, currentPos.up);
            projectionMatrix = glm::perspective(fov, (float)window.getWidth() / (float)window.getHeight(), 0.1f, viewDistance);
        }

        glm::mat4 Camera::getViewMatrix() {
            return viewMatrix;
        }

        glm::mat4 Camera::getProjectionMatrix() {
            return projectionMatrix;
        }

    }
}