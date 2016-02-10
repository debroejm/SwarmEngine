// NOTE: Add ' extern "C" ' after lib definitions for C compatibility if needed

#ifndef SWARMENGINE_CORE_H
#define SWARMENGINE_CORE_H

#if defined(_WIN32) || defined(__WIN32__)
    #if defined(SwarmEngine_EXPORTS)
        #define DLLFUNC __declspec(dllexport)
    #else
        #define DLLFUNC __declspec(dllimport)
    #endif
#else
    #define DLLFUNC
#endif

// External Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>
#include <map>
using namespace std;

#include <stdlib.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


// -----------
// Sub-Headers
// -----------

#include "core/headers/Initialization.h"

// Base System
#include "core/headers/Logging.h"
#include "core/headers/Shaders.h"
#include "core/headers/Controls.h"
#include "core/headers/Rendering.h"

// Textures and Models
#include "core/headers/TextureHandler.h"
#include "core/headers/AnimatedTexture.h"
#include "core/headers/ModelLoading.h"

// Structures

#endif //SWARMENGINE_CORE_H
