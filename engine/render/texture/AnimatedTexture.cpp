#include "../../Render.h"

namespace Swarm {
    namespace Texture {

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

        GLuint AnimatedTexture::getID() {
            if(textureList.empty()) return 0;
            int tIndex = currentIndex+1;
            if(tIndex > (changeDelay.size() - 1)) tIndex = 0;
            if( (glfwGetTime() - lastTime) > changeDelay[tIndex] ) {
                currentIndex++;
                if(currentIndex > (textureList.size() - 1)) currentIndex = 0;
                lastTime = glfwGetTime();
            }
            return textureList[currentIndex];
        }

        void AnimatedTexture::bind() {
            if(textureList.empty()) return;
            int tIndex = currentIndex+1;
            if(tIndex > (changeDelay.size() - 1)) tIndex = 0;
            if( (glfwGetTime() - lastTime) > changeDelay[tIndex] ) {
                currentIndex++;
                if(currentIndex > (textureList.size() - 1)) currentIndex = 0;
                lastTime = glfwGetTime();
            }
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureList[currentIndex]);
        }

        AnimatedTexture &AnimatedTexture::operator=(const AnimatedTexture &rhs) {
            this->textureList = rhs.textureList;
            this->changeDelay = rhs.changeDelay;
            this->currentIndex = 0;
            this->lastTime = glfwGetTime();
            return *this;
        };

        AnimatedTexture &AnimatedTexture::operator=(const SingleTexture &rhs) {
            this->textureList.clear();
            this->textureList.push_back(rhs.getTex(MapType::DIFFUSE));
            this->changeDelay.clear();
            this->changeDelay.push_back(1.0);
            this->currentIndex = 0;
            this->lastTime = glfwGetTime();
            return *this;
        }

        AnimatedTexture &AnimatedTexture::operator=(const GLuint &rhs) {
            this->textureList.clear();
            this->textureList.push_back(rhs);
            this->changeDelay.clear();
            this->changeDelay.push_back(1.0);
            this->currentIndex = 0;
            this->lastTime = glfwGetTime();
            return *this;
        }


    }
}