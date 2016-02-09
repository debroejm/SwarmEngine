#ifndef SWARMENGINE_INITIALIZATION_H
#define SWARMENGINE_INITIALIZATION_H

// External Libraries
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

//#define GLFW_DLL
#include <glfw/glfw3.h>

// Internal Libraries
#include "Controls.h"
#include "Logging.h"
#include "Rendering.h"

#include "GLFWCallbacks.h"

#include "../Definitions.h"

// ----------------
// | INIT SECTION |
// ----------------

namespace ENGINE_NAMESPACE {
    namespace Init{

        bool init();
        bool init(const char * windowName);
        bool init(const char * windowName, int windowX, int windowY);
        bool init(int logLevel, int logType);
        bool init(int logLevel, int logType, const char * windowName);
        bool init(int logLevel, int logType, const char * windowName, int windowX, int windowY);

    }
}

#endif //SWARMENGINE_INITIALIZATION_H
