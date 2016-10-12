#include "Core.h"
#include "Render.h"

using namespace glm;
using namespace Swarm;
using namespace std;

using namespace Swarm::Logging;

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

    float data_vertex[]{
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f
    };
    float data_uv[]{
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            1.0f, 1.0f
    };
    float data_normal[]{
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f
    };

    Model::RawModelData data;
    data.setData(Model::MDT_Vertex, data_vertex, 4);
    data.setData(Model::MDT_UV, data_uv, 4);
    data.setData(Model::MDT_Normal, data_normal, 4);

    Log::log_core(INFO) << "Vertices: " << (data.hasData(Model::MDT_Vertex) ? "True" : "False")
                        << ", UVs: " << (data.hasData(Model::MDT_UV) ? "True" : "False")
                        << ", Normals: " << (data.hasData(Model::MDT_Normal) ? "True" : "False")
                        << ", Colors: " << (data.hasData(Model::MDT_Color) ? "True" : "False");
    Log::log_core(INFO) << "Size: " << data.getSize();

    Log::log_core(INFO) << data.getSize() << ": ";
    float* data_array = data.getData(Model::MDT_Vertex);
    for(int i = 0; i < data.getSize()*3; i++) Log::log_core << data_array[i] << ", ";

    Model::RawModelDataIndexed* data_indexed = data.index();
    Log::log_core(INFO) << data_indexed->getSize() << ": ";
    data_array = data_indexed->getData(Model::MDT_Vertex);
    for(int i = 0; i < data_indexed->getSize()*3; i++) Log::log_core << data_array[i] << ", ";

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