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

// -------------------
// | SHADERS SECTION |
// -------------------

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_SHADER {

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
                    bool vertices = true, bool uvs = true, bool normals = true,
                    const char * model = "_m", const char * view = "_v", const char * projection = "_p",
                    const char * texMap = "texturemap");
            ~Program();

            void operator=(const Program &rhs);

            GLuint getProgramID() { return programID; }

            bool usesVertices() { return vertices; }
            bool usesUVs() { return uvs; }
            bool usesNormals() { return normals; }

            GLint getUniformID(const char * name);
            GLint getUniformID_model() { return uniformID_model; }
            GLint getUniformID_view() { return uniformID_view; }
            GLint getUniformID_projection() { return uniformID_projection; }
            GLint getUniformID_texture() { return uniformID_texture; }

            bool isLinked() { return linked; }

        protected:
            bool findUniformIDs(const char * model, const char * view, const char * projection,
                                const char * texMap);

            GLuint programID;
            bool linked;

            bool vertices;
            bool uvs;
            bool normals;

            GLint uniformID_model;
            GLint uniformID_view;
            GLint uniformID_projection;

            GLint uniformID_texture;
        };

        void cleanupShaders();
        void cleanupPrograms();

    }
}

#endif //SWARMENGINE_SHADERHANDLER_H
