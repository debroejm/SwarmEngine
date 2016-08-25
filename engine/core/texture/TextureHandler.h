#ifndef SWARMENGINE_TEXTUREHANDLER_H
#define SWARMENGINE_TEXTUREHANDLER_H

// External Libraries
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

//#define GLFW_DLL
#include <glfw/glfw3.h>

#include <lodepng.h>


// Internal Libraries
#include "Logging.h"

#include "Definitions.h"


// --------------------
// | TEXTURES SECTION |
// --------------------

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_TEXTURE {

        #pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_TEXTURE, 50)
        GLuint loadPNGTexture(const char* filename);
        void registerTexture(GLuint textureID);
        void cleanupTextures();
        #pragma lhgMultiOff()

    }
}

#endif //SWARMENGINE_TEXTUREHANDLER_H
