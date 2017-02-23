#include "api/Core.h"
#include "api/Logging.h"
#include "api/Render.h"
#include "api/CLEngine.h"
#include "api/Util.h"

#include <math.h>
#include <iostream>

using namespace Swarm;
using namespace std;

using namespace Swarm::Logging;

#define PI 3.141592f
#define TWO_PI PI*2.0f

// Keybindings
Input::Keybinding EXIT;
Input::Keybinding FORWARD;
Input::Keybinding BACKWARD;
Input::Keybinding LEFT;
Input::Keybinding RIGHT;
Input::Keybinding COLOR_R_UP;
Input::Keybinding COLOR_R_DOWN;
Input::Keybinding COLOR_G_UP;
Input::Keybinding COLOR_G_DOWN;
Input::Keybinding COLOR_B_UP;
Input::Keybinding COLOR_B_DOWN;
Input::Keybinding SHOW_ALPHA;
Input::Keybinding SHOW_BETA;
Input::Keybinding DEBUG_KEY;

// Light Settings
float light_color[3]{1.0f, 1.0f, 1.0f};
float speed = 1.0f;

// Position Settings
float yaw = 0.0f;
float pitch = 0.0f;

// Custom Uniforms
Render::Uniform uniform_light_ambient_color("_ambient_light_color");
Render::Uniform uniform_light_ambient_direction("_ambient_light_direction");

// Windows
Render::Window window1;
Render::Window window2;

void main_cycle(double delta_time);

int main() {

    try {

        // Initialization
        if (!Core::init()) {
            return -1;
        }

        std::set<CL::Device*> all_devices = CL::Device::getAll();
        CL::Device* best_device = *all_devices.rbegin();
        if (all_devices.empty()) Log::log_cl(DEBUG) << "No CL Devices Found";
        else Log::log_cl(DEBUG) << "Auto-Selected CL Device: " << best_device->name();

        Render::RenderObjectCollection all_render_objects;

        // Load our Shaders
        Render::Shader* shader_vertex   = Render::Shader::compileFromFile("resources/shaders/SimpleShader_Vertex.glsl",   Render::ShaderType::VERTEX);
        Render::Shader* shader_fragment = Render::Shader::compileFromFile("resources/shaders/SimpleShader_Fragment.glsl", Render::ShaderType::FRAGMENT);
        Render::Shader* shaders[]{shader_vertex, shader_fragment};
        Render::Program* program = Render::Program::compile(shaders, 2);

        // Renderer object
        Render::Renderer* renderer = Render::Renderer::create(program);

        window1 = Render::Window(1600, 900, "Swarm Engine Test - Generic Alpha");
        window2 = Render::Window(800, 600,  "Swarm Engine Test - Generic Beta");

        window1.attachRenderer(renderer, all_render_objects);
        window2.attachRenderer(renderer, all_render_objects);

        window1.setClearColor(0.0f, 0.0f, 0.75f);
        window2.setClearColor(0.75f, 0.0f, 0.0f);

        // Cameras
        Render::Camera* cam1 = Render::Camera::create(Render::CameraPosition(
                0.0f, 4.0f, 9.0f,   // Position
                0.0f, 0.0f, 0.0f,   // LookAt
                0.0f, 1.0f, 0.0f)); // Up
        Render::Camera* cam2 = Render::Camera::create(Render::CameraPosition(
                0.0f, -4.0f, 9.0f,   // Position
                0.0f, 0.0f, 0.0f,   // LookAt
                0.0f, 1.0f, 0.0f)); // Up

        window1.setCamera(cam1);
        window2.setCamera(cam2);

        // Keybindings
        Config::RawConfigData keybindingConfig("keybinding.config");
        EXIT = Input::Keybinding(         Input::KeyType::K_ESCAPE, keybindingConfig, "Exit");
        FORWARD = Input::Keybinding(      Input::KeyType::K_W,      keybindingConfig, "Forward");
        BACKWARD = Input::Keybinding(     Input::KeyType::K_S,      keybindingConfig, "Backward");
        LEFT = Input::Keybinding(         Input::KeyType::K_A,      keybindingConfig, "Left");
        RIGHT = Input::Keybinding(        Input::KeyType::K_D,      keybindingConfig, "Right");
        COLOR_R_UP = Input::Keybinding(   Input::KeyType::K_KP_7,   keybindingConfig, "ColorRUp");
        COLOR_R_DOWN = Input::Keybinding( Input::KeyType::K_KP_4,   keybindingConfig, "ColorRDown");
        COLOR_G_UP = Input::Keybinding(   Input::KeyType::K_KP_8,   keybindingConfig, "ColorGUp");
        COLOR_G_DOWN = Input::Keybinding( Input::KeyType::K_KP_5,   keybindingConfig, "ColorGDown");
        COLOR_B_UP = Input::Keybinding(   Input::KeyType::K_KP_9,   keybindingConfig, "ColorBUp");
        COLOR_B_DOWN = Input::Keybinding( Input::KeyType::K_KP_6,   keybindingConfig, "ColorBDown");
        SHOW_ALPHA = Input::Keybinding(   Input::KeyType::K_LEFT_CONTROL, keybindingConfig, "ShowAlpha");
        SHOW_BETA = Input::Keybinding(    Input::KeyType::K_RIGHT_CONTROL, keybindingConfig, "ShowBeta");
        DEBUG_KEY = Input::Keybinding(    Input::KeyType::K_KP_0,   keybindingConfig, "Debug");

        // Load Texture
        Texture::TexMap tex_cube_diffuse  = Texture::loadTexFromFile("resources/textures/box_diffuse.png",  Texture::PNG);
        Texture::TexMap tex_cube_specular = Texture::loadTexFromFile("resources/textures/box_specular.png", Texture::PNG);
        Texture::TexMap tex_cube_normal   = Texture::loadTexFromFile("resources/textures/box_normal.png",   Texture::PNG);
        Texture::TexMap tex_cube_emissive = Texture::loadTexFromFile("resources/textures/box_emissive.png", Texture::PNG);
        Texture::Texture tex_cube;
        tex_cube.put(Texture::Type::DIFFUSE,  tex_cube_diffuse);
        tex_cube.put(Texture::Type::SPECULAR, tex_cube_specular);
        tex_cube.put(Texture::Type::NORMAL,   tex_cube_normal);
        tex_cube.put(Texture::Type::EMISSIVE, tex_cube_emissive);

        // Setup Texture Uniforms
        renderer->setTextureName(Texture::Type::DIFFUSE,  "_texture_diffuse");
        renderer->setTextureName(Texture::Type::SPECULAR, "_texture_specular");
        renderer->setTextureName(Texture::Type::NORMAL,   "_texture_normal");
        renderer->setTextureName(Texture::Type::EMISSIVE, "_texture_emissive");

        // Our Object
        Model::RawModelData model_cube_data = Model::loadFromOBJ("resources/models/Cube.obj");
        Model::computeTangents(model_cube_data);
        Model::RawModelDataIndexed model_cube_data_indexed = model_cube_data.index();
        Model::Model model_cube(model_cube_data_indexed);
        Render::RenderObject* render_object_cube_1 = Render::RenderObject::createStaticRenderObject(
                model_cube, tex_cube,
                0.0f, 0.0f, 0.0f,
                3.0f, 3.0f, 3.0f,
                0.0f, 0.0f, 0.0f);
        Render::RenderObject* render_object_cube_2 = Render::RenderObject::createStaticRenderObject(
                model_cube, tex_cube,
                0.0f, -4.0f, 0.0f,
                3.0f,  3.0f, 3.0f,
                0.0f,  0.5f, 0.0f);
        Render::RenderObject* render_object_cube_3 = Render::RenderObject::createStaticRenderObject(
                model_cube, tex_cube,
                0.0f, 4.0f, 0.0f,
                3.0f, 3.0f, 3.0f,
                0.0f, -0.5f, 0.0f);

        // Register Objects
        all_render_objects.insert(render_object_cube_1);
        all_render_objects.insert(render_object_cube_2);
        all_render_objects.insert(render_object_cube_3);

        // Setup Custom Uniforms
        renderer->insertUniform(uniform_light_ambient_color);
        renderer->insertUniform(uniform_light_ambient_direction);

        uniform_light_ambient_color.setf(1, 3, &light_color[0]);

        window1.setVisible(true);
        //window2->setVisible(true);

        // A thing

        Log::log_core(DEBUG) << "Starting Main Cycle Loop";

        // Main runtime loop
        Core::start(main_cycle);

        // Clean everything up
        Log::log_core(DEBUG) << "Cleaning Up";
        Core::cleanup();

        keybindingConfig.writeConfigData();

    } catch(exception &e) {
        std::cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}

void main_cycle(double delta_time) {
    bool input = false;
    if (LEFT) {
        yaw -= 0.01f;
        input = true;
    }
    if (RIGHT) {
        yaw += 0.01f;
        input = true;
    }
    if (FORWARD) {
        pitch -= 0.01f;
        input = true;
    }
    if (BACKWARD) {
        pitch += 0.01f;
        input = true;
    }
    if (!input) {
        yaw += 0.001f;
        pitch -= 0.001f;
    }

    if (yaw > TWO_PI) yaw = fmod(yaw, TWO_PI);
    if (pitch > TWO_PI) pitch = fmod(pitch, TWO_PI);
    if (yaw < 0.0f) yaw = TWO_PI - fmod(-yaw, TWO_PI);
    if (pitch < 0.0f) pitch = TWO_PI - fmod(-pitch, TWO_PI);

    input = false;
    if (COLOR_R_UP) {
        light_color[0] += speed * delta_time;
        input = true;
    }
    if (COLOR_R_DOWN) {
        light_color[0] -= speed * delta_time;
        input = true;
    }
    if (COLOR_G_UP) {
        light_color[1] += speed * delta_time;
        input = true;
    }
    if (COLOR_G_DOWN) {
        light_color[1] -= speed * delta_time;
        input = true;
    }
    if (COLOR_B_UP) {
        light_color[2] += speed * delta_time;
        input = true;
    }
    if (COLOR_B_DOWN) {
        light_color[2] -= speed * delta_time;
        input = true;
    }
    if (light_color[0] < 0.0f) light_color[0] = 0.0f;
    if (light_color[0] > 1.0f) light_color[0] = 1.0f;
    if (light_color[1] < 0.0f) light_color[1] = 0.0f;
    if (light_color[1] > 1.0f) light_color[1] = 1.0f;
    if (light_color[2] < 0.0f) light_color[2] = 0.0f;
    if (light_color[2] > 1.0f) light_color[2] = 1.0f;
    if (input) uniform_light_ambient_color.setf(1, 3, &light_color[0]);

    double cursorX, cursorY;
    window1.cursorPos(cursorX, cursorY);
    int width = window1.width();
    int height = window1.height();
    float light_dir[3]{(float) -((cursorX - width / 2) / (width / 2)),
                       (float)((cursorY - height / 2) / (height / 2)), 1.0f};
    uniform_light_ambient_direction.setf(1, 3, &light_dir[0]);

    if (EXIT) Core::stop();
    if(!(window1.visible() || window2.visible())) Core::stop();

    if (SHOW_ALPHA) window1.setVisible(true);
    if (SHOW_BETA) window2.setVisible(true);

    if (DEBUG_KEY) {
        Log::log_core(DEBUG) << "Window 1 Size: " << window1.width() << ", " << window1.height();
        Log::log_core(DEBUG) << "Window 2 Size: " << window2.width() << ", " << window2.height();
    }

    //object.resetMatrix();
    //object.scale(3.0f, 3.0f, 3.0f);
    //object.rotate(pitch, 1.0f, 0.0f, 0.0f);
    //object.rotate(yaw, 0.0f, 1.0f, 0.0f);

    //object2.rotate(0.001f, 0.0f, 1.0f, 0.0f);
    //object3.rotate(-0.001f, 0.0f, 1.0f, 0.0f);
}