#include "../Render.h"

#include "../Core.h"


namespace Swarm {
    namespace Render {

        Camera::Camera(Renderer &renderer, GLFWwindow *window, float speed, CameraMovementMode mode)
                : renderer(renderer), window(window), moveSpeed(speed), movementMode(mode) {
            currentPos.position = glm::vec3(0);
            currentPos.lookAt = glm::vec3(0,0,1);
            currentPos.up = glm::vec3(0,1,0);
        }

        Camera::Camera(Renderer &renderer, GLFWwindow *window, glm::vec3 position, glm::vec3 lookAt, glm::vec3 up, float speed, CameraMovementMode mode)
                : renderer(renderer), window(window), moveSpeed(speed), movementMode(mode) {
            currentPos.position = position;
            currentPos.lookAt = lookAt;
            currentPos.up = up;
        }

        void Camera::update(double deltaTime) {
            /*
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
             */
            viewMatrix = glm::lookAt(currentPos.position, currentPos.lookAt, currentPos.up);
            Uniforms::MatrixView.setm(renderer, 1, 4, 4, &viewMatrix[0][0]);
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            projectionMatrix = glm::perspective(fov, (float)width / (float)height, 0.1f, viewDistance);
            Uniforms::MatrixProjection.setm(renderer, 1, 4, 4, &projectionMatrix[0][0]);
        }

        glm::mat4 Camera::getViewMatrix() {
            return viewMatrix;
        }

        glm::mat4 Camera::getProjectionMatrix() {
            return projectionMatrix;
        }

    }
}