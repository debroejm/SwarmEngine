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

#include "Definitions.h"

// -------------------
// | SHADERS SECTION |
// -------------------

#pragma lhgMultiOn(SwarmEngine, LHG_NONE, 100)
#define SHADER_ATTRIBUTE_VERTEX 1
#define SHADER_ATTRIBUTE_NORMAL 2
#define SHADER_ATTRIBUTE_COLOR 3
#define SHADER_ATTRIBUTE_UV 4
#define SHADER_ATTRIBUTE_TANGET 5
#pragma lhgMultiOff()

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_SHADER {

        #pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_SHADER)
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
            Program();
            Program(Program &other);
            Program(Shader *shaders[], int shaderCount,
                    bool vertices = true, bool uvs = true, bool normals = true);
            ~Program();

            void operator=(const Program &rhs);

            GLuint getProgramID() { return programID; }

            bool usesVertices() { return vertices; }
            bool usesUVs() { return uvs; }
            bool usesNormals() { return normals; }

            GLint getUniformID(string name);

            bool isLinked() { return linked; }

        protected:
            GLuint programID;
            bool linked;

            bool vertices;
            bool uvs;
            bool normals;

            map<string, GLint> uniformCache;
        };

        void cleanupShaders();
        void cleanupPrograms();
        #pragma lhgMultiOff()

    }
}

#endif //SWARMENGINE_SHADERHANDLER_H
