#include "Initialization.h"

using namespace ENGINE_NAMESPACE::ENGINE_NAMESPACE_LOG;

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_INIT {

        bool init() { return init( "Swarm Engine Instance", 1600, 900 ); }
        bool init(const char * windowName) { return init( windowName, 1600, 900 ); }

        bool init(const char * windowName, int windowX, int windowY) {

            GLFWwindow* window;

            // Initialize GLFW
            Log::log_global(INFO) << "Initializing GLFW";
            glfwSetErrorCallback(ENGINE_NAMESPACE_GLFW::error_callback);
            if (!glfwInit()) {
                Log::log_global(FATAL) << "Failed to Initialize GLFW!";
                exit(EXIT_FAILURE);
            }

            // Create our GLFW window, and make its context current
            window = glfwCreateWindow(windowX, windowY, windowName, NULL, NULL);
            if (!window)
            {
                glfwTerminate();
                Log::log_global(FATAL) << "Failed to Create Window!";
                exit(EXIT_FAILURE);
            }
            glfwMakeContextCurrent(window);

            glfwSetKeyCallback(window, ENGINE_NAMESPACE_GLFW::key_callback);

            glfwSetCursorPos(window, windowX/2, windowY/2);

            ENGINE_NAMESPACE_INPUT::setWindow(window);

            // Initialize GLEW
            glewExperimental = true; // Needed for core profile
            if (glewInit() != GLEW_OK) {
                Log::log_global(FATAL) << "Failed to Initialize GLEW!";
                return false;
            }

            // Black background
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


            ENGINE_NAMESPACE_RENDER::init();

            return true;

        }

        void cleanup() {
            ENGINE_NAMESPACE_TEXTURE::cleanupTextures();
            ENGINE_NAMESPACE_MODEL::cleanupBuffers();
            ENGINE_NAMESPACE_SHADER::cleanupShaders();
            ENGINE_NAMESPACE_SHADER::cleanupPrograms();
            ENGINE_NAMESPACE_RENDER::cleanup();

            Log::cleanupAll();
        }


    }
}