#include "Core.h"
#include "Render.h"

using namespace glm;
using namespace Swarm;
using namespace std;

int main() {

    if(!Init::init()) {
        return -1;
    }

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Load our Shaders
    Render::Shader vertexShader("resources/shaders/TextureVShader.glsl", GL_VERTEX_SHADER);
    Render::Shader fragmentShader("resources/shaders/TextureFShader.glsl", GL_FRAGMENT_SHADER);
    Render::Shader* shaders[] { &vertexShader, &fragmentShader };
    Render::Program program(shaders, 2);

    Render::Renderer renderer(&program);

    Model::Model testModel;
    //testModel.loadMMD("Resources/Models/BasicMinion.mmd");
    testModel.loadMMD("Resources/Models/BlockPerson.mmd");
    testModel.setTexture(Texture::AnimatedTexture("Resources/Models/BasicMinion.png"));
    Model::Model testModel2(testModel);
    Anim::RiggingHumanoid rigging(testModel2);
    rigging.apply();
    //testModel.getStringData();
    //char infoMsg[256];
    //vec3 bonePos = testModel.getBonePosition(0);
    //sprintf(infoMsg, "(%f, %f, %f)", bonePos.x, bonePos.y, bonePos.z);
    //Logging::Log(LOGGING_INFO, "Main", infoMsg);

    Config::RawConfigData keybindingConfig("keybinding.config");
    Input::Keybinding EXIT(GLFW_KEY_ESCAPE, keybindingConfig, "Exit");
    Input::addKeybinding(EXIT);
    Input::Keybinding FORWARD(GLFW_KEY_O, keybindingConfig, "Forward");
    Input::addKeybinding(FORWARD);

    Model::Model cube;
    cube.loadOBJ("Resources/Models/Cube.obj");
    cube.setTexture(Texture::AnimatedTexture("Resources/Models/Sample.png"));

    //Logging::Log(LOGGING_INFO, "Debug", ("Model Info:\n"+testModel.printDebug()).c_str());
    //Logging::Log(LOGGING_INFO, "Debug", ("Model Info:\n"+testModel2.printDebug()).c_str());

    GLFWwindow * window = Input::getWindow();

    // Main runtime loop
    while (!glfwWindowShouldClose(window))
    {

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Input::computeMatricesFromInputs();

        vec3 tPos(0.0f, 0.0f, 0.005f);
        if (FORWARD.isPressed()) cube.addBonePosition(0, tPos);
        testModel.rotateBonePosition(1, glfwGetTime(), vec3(1.0f, 0.0f, 1.0f));
        testModel2.rotateBonePosition(10, glfwGetTime(), vec3(0.0f, 1.0f, 0.0f));
        cube.updateBoneBuffer();
        testModel.updateBoneBuffer();
        testModel2.updateBoneBuffer();

        glm::mat4 transMat = glm::translate( vec3(10.0f, 0.0f, 0.0f) );
        renderer.render(cube, transMat);
        renderer.render(testModel);
        renderer.render(testModel2, transMat);

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