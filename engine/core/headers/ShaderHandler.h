#ifndef SWARMENGINE_SHADERHANDLER_H
#define SWARMENGINE_SHADERHANDLER_H

// External Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

//#define GLFW_DLL
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

// Internal Libraries
#include "Logging.h"

#include "../Definitions.h"

// -----------
// Definitions
// -----------

// Shader Uniform IDs
#define SHADER_UNIFORM_MVP 1
#define SHADER_UNIFORM_MODEL 2
#define SHADER_UNIFORM_VIEW 3
#define SHADER_UNIFORM_TEXTURE 4
#define SHADER_UNIFORM_LIGHT_POS 5
#define SHADER_UNIFORM_LIGHT_COLOR 6
#define SHADER_UNIFORM_LIGHT_POWER 7
#define SHADER_UNIFORM_SCREENSIZE 8


// -------------------
// | SHADERS SECTION |
// -------------------

namespace ENGINE_NAMESPACE {
    namespace Shaders {

        struct UniformID{
            GLuint ID;
            bool used;
        };

        class ShaderData{
        public:
            ShaderData();
            ShaderData(const char * vertex_shader_path, const char * fragment_shader_path);
            ~ShaderData();

            void operator=(const ShaderData &rhs);

            void setUniformID(int uniformType, bool useIt);
            GLuint getUniformID(int uniformType);
            GLuint getProgramID();
        private:
            GLuint ShaderID;
            UniformID MatrixID;
            UniformID ViewMatrixID;
            UniformID ModelMatrixID;
            UniformID TextureID;
            UniformID LightPositionID;
            UniformID LightColorID;
            UniformID LightPowerID;
            UniformID ScreenSizeID;

            bool loaded;
        };

    }
}

#endif //SWARMENGINE_SHADERHANDLER_H
