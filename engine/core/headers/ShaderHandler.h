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

        class Shader{
        public:
            Shader(const char * shaderPath, GLenum shaderType);
            ~Shader();

            void operator=(const Shader &rhs);

            GLuint getShaderID();
            GLenum getShaderType();
        protected:
            GLuint shaderID;
            GLenum shaderType;
        };

        class Program {
        public:
            Program(Shader *shaders[], int shaderCount);
            ~Program();

            void operator=(const Program &rhs);

            GLuint getProgramID();
        protected:
            GLuint programID;
        };

        void cleanupShaders();
        void cleanupPrograms();

    }
}

#endif //SWARMENGINE_SHADERHANDLER_H
