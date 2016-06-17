#include "AnimatedTexture.h"

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_TEXTURE {

        AnimatedTexture::AnimatedTexture(GLuint textureID)
        {
            addTexture(textureID);
            currentIndex = 0;
            lastTime = glfwGetTime();
            changeDelay.push_back(1.0);
        }

        AnimatedTexture::AnimatedTexture(const char * textureName)
        {
            addTexture(textureName);
            currentIndex = 0;
            lastTime = glfwGetTime();
            changeDelay.push_back(1.0);
        }

        AnimatedTexture::AnimatedTexture()
        {
            currentIndex = 0;
            lastTime = glfwGetTime();
        }

        AnimatedTexture::~AnimatedTexture()
        {

        }

        void AnimatedTexture::addTexture(GLuint textureID)
        {
            textureList.push_back(textureID);
            changeDelay.push_back(1.0);
        }

        void AnimatedTexture::addTexture(const char *textureName)
        {
            textureList.push_back(loadPNGTexture(textureName));
            changeDelay.push_back(1.0);
        }

        void AnimatedTexture::addTexture(GLuint textureID, double interval)
        {
            textureList.push_back(textureID);
            changeDelay.push_back(interval);
        }

        void AnimatedTexture::addTexture(const char *textureName, double interval)
        {
            textureList.push_back(loadPNGTexture(textureName));
            changeDelay.push_back(interval);
        }

        GLuint AnimatedTexture::getTexture()
        {
            if(textureList.size() == 0) return 0;
            int tIndex = currentIndex+1;
            if(tIndex > (changeDelay.size() - 1)) tIndex = 0;
            if( (glfwGetTime() - lastTime) > changeDelay[tIndex] )
            {
                currentIndex++;
                if(currentIndex > (textureList.size() - 1)) currentIndex = 0;
                lastTime = glfwGetTime();
            }
            GLuint tx = textureList[currentIndex];
            return tx;
        }

        void AnimatedTexture::operator=(const AnimatedTexture &rhs) {
            (*this).textureList = rhs.textureList;
            (*this).changeDelay = rhs.changeDelay;
            (*this).currentIndex = 0;
            (*this).lastTime = glfwGetTime();
        };


    }
}