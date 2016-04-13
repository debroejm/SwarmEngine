#ifndef SWARMENGINE_MODELLOADING_H
#define SWARMENGINE_MODELLOADING_H

// External Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>


// Internal Libraries
#include "Logging.h"
#include "AnimatedTexture.h"

#include "../Definitions.h"

#define MAX_BONE_CHILDREN 8


// -------------------
// | MODELS SECTION |
// -------------------

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_MODEL {

        #pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_MODEL)
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
            void setTexture(ENGINE_NAMESPACE_TEXTURE::AnimatedTexture other);

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

            ENGINE_NAMESPACE_TEXTURE::AnimatedTexture texture;
        };
        #pragma lhgMultiOff()
    }
}

#endif //SWARMENGINE_MODELLOADING_H
