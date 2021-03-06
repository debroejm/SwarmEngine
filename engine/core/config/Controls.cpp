#define SWARM_INCLUDE_GLM
#include "api/Core.h"

namespace Swarm {
    namespace Input {

        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;


        glm::mat4 getViewMatrix(){
            return ViewMatrix;
        }
        glm::mat4 getProjectionMatrix(){
            return ProjectionMatrix;
        }


        // Initial position : on +Z
        glm::vec3 position = glm::vec3( 0, 0, 20 );
        // Initial horizontal angle : toward -Z
        float horizontalAngle = 3.14f;
        // Initial vertical angle : none
        float verticalAngle = 0.0f;
        // Initial Field of View
        float initialFoV = 45.0f;

        float speed = 10.0f; // 3 units / second
        float mouseSpeed = 0.005f;


        void computeMatricesFromInputs(){

            // glfwGetTime is called only once, the first time this function is called
            static double lastTime = glfwGetTime();

            // Compute time difference between current and last frame
            double currentTime = glfwGetTime();
            float deltaTime = float(currentTime - lastTime);

            // Get mouse position
            double xpos, ypos;
            glfwGetCursorPos(windowHandle, &xpos, &ypos);

            int width, height;
            glfwGetWindowSize(windowHandle, &width, &height);

            // Reset mouse position for next frame
            glfwSetCursorPos(windowHandle, width/2, height/2);

            // Compute new orientation
            horizontalAngle += mouseSpeed * float(width/2 - xpos );
            verticalAngle   += mouseSpeed * float(height/2 - ypos );

            if(verticalAngle > 1.57f) verticalAngle = 1.57f;
            if(verticalAngle < -1.57f) verticalAngle = -1.57f;

            // Direction : Spherical coordinates to Cartesian coordinates conversion
            glm::vec3 direction(
                    cos(verticalAngle) * sin(horizontalAngle),
                    sin(verticalAngle),
                    cos(verticalAngle) * cos(horizontalAngle)
            );

            // Right vector
            glm::vec3 right = glm::vec3(
                    sin(horizontalAngle - 3.14f/2.0f),
                    0,
                    cos(horizontalAngle - 3.14f/2.0f)
            );

            // Up vector
            //glm::vec3 up = glm::cross( right, direction );
            glm::vec3 up(0.0f, 1.0f, 0.0f);


            // Move forward
            if (glfwGetKey( windowHandle, GLFW_KEY_UP ) == GLFW_PRESS){
                position += direction * deltaTime * speed;
            }
            // Move backward
            if (glfwGetKey( windowHandle, GLFW_KEY_DOWN ) == GLFW_PRESS){
                position -= direction * deltaTime * speed;
            }
            // Strafe right
            if (glfwGetKey( windowHandle, GLFW_KEY_RIGHT ) == GLFW_PRESS){
                position += right * deltaTime * speed;
            }
            // Strafe left
            if (glfwGetKey( windowHandle, GLFW_KEY_LEFT ) == GLFW_PRESS){
                position -= right * deltaTime * speed;
            }

            float FoV = initialFoV; // - 5 * glfwGetMouseWheel();

            // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
            ProjectionMatrix = glm::perspective(FoV, (float)width / (float)height, 0.1f, 1000.0f);
            // Camera matrix
            ViewMatrix       = glm::lookAt(
                    position,           // Camera is here
                    position+direction, // and looks here : at the same position, plus "direction"
                    //vec3(0.0f, 0.0f, 0.0f),
                    up                  // Head is up (set to 0,-1,0 to look upside-down)
            );

            // For the next frame, the "last time" will be "now"
            lastTime = currentTime;
        }

    }
}