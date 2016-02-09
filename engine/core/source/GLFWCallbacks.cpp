#include "../headers/GLFWCallbacks.h"

namespace ENGINE_NAMESPACE {
    namespace GLFW {

        void error_callback(int error, const char* description)
        {
            Logging::Log(LOGGING_ERROR, "GLFW", description);
            //fputs(description, stderr);
        }

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                glfwSetWindowShouldClose(window, GL_TRUE);
            // Send keycode and other info to the input processing part of mEng
        }

        void framebuffer_size_callback(GLFWwindow* window, int width, int height)
        {
            // Send the new size info to the mEng global variables
        }

    }
}