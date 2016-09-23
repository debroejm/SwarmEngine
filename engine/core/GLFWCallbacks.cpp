#include "GLFWCallbacks.h"

#include "../Core.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace GLFW {

        void error_callback(int error, const char* description)
        {
            Log::log_core(ERR) << " " << error << ": " << description;
        }

        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            /*
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GL_TRUE);
                */

            // Send keycode and other info to the input processing part of mEng
            if(action == GLFW_PRESS) Input::setPressed(key, true);
            if(action == GLFW_RELEASE) Input::setPressed(key, false);
        }

        void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            // Send the new size info to the mEng global variables
        }

    }
}