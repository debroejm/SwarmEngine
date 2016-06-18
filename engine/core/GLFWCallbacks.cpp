#include "GLFWCallbacks.h"

using namespace ENGINE_NAMESPACE::ENGINE_NAMESPACE_LOG;

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_GLFW {

        void error_callback(int error, const char* description)
        {
            Log::log_global(ERR) << description;
            //fputs(description, stderr);
        }

        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            /*
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GL_TRUE);
                */

            // Send keycode and other info to the input processing part of mEng
            if(action == GLFW_PRESS) ENGINE_NAMESPACE_CONFIG::setPressed(key, true);
            if(action == GLFW_RELEASE) ENGINE_NAMESPACE_CONFIG::setPressed(key, false);
        }

        void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            // Send the new size info to the mEng global variables
        }

    }
}