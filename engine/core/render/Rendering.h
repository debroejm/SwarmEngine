#ifndef SWARMENGINE_RENDERING_H
#define SWARMENGINE_RENDERING_H

// External Libraries
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
using namespace std;

#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

//#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
using namespace glm;

// Internal Libraries
#include "Logging.h"
#include "Shaders.h"
#include "model/ModelLoading.h"
#include "config/Controls.h"

#include "Definitions.h"


// ---------------------
// | RENDERING SECTION |
// ---------------------

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_RENDER {

        #pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_RENDER, -1)
        void init();
        void cleanup();

        void ChangeShader(ENGINE_NAMESPACE_SHADER::Program &newProgram);
        void Render(ENGINE_NAMESPACE_MODEL::Model & object);
        void Render(ENGINE_NAMESPACE_MODEL::Model & object, glm::mat4 modelMatrix);
        #pragma lhgMultiOff()

    }
}

#endif //SWARMENGINE_RENDERING_H
