#pragma once

// External Libraries
#define GLEW_STATIC
#include <GL/glew.h>

//#define GLFW_DLL
#include <glfw/glfw3.h>

namespace Swarm {
    namespace GLFW {

        // GLFW Callback Prototypes
        void error_callback(int error, const char* description);
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    }
}