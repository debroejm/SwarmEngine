#ifndef SWARMENGINE_CONTROLS_H
#define SWARMENGINE_CONTROLS_H

// External Libraries
#define GLEW_STATIC
#include <GL/glew.h>

#include <glfw/glfw3.h>

//#define GLM_FORCE_CXX98
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


#include "../Definitions.h"

// --------------------
// | CONTROLS SECTION |
// --------------------

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_INPUT {

        #pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_INPUT)
        void computeMatricesFromInputs();
        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
        void setWindow(GLFWwindow* windowIN);
        GLFWwindow* getWindow();
        #pragma lhgMultiOff()

    }
}

#endif //SWARMENGINE_CONTROLS_H
