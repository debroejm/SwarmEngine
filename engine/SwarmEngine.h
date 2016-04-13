#pragma once


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




namespace Swarm {
    namespace Texture {

        
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

    namespace Anim {

        

    }

    namespace Config {

        
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

    namespace Input {

        
        void computeMatricesFromInputs();
        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
        void setWindow(GLFWwindow* windowIN);
        GLFWwindow* getWindow();

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
}





namespace Swarm {
    namespace Logging {

        
        void ChangeLoggingLevel(int newLoggingLevel);
        void ChangeLoggingType(int newLoggingType);
        
        void initLogging();
        void initLogging(int newLoggingLevel, int newLoggingType);
        void cleanupLogging();
        
        void Log(int level, const char * category, const char * message);
        //void Log(int level, const char * category, const char * message, std::vector< int > intInputs);
        
        string formatVec3(glm::vec3 pos);
        string formatVec3(float x, float y, float z);

    }

    namespace Model {

        
        void cleanupBuffers();
        
        class Bone
        {
        public:
        Bone();
        Bone(glm::vec3 pos);
        Bone(glm::vec3 pos, string name);
        Bone(glm::vec3 pos, Bone &parent);
        Bone(glm::vec3 pos, string name, Bone &parent);
        Bone(Bone &bone);
        Bone(Bone &bone, string name);
        ~Bone();
        
        void revertPosition();
        void addPosition(glm::vec3 newPos);
        void setPosition(glm::vec3 newPos);
        void rotatePosition(float angle, glm::vec3 amount);
        void rotatePosition(glm::mat4 rotMatrix);
        
        void updateBoneBuffer();
        
        void setParent(Bone &parent) { this->parent = &parent; }
        Bone* getParent() { return parent; }
        
        void addChild(Bone &child);
        vector<Bone*> getChildren() { return children; }
        
        void addBonePosition(glm::vec3 &bonePos);
        void addNormal(glm::vec3 &normal);
        void addNormal(glm::vec3 &normal, glm::vec3 &normalStatic);
        vector<glm::vec3*> getBonePositions() { return bonePositions; }
        vector<glm::vec3*> getNormals() { return normals; }
        
        glm::vec3 getPosition();
        glm::vec3 getRelativePosition();
        glm::mat4 getRotationMatrix();
        
        string getName() { return name; }
        
        string printHierarchy();
        string printHierarchy(string header);
        
        void operator=(const Bone &rhs);
        
        private:
        string name;
        glm::vec3 originalPos;
        glm::vec3 position;
        glm::mat4 rotationMatrix = glm::mat4(1.0f);
        Bone* parent;
        vector<Bone*> children;
        vector<glm::vec3*> bonePositions;
        vector<glm::vec3*> normals;
        vector<glm::vec3*> normalsStatic;
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
        void setTexture(Texture::AnimatedTexture other);
        
        GLuint getBoneBuffer();
        GLuint getUVBuffer();
        GLuint getNormalBuffer();
        GLuint getElementBuffer();
        int getElementCount();
        GLuint getTexture();
        
        Bone* getBone(int index);
        Bone* getBone(string tag);
        
        bool isLoaded();
        string getName() { return name; }
        
        glm::vec3 getMaxDimensions();
        glm::vec3 getMinDimensions();
        
        glm::vec3 getBonePosition(int index);
        void addBonePosition(int index, glm::vec3 pos);
        void setBonePosition(int index, glm::vec3 pos);
        void rotateBonePosition(int index, float angle, glm::vec3 amount);
        
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
        
        Bone* skeleton = NULL;
        int boneCount;
        glm::vec3* bonePositions= NULL;
        glm::vec3* normals = NULL;
        glm::vec3* normalsStatic = NULL;
        int elementCount;
        
        //vector<Bone> skeleton;
        //vector<glm::vec3> bones;
        //vector<glm::vec3> normals;
        
        glm::vec3 maxDim;
        glm::vec3 minDim;
        
        Texture::AnimatedTexture texture;
        };

    }

    namespace Render {

        

    }

    namespace Shader {

        
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

    namespace Texture {

        
        GLuint loadPNGTexture(const char* filename);
        void registerTexture(GLuint textureID);
        void cleanupTextures();

    }

    namespace Render {

        void init();
        void cleanup();
        
        void ChangeShader(Shader::Program &newProgram);
        void Render(Model::Model & object);
        void Render(Model::Model & object, glm::mat4 modelMatrix);

    }

    namespace Anim {

        class Rigging {
        public:
        Rigging(Model::Model &input);
        virtual ~Rigging() {}
        
        virtual void apply() = 0;
        virtual void update(float delta) = 0;
        
        protected:
        Model::Model* model = NULL;
        };
        
        class RiggingHumanoid: public Rigging {
        public:
        RiggingHumanoid(Model::Model &input);
        
        void apply();
        void update(float delta);
        
        protected:
        Model::Bone* leftKnee = NULL;
        Model::Bone* rightKnee = NULL;
        Model::Bone* leftFoot = NULL;
        Model::Bone* rightFoot = NULL;
        };
    }
}
