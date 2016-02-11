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
            Program();
            Program(Shader *shaders[], int shaderCount);
            Program(Shader *shaders[], int shaderCount, bool vertices, bool uvs, bool normals);
            ~Program();

            void operator=(const Program &rhs);

            GLuint getProgramID();

            bool usesVertices() { return vertices; }
            bool usesUVs() { return uvs; }
            bool usesNormals() { return normals; }

            GLint getAttribID_vertex() { return attribID_vertex; }
            GLint getAttribID_uv() { return attribID_uv; }
            GLint getAttribID_normal() { return attribID_normal; }

            GLint getUniformID_model() { return uniformID_model; }
            GLint getUniformID_view() { return uniformID_view; }
            GLint getUniformID_projection() { return uniformID_projection; }

            GLint getUniformID_texture() { return uniformID_texture; }

            void findIDs();

            bool isLinked() { return linked; }

        protected:
            GLuint programID;
            bool linked;

            bool vertices;
            bool uvs;
            bool normals;

            GLint attribID_vertex;
            GLint attribID_uv;
            GLint attribID_normal;

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
