#include "../Core.h"

#include "../Render.h"
#include "GLFWCallbacks.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Init {

        bool init(Type type) {
            return init( "Swarm Engine Instance", type );
        }

        bool init(string windowName, Type type) {
            // TODO: Modify so that default size is determined by screen capabilities
            return init( 1600, 900, windowName, type );
        }

        bool init(int windowX, int windowY, string windowName, Type type) {

            if(type >= RENDERING) {
                GLFWwindow *window;

                // Initialize GLFW
                Log::log_core(INFO) << "Initializing GLFW";
                glfwSetErrorCallback(GLFW::error_callback);
                if (!glfwInit()) {
                    Log::log_core(FATAL) << "Failed to Initialize GLFW!";
                    exit(EXIT_FAILURE);
                }

                // Create our GLFW window, and make its context current
                window = glfwCreateWindow(windowX, windowY, windowName.c_str(), NULL, NULL);
                if (!window) {
                    glfwTerminate();
                    Log::log_core(FATAL) << "Failed to Create Window!";
                    exit(EXIT_FAILURE);
                }
                glfwMakeContextCurrent(window);

                glfwSetKeyCallback(window, GLFW::key_callback);

                glfwSetCursorPos(window, windowX / 2, windowY / 2);

                Input::setWindow(window);

                // Initialize GLEW
                glewExperimental = GL_TRUE; // Needed for core profile
                if (glewInit() != GLEW_OK) {
                    Log::log_core(FATAL) << "Failed to Initialize GLEW!";
                    return false;
                }

                // Black background
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

                Render::init();
            }

            return true;

        }

        void cleanup() {
            Texture::cleanupTextures();
            Model::cleanupBuffers();
            Render::cleanupShaders();
            Render::cleanupPrograms();
            Render::cleanup();

            Log::cleanupAll();
        }


    }
}