#ifndef SWARMENGINE_MODELLOADING_H
#define SWARMENGINE_MODELLOADING_H

// External Libraries
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

using namespace glm;


// Internal Libraries
#include "Logging.h"
#include "AnimatedTexture.h"

#include "../Definitions.h"

#define MAX_BONE_CHILDREN 8


// -------------------
// | MODELS SECTION |
// -------------------

namespace ENGINE_NAMESPACE {
    namespace Models {

        void cleanupBuffers();

        class Bone
        {
        public:
            Bone();
            Bone(vec3 pos);
            Bone(vec3 pos, Bone &parent);
            ~Bone();

            void revertPosition();
            void addPosition(vec3 newPos);
            void setPosition(vec3 newPos);

            void setParent(Bone &parent) { this->parent = &parent; }
            Bone* getParent() { return parent; }

            void addChild(Bone &child);
            void addBonePosition(vec3 &bonePos);
            void addNormal(vec3 &normal);

            glm::vec3 getPosition();

            void operator=(const Bone &rhs);

        private:
            vec3 originalPos;
            vec3 position;
            Bone* parent;
            vector<Bone*> children;
            vector<vec3*> bonePositions;
            vector<vec3*> normals;
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
            ~Model();
            bool loadOBJ(const char * path);
            bool loadMMD(const char * path);

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

            void updateBoneBuffer();

            string getStringData();

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
            int elementCount;

            //vector<Bone> skeleton;
            //vector<vec3> bones;
            //vector<vec3> normals;

            vec3 maxDim;
            vec3 minDim;

            Textures::AnimatedTexture texture;
        };



    }
}

#endif //SWARMENGINE_MODELLOADING_H
