#ifndef SWARMENGINE_ANIMATEDTEXTURE_H
#define SWARMENGINE_ANIMATEDTEXTURE_H

// External Libraries
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

//#define GLFW_DLL
#include <glfw/glfw3.h>

// Internal Libraries
#include "TextureHandler.h"

#include "Definitions.h"


// --------------------
// | TEXTURES SECTION |
// --------------------

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_TEXTURE {

        #pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_TEXTURE)
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
        #pragma lhgMultiOff()

    }
}

#endif //SWARMENGINE_ANIMATEDTEXTURE_H
