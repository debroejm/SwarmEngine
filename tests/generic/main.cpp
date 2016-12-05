#include "Core.h"
#include "Render.h"
#include "CLEngine.h"

#include <math.h>

using namespace Swarm;
using namespace std;

using namespace Swarm::Logging;

#define PI 3.141592f
#define TWO_PI PI*2.0f

int main() {

    // Initialization
    if(!Init::init()) {
        return -1;
    }

    //if(CL::all_device_info.empty()) Log::log_cl(DEBUG) << "No CL Devices Found";
    //else Log::log_cl(DEBUG) << "Auto-Selected CL Device: " << CL::all_device_info.end()->deviceName;

    GLFWwindow * window = Input::getWindow();

    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);

    // Load our Shaders
    Render::Shader vertexShader("resources/shaders/SimpleShader_Vertex.glsl", GL_VERTEX_SHADER);
    Render::Shader fragmentShader("resources/shaders/SimpleShader_Fragment.glsl", GL_FRAGMENT_SHADER);
    Render::Shader* shaders[] { &vertexShader, &fragmentShader };
    Render::Program program(shaders, 2);

    // Renderer object
    Render::Renderer renderer(&program);
    Render::Camera camera(renderer, window, glm::vec3(0.0f, 4.0f, 9.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    // Keybindings
    Config::RawConfigData keybindingConfig("keybinding.config");
    Input::Keybinding EXIT(GLFW_KEY_ESCAPE, keybindingConfig, "Exit"); Input::addKeybinding(EXIT);
    Input::Keybinding FORWARD(GLFW_KEY_W, keybindingConfig, "Forward"); Input::addKeybinding(FORWARD);
    Input::Keybinding BACKWARD(GLFW_KEY_S, keybindingConfig, "Backward"); Input::addKeybinding(BACKWARD);
    Input::Keybinding LEFT(GLFW_KEY_A, keybindingConfig, "Left"); Input::addKeybinding(LEFT);
    Input::Keybinding RIGHT(GLFW_KEY_D, keybindingConfig, "Right"); Input::addKeybinding(RIGHT);
    Input::Keybinding COLOR_R_UP(GLFW_KEY_KP_7, keybindingConfig, "ColorRUp"); Input::addKeybinding(COLOR_R_UP);
    Input::Keybinding COLOR_R_DOWN(GLFW_KEY_KP_4, keybindingConfig, "ColorRDown"); Input::addKeybinding(COLOR_R_DOWN);
    Input::Keybinding COLOR_G_UP(GLFW_KEY_KP_8, keybindingConfig, "ColorGUp"); Input::addKeybinding(COLOR_G_UP);
    Input::Keybinding COLOR_G_DOWN(GLFW_KEY_KP_5, keybindingConfig, "ColorGDown"); Input::addKeybinding(COLOR_G_DOWN);
    Input::Keybinding COLOR_B_UP(GLFW_KEY_KP_9, keybindingConfig, "ColorBUp"); Input::addKeybinding(COLOR_B_UP);
    Input::Keybinding COLOR_B_DOWN(GLFW_KEY_KP_6, keybindingConfig, "ColorBDown"); Input::addKeybinding(COLOR_B_DOWN);

    // Textures
    Texture::SingleTexture cube_tex("resources/textures/box_diffuse.png");
    cube_tex.setTex(Texture::MapType::SPECULAR, "resources/textures/box_specular.png");
    cube_tex.setTex(Texture::MapType::NORMAL, "resources/textures/box_normal.png");
    cube_tex.setTex(Texture::MapType::EMISSIVE, "resources/textures/box_emissive.png");

    // Our Object
    Model::RawModelDataIndexed *data = Model::loadFromOBJ("resources/models/Cube.obj", true);
    Model::ModelSegment model(*data);
    Render::RenderObjectSimple object(model, cube_tex);
    object.scale(3.0f, 3.0f, 3.0f);
    Render::RenderObjectSimple object2(object);
    object2.translate(0.0f, -4.0f, 0.0f);
    Render::RenderObjectSimple object3(object);
    object3.translate(0.0f, 4.0f, 0.0f);

    // Register Objects
    renderer.registerRenderObject(&object);
    renderer.registerRenderObject(&object2);
    renderer.registerRenderObject(&object3);

    // Some Light Settings
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    float speed = 0.01f;

    float yaw = 0.0f;
    float pitch = 0.0f;

    double lastTime = glfwGetTime();

    // Main runtime loop
    while (!glfwWindowShouldClose(window))
    {
        double newTime = glfwGetTime();
        double deltaTime = newTime - lastTime;
        lastTime = newTime;

        bool input = false;
        if (LEFT) { yaw -= 0.01f; input = true; }
        if (RIGHT) { yaw += 0.01f; input = true; }
        if (FORWARD) { pitch -= 0.01f; input = true; }
        if (BACKWARD) { pitch += 0.01f; input = true; }
        if(!input) { yaw += 0.001f; pitch -= 0.001f; }

        if(yaw > TWO_PI) yaw = fmod(yaw, TWO_PI);
        if(pitch > TWO_PI) pitch = fmod(pitch, TWO_PI);
        if(yaw < 0.0f) yaw = TWO_PI - fmod(-yaw, TWO_PI);
        if(pitch < 0.0f) pitch = TWO_PI - fmod(-pitch, TWO_PI);

        input = false;
        if (COLOR_R_UP)   { lightColor.r += speed; input = true; }
        if (COLOR_R_DOWN) { lightColor.r -= speed; input = true; }
        if (COLOR_G_UP)   { lightColor.g += speed; input = true; }
        if (COLOR_G_DOWN) { lightColor.g -= speed; input = true; }
        if (COLOR_B_UP)   { lightColor.b += speed; input = true; }
        if (COLOR_B_DOWN) { lightColor.b -= speed; input = true; }
        if(lightColor.r < 0.0f) lightColor.r = 0.0f;
        if(lightColor.r > 1.0f) lightColor.r = 1.0f;
        if(lightColor.g < 0.0f) lightColor.g = 0.0f;
        if(lightColor.g > 1.0f) lightColor.g = 1.0f;
        if(lightColor.b < 0.0f) lightColor.b = 0.0f;
        if(lightColor.b > 1.0f) lightColor.b = 1.0f;
        if(input) Render::Uniforms::LightAmbientColor.setf(renderer, 1, 3, &lightColor[0]);

        double cursorX, cursorY;
        glfwGetCursorPos(window, &cursorX, &cursorY);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glm::vec3 lightDir((float)-((cursorX - width/2) / (width/2)), (float)((cursorY - height/2) / (height/2)), 1.0f);
        Render::Uniforms::LightAmbientDirection.setf(renderer, 1, 3, &lightDir[0]);

        object.resetMatrix();
        object.scale(3.0f, 3.0f, 3.0f);
        object.rotate(pitch, 1.0f, 0.0f, 0.0f);
        object.rotate(yaw, 0.0f, 1.0f, 0.0f);

        object2.rotate(0.001f, 0.0f, 1.0f, 0.0f);
        object3.rotate(-0.001f, 0.0f, 1.0f, 0.0f);

        camera.update(deltaTime);

        renderer.start();
        //Input::computeMatricesFromInputs();
        renderer.renderAll();
        renderer.end();

        if (EXIT)
            glfwSetWindowShouldClose( window, GL_TRUE );

        glfwPollEvents();
    }

    // Clean everything up
    Init::cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();

    keybindingConfig.writeConfigData();

    return 0;
}