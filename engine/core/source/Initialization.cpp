#include "../headers/Initialization.h"

namespace ENGINE_NAMESPACE {
    namespace Init {

        bool init() { return init( LOGGING_LEVEL_NORMAL, LOGGING_TYPE_FILE_AND_CONSOLE, "Swarm Engine Instance", 1600, 900 ); }
        bool init(const char * windowName) { return init( LOGGING_LEVEL_NORMAL, LOGGING_TYPE_FILE_AND_CONSOLE, windowName, 1600, 900 ); }
        bool init(const char * windowName, int windowX, int windowY) { return init( LOGGING_LEVEL_NORMAL, LOGGING_TYPE_FILE_AND_CONSOLE, windowName, windowX, windowY ); }
        bool init(int logLevel, int logType) { return init( logLevel, logType, "Swarm Engine Instance", 1600, 900 ); }
        bool init(int logLevel, int logType, const char * windowName) { return init( logLevel, logType, windowName, 1600, 900 ); }

        bool init(int logLevel, int logType, const char * windowName, int windowX, int windowY) {

            GLFWwindow* window;

            Logging::initLogging(logLevel, logType);

            // Initialize GLFW
            Logging::Log(LOGGING_INFO, "GLFW", "Initializing GLFW");
            glfwSetErrorCallback(GLFW::error_callback);
            if (!glfwInit()) exit(EXIT_FAILURE);

            // Create our GLFW window, and make its context current
            window = glfwCreateWindow(windowX, windowY, windowName, NULL, NULL);
            if (!window)
            {
                glfwTerminate();
                exit(EXIT_FAILURE);
            }
            glfwMakeContextCurrent(window);

            Controls::setWindow(window);

            // Initialize GLEW
            glewExperimental = true; // Needed for core profile
            if (glewInit() != GLEW_OK) {
                fprintf(stderr, "Failed to initialize GLEW\n");
                return false;
            }

            // Black background
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


            Rendering::init();

            return true;

        }

        void cleanup() {
            Textures::cleanupTextures();
            Shaders::cleanupShaders();
            Shaders::cleanupPrograms();
            Rendering::cleanup();
        }


    }
}