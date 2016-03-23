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

#include "../engine/SwarmEngine.h"

using namespace glm;
using namespace Swarm;
using namespace std;

int main() {

    if(!Init::init(LOGGING_LEVEL_MOST, LOGGING_TYPE_FILE_AND_CONSOLE)) {
        return -1;
    }

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Load our Shaders
    Shaders::Shader vertexShader("resources/shaders/TextureVShader.glsl", GL_VERTEX_SHADER);
    Shaders::Shader fragmentShader("resources/shaders/TextureFShader.glsl", GL_FRAGMENT_SHADER);
    Shaders::Shader* shaders[] { &vertexShader, &fragmentShader };
    Shaders::Program program(shaders, 2);

    Rendering::ChangeShader(program);

    Models::Model testModel;
    //testModel.loadMMD("Resources/Models/BasicMinion.mmd");
    testModel.loadMMD("Resources/Models/BlockPerson.mmd");
    testModel.setTexture(Textures::AnimatedTexture("Resources/Models/BasicMinion.png"));
    Models::Model testModel2(testModel);
    Animation::RiggingHumanoid rigging(testModel2);
    rigging.apply();
    //testModel.getStringData();
    //char infoMsg[256];
    //vec3 bonePos = testModel.getBonePosition(0);
    //sprintf(infoMsg, "(%f, %f, %f)", bonePos.x, bonePos.y, bonePos.z);
    //Logging::Log(LOGGING_INFO, "Main", infoMsg);

    Configuration::RawConfigData keybindingConfig("keybinding.config");
    Configuration::Keybinding EXIT("Exit", GLFW_KEY_ESCAPE, keybindingConfig);
    Configuration::addKeybinding(EXIT);
    Configuration::Keybinding FORWARD("Forward", GLFW_KEY_O, keybindingConfig);
    Configuration::addKeybinding(FORWARD);

    Models::Model cube;
    cube.loadOBJ("Resources/Models/Cube.obj");
    cube.setTexture(Textures::AnimatedTexture("Resources/Models/Sample.png"));

    Logging::Log(LOGGING_INFO, "Debug", ("Model Info:\n"+testModel.printDebug()).c_str());
    Logging::Log(LOGGING_INFO, "Debug", ("Model Info:\n"+testModel2.printDebug()).c_str());

    GLFWwindow * window = Controls::getWindow();

    // Main runtime loop
    while (!glfwWindowShouldClose(window))
    {

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Controls::computeMatricesFromInputs();

        vec3 tPos(0.0f, 0.0f, 0.005f);
        cube.addBonePosition(0, tPos);
        testModel.rotateBonePosition(1, glfwGetTime(), vec3(1.0f, 0.0f, 1.0f));
        testModel2.rotateBonePosition(10, glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
        //testModel.setBonePosition(1, vec3(0, 0, 10));
        cube.updateBoneBuffer();
        testModel.updateBoneBuffer();
        testModel2.updateBoneBuffer();

        glm::mat4 transMat = glm::translate( vec3(10.0f, 0.0f, 0.0f) );
        Rendering::Render(cube, transMat);
        Rendering::Render(testModel);
        Rendering::Render(testModel2, transMat);

        if (EXIT.isPressed())
            glfwSetWindowShouldClose( window, GL_TRUE );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Init::cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    keybindingConfig.writeConfigData();

    return 0;
}