#include "../Render.h"

#include "../Core.h"


namespace Swarm {
    namespace Render {



        Camera::Camera(GLFWwindow *window, float speed, CameraMovementMode mode)
                : window(window), moveSpeed(speed), movementMode(mode) {
            currentPos.position = glm::vec3(0);
            currentPos.lookAt = glm::vec3(0,0,1);
            currentPos.up = glm::vec3(0,1,0);
        }

        Camera::Camera(GLFWwindow *window, glm::vec3 position, glm::vec3 lookAt, glm::vec3 up, float speed, CameraMovementMode mode)
                : window(window), moveSpeed(speed), movementMode(mode) {
            currentPos.position = position;
            currentPos.lookAt = lookAt;
            currentPos.up = up;
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
        }

        glm::mat4 Camera::getViewMatrix() {
            return glm::lookAt(currentPos.position, currentPos.lookAt, currentPos.up);
        }

        glm::mat4 Camera::getProjectionMatrix() {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            return glm::perspective(fov, (float)width / (float)height, 0.1f, viewDistance);
        }

    }
}