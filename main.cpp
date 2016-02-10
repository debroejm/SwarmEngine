// External Libraries
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>

#include <stdlib.h>

#define GLEW_NO_GLU
#include <GL/glew.h>

//#define GLFW_DLL
#include <glfw/glfw3.h>

#include <glm/glm.hpp>

#include "engine/SwarmEngine.h"

using namespace glm;
using namespace Swarm;
using namespace std;

int main() {

    if(!Init::init(LOGGING_LEVEL_MOST, LOGGING_TYPE_FILE_AND_CONSOLE)) {
        return -1;
    }

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Load our Shaders
    Shaders::Shader vertexShader("Resources/Shaders/TextureVShader.glsl", GL_VERTEX_SHADER);
    Shaders::Shader fragmentShader("Resources/Shaders/TextureFShader.glsl", GL_FRAGMENT_SHADER);
    Shaders::Shader* shaders[] { &vertexShader, &fragmentShader };
    Shaders::Program program(shaders, 2);

    Rendering::ChangeShader(&program);

    Models::Model testModel;
    testModel.loadMMD("Resources/Models/BasicMinion.mmd");
    testModel.setTexture(Textures::AnimatedTexture("Resources/Models/BasicMinion.png"));

    //Models::Model cube;
    //cube.loadOBJ("cube.obj");
    //cube.setTexture(Textures::AnimatedTexture("DSD_Block_Buildable.png"));

    GLFWwindow * window = Controls::getWindow();

    // Main runtime loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Controls::computeMatricesFromInputs();

        vec3 tPos(0.0f, 0.05f, 0.0f);
        testModel.changeBonePosition(0, tPos);
        testModel.updateBoneBuffer();

        glm::mat4 transMat = glm::translate( vec3(0.0f, 2.0f, 0.0f) );
        //Rendering::Render(cube, transMat);
        Rendering::Render(testModel);

        if (glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS)
            glfwSetWindowShouldClose( window, GL_TRUE );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Init::cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}