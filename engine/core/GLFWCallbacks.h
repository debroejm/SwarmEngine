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

    }
}