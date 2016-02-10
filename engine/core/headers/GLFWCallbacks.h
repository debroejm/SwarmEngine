#ifndef SWARMENGINE_GLFWCALLBACKS_H
#define SWARMENGINE_GLFWCALLBACKS_H

// External Libraries
#define GLEW_STATIC
#include <GL/glew.h>

//#define GLFW_DLL
#include <glfw/glfw3.h>


// Internal Libraries
#include "Logging.h"
#include "Configuration.h"

#include "../Definitions.h"


// ----------------
// | GLFW SECTION |
// ----------------

namespace ENGINE_NAMESPACE {
    namespace GLFW {

        // GLFW Callback Prototypes
        void error_callback(int error, const char* description);
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    }
}

#endif //SWARMENGINE_GLFWCALLBACKS_H
