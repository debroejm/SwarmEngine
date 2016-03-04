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
            Bone(vec3 pos, string name);
            Bone(vec3 pos, Bone &parent);
            Bone(vec3 pos, string name, Bone &parent);
            Bone(Bone &bone);
            Bone(Bone &bone, string name);
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

            string getName() { return name; }

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

            Bone* getBone(int index);
            Bone* getBone(string tag);

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

            Bone* skeleton = NULL;
            int boneCount;
            vec3* bonePositions= NULL;
            vec3* normals = NULL;
            vec3* normalsStatic = NULL;
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
