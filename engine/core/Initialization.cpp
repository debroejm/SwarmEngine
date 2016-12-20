#include "../Core.h"

#include "../Render.h"
#include "../CLEngine.h"
#include "GLFWCallbacks.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Init {

        bool init(unsigned int flags) {

            if(flags & SWM_INIT_RENDER) {

                // Initialize GLFW
                Log::log_core(INFO) << "Initializing GLFW";
                glfwSetErrorCallback(GLFW::error_callback);
                if (!glfwInit()) {
                    Log::log_core(FATAL) << "Failed to Initialize GLFW!";
                    return false;
                }

                // Might need to move this to renderer init
                Render::init();
            }

            if(flags & SWM_INIT_CL) {
                CL::init();
            }

            return true;

        }

        void cleanup() {
            Texture::cleanupTextures();
            Model::cleanupBuffers();
            Render::cleanupShaders();
            Render::cleanupPrograms();
            Render::cleanup();
            CL::cleanup();

            Log::cleanupAll();
        }


    }
}