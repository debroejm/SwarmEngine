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
#include "ShaderHandler.h"
#include "ModelLoading.h"
#include "Controls.h"

#include "../Definitions.h"


// ---------------------
// | RENDERING SECTION |
// ---------------------

namespace ENGINE_NAMESPACE {
    namespace Rendering {

        void init();
        void cleanup();

        void ChangeShader(Swarm::Shaders::ShaderData & newShader);
        void Render(Swarm::Models::Model & object);
        void Render(Swarm::Models::Model & object, glm::mat4 modelMatrix);

    }
}

#endif //SWARMENGINE_RENDERING_H
