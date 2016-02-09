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

        class Bone
        {
        public:
            Bone(vec3 pos);
            Bone(vec3 pos, int PID);
            ~Bone();

            bool addChild(int childID);
            void revertPosition();
            void changePosition(vec3 newPos);

            bool addLinkedBonePos(int BonePosIndex);
            bool addLinkedNormalPos(int NormalPosIndex);

            void setModelBonesPointer(vector<vec3> * mBones);
            void setModelNormalsPointer(vector<vec3> * mNormals);
            void setSkeletonPointer(vector<Bone> * mSkeleton);

            glm::vec3 getPosition();

        private:
            vec3 originalPos;
            vec3 position;
            vector<int> children;
            int parentID;
            vector<vec3> * modelBones;
            vector<vec3> * modelNormals;
            vector<Bone> * modelSkeleton;
            vector<int> boneIndices;
            vector<int> normalIndices;
        };

        struct bVert{
            float x;
            float y;
            float z;
            Bone *modifier;
        };

        class Model
        {
        public:
            Model();
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

            vec3 getMaxDimensions();
            vec3 getMinDimensions();

            vec3 getBonePosition(int index);
            void changeBonePosition(int index, vec3 pos);

            void updateBoneBuffer();

        private:
            bool loaded;

            GLuint bonebuffer;
            GLuint uvbuffer;
            GLuint normalbuffer;
            GLuint elementbuffer;
            int indexCount;

            vector<Bone> skeleton;
            vector<vec3> bones;
            vector<vec3> normals;

            vec3 maxDim;
            vec3 minDim;

            Textures::AnimatedTexture texture;
        };



    }
}

#endif //SWARMENGINE_MODELLOADING_H
