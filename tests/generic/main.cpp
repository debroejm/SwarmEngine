#include "Core.h"
#include "Render.h"

using namespace Swarm;
using namespace std;

using namespace Swarm::Logging;

int main() {

    // Initialization
    if(!Init::init()) {
        return -1;
    }

    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);

    // Load our Shaders
    Render::Shader vertexShader("resources/shaders/SimpleShader_Vertex.glsl", GL_VERTEX_SHADER);
    Render::Shader fragmentShader("resources/shaders/SimpleShader_Fragment.glsl", GL_FRAGMENT_SHADER);
    Render::Shader* shaders[] { &vertexShader, &fragmentShader };
    Render::Program program(shaders, 2);

    // Renderer object
    Render::Camera camera(glm::vec3(-3.0f, 4.0f, 9.0f), glm::vec3(1.0f, 1.5f, 0.5f));
    Render::Renderer renderer(&program);
    renderer.changeCamera(&camera);

    // Keybindings
    Config::RawConfigData keybindingConfig("keybinding.config");
    Input::Keybinding EXIT(GLFW_KEY_ESCAPE, keybindingConfig, "Exit");
    Input::addKeybinding(EXIT);
    Input::Keybinding FORWARD(GLFW_KEY_W, keybindingConfig, "Forward");
    Input::addKeybinding(FORWARD);

    // Our Object
    Model::RawModelDataIndexed *data = Model::loadFromOBJ("resources/models/Cube.obj", true);
    Model::ModelSegment model(*data);
    Render::SimpleROS object(model);
    object.scale(3.0f, 3.0f, 3.0f);

    // Textures
    Texture::SingleTexture cube_tex("resources/textures/box_diffuse.png");
    cube_tex.setSpecular("resources/textures/box_specular.png");
    cube_tex.setNormal("resources/textures/box_normal.png");

    // Some Light Settings
    glUniform3f(program.getUniformID(Render::Uniforms::LightAmbientColor), 1.0f, 1.0f, 1.0f);
    glUniform3f(program.getUniformID(Render::Uniforms::LightAmbientDirection), 1.0f, 1.0f, 1.0f);

    // Main runtime loop
    GLFWwindow * window = Input::getWindow();
    while (!glfwWindowShouldClose(window))
    {

        renderer.start();

        // Do something fun to the object
        object.rotate(0.01f, 0.5f, 1.0f, 0.5f);

        if (FORWARD.isPressed())
            object.scale(0.99f, 0.99f, 0.99f);

        Input::computeMatricesFromInputs();

        cube_tex.bind();
        renderer.render(object);

        if (EXIT.isPressed())
            glfwSetWindowShouldClose( window, GL_TRUE );

        renderer.end();
        glfwPollEvents();
    }

    // Clean everything up
    Init::cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    keybindingConfig.writeConfigData();

    return 0;
}