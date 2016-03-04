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
#include <unordered_map>
using namespace std;

#include <stdlib.h>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

#define ENGINE_NAMESPACE Swarm

// Logging Level
#define LOGGING_LEVEL_MIN 1 // Just Errors
#define LOGGING_LEVEL_NORMAL 2 // Errors + Warnings
#define LOGGING_LEVEL_MOST 3 // All

// Logging Type
#define LOGGING_TYPE_NONE 0 // No log will be outputted
#define LOGGING_TYPE_FILE 1 // Output logging info to a file
#define LOGGING_TYPE_CONSOLE 2 // Only display logging info on the console
#define LOGGING_TYPE_FILE_AND_CONSOLE 3 // Output logging info to both the console and a file

// Logging Severety
#define LOGGING_ERROR 1
#define LOGGING_WARNING 2
#define LOGGING_INFO 3


// -----------
// Sub-Headers
// -----------
/*
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
 */

// Structures

namespace ENGINE_NAMESPACE {
    namespace Textures {
        class AnimatedTexture
        {
        public:
            AnimatedTexture(GLuint textureID);
            AnimatedTexture(const char * textureName);
            AnimatedTexture();
            ~AnimatedTexture();

            void addTexture(GLuint textureID);
            void addTexture(const char * textureName);
            void addTexture(GLuint textureID, double interval);
            void addTexture(const char * textureName, double interval);

            GLuint getTexture();

            void operator=(const AnimatedTexture &rhs);
        private:
            vector<GLuint> textureList;
            int currentIndex;
            vector<double> changeDelay;
            double lastTime;
        };
    }

    namespace Configuration {
        class RawConfigData {
        public:
            RawConfigData(const char * filepath);
            ~RawConfigData();

            void writeConfigData();
            void writeConfigData(const char * filepath);

            int getInteger(string key);
            int getInteger(string key, int defaultValue);
            void setInteger(string key, int value);

            string getString(string key);
            string getString(string key, string defaultValue);
            void setString(string key, string value);
        protected:
            const char * filepath;

            unordered_map<string, int> dataMap_int;
            unordered_map<string, string> dataMap_str;
        };

        class Keybinding {
        public:
            Keybinding(int key);
            Keybinding(string name, int key);
            Keybinding(string name, int key, RawConfigData &configData);
            ~Keybinding();

            bool isPressed();
            void setPressed(bool pressed);

            string getName() { return name; }
            int getKey() { return key; }
        protected:
            int key;
            bool pressed;
            string name;
        };

        void setPressed(int key, bool pressed);
        void setPressed(string name, bool pressed);
        bool isPressed(int key);
        bool isPressed(string name);

        Keybinding* getKeybinding(int key);
        Keybinding* getKeybinding(string name);
        void addKeybinding(Keybinding &binding);
    }

    namespace Controls {
        void computeMatricesFromInputs();
        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
        void setWindow(GLFWwindow* windowIN);
        GLFWwindow* getWindow();
    }

    namespace GLFW {
        void error_callback(int error, const char* description);
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    }

    namespace Init {
        bool init();
        bool init(const char * windowName);
        bool init(const char * windowName, int windowX, int windowY);
        bool init(int logLevel, int logType);
        bool init(int logLevel, int logType, const char * windowName);
        bool init(int logLevel, int logType, const char * windowName, int windowX, int windowY);

        void cleanup();
    }

    namespace Logging {
        void ChangeLoggingLevel(int newLoggingLevel);
        void ChangeLoggingType(int newLoggingType);

        void initLogging();
        void initLogging(int newLoggingLevel, int newLoggingType);
        void cleanupLogging();

        void Log(int level, const char * category, const char * message);

        string formatVec3(vec3 pos);
        string formatVec3(float x, float y, float z);
    }

    namespace Models {
        void cleanupBuffers();

        class Bone
        {
        public:
            Bone();
            Bone(vec3 pos);
            Bone(vec3 pos, string name);
            Bone(vec3 pos, Bone &parent);
            Bone(vec3 pos, string name, Bone &parent);
            ~Bone();

            void revertPosition();
            void addPosition(vec3 newPos);
            void setPosition(vec3 newPos);
            void rotatePosition(float angle, vec3 amount);
            void rotatePosition(mat4 rotMatrix);

            void updateBoneBuffer();

            void setParent(Bone &parent) { this->parent = &parent; }
            Bone* getParent() { return parent; }

            void addChild(Bone &child);
            vector<Bone*> getChildren() { return children; }

            void addBonePosition(vec3 &bonePos);
            void addNormal(vec3 &normal);
            void addNormal(vec3 &normal, vec3 &normalStatic);
            vector<vec3*> getBonePositions() { return bonePositions; }
            vector<vec3*> getNormals() { return normals; }

            vec3 getPosition();
            vec3 getRelativePosition();
            mat4 getRotationMatrix();



            string printHierarchy();
            string printHierarchy(string header);

            void operator=(const Bone &rhs);

        private:
            string name;
            vec3 originalPos;
            vec3 position;
            mat4 rotationMatrix = mat4(1.0f);
            Bone* parent;
            vector<Bone*> children;
            vector<vec3*> bonePositions;
            vector<vec3*> normals;
            vector<vec3*> normalsStatic;
        };

        struct bVert{
            float x;
            float y;
            float z;
            Bone* modifier;
        };

        class Model
        {
        public:
            Model();
            Model(string name);
            Model(Model &other);
            ~Model();
            bool loadOBJ(const char * path);
            bool loadMMD(const char * path);

            void operator=(Model &rhs);

            void addTexture(const char * textureName);
            void addTexture(GLuint textureID);
            void setTexture(Textures::AnimatedTexture other);

            GLuint getBoneBuffer();
            GLuint getUVBuffer();
            GLuint getNormalBuffer();
            GLuint getElementBuffer();
            int getElementCount();
            GLuint getTexture();

            bool isLoaded();
            string getName() { return name; }

            vec3 getMaxDimensions();
            vec3 getMinDimensions();

            vec3 getBonePosition(int index);
            void addBonePosition(int index, vec3 pos);
            void setBonePosition(int index, vec3 pos);
            void rotateBonePosition(int index, float angle, vec3 amount);

            void updateBoneBuffer();

            string getStringData();
            string printDebug();

        private:
            bool loaded;
            string name;

            GLuint bonebuffer;
            GLuint uvbuffer;
            GLuint normalbuffer;
            GLuint elementbuffer;
            int indexCount;

            Bone* skeleton;
            int boneCount;
            vec3* bonePositions;
            vec3* normals;
            vec3* normalsStatic;
            int elementCount;

            //vector<Bone> skeleton;
            //vector<vec3> bones;
            //vector<vec3> normals;

            vec3 maxDim;
            vec3 minDim;

            Textures::AnimatedTexture texture;
        };
    }

    namespace Animation {
        class Rigging {
        public:
            Rigging(Models::Model &input);
            virtual ~Rigging() {}

            virtual void apply() = 0;
            virtual void update(float delta) = 0;

        protected:
            Models::Model* model = NULL;
        };

        class RiggingHumanoid: public Rigging {
        public:
            RiggingHumanoid(Models::Model &input);

            void apply();
            void update(float delta);

        protected:
            Models::Bone* leftKnee = NULL;
            Models::Bone* rightKnee = NULL;
            Models::Bone* leftFoot = NULL;
            Models::Bone* rightFoot = NULL;
        };
    }

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

    namespace Rendering {
        void init();
        void cleanup();

        void ChangeShader(ENGINE_NAMESPACE::Shaders::Program &newProgram);
        void Render(ENGINE_NAMESPACE::Models::Model & object);
        void Render(ENGINE_NAMESPACE::Models::Model & object, glm::mat4 modelMatrix);
    }

    namespace Textures {
        GLuint loadPNGTexture(const char* filename);
        void registerTexture(GLuint textureID);
        void cleanupTextures();
    }
}

#endif //SWARMENGINE_CORE_H
