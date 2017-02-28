#define SWARM_INCLUDE_GLFW
#include "api/Core.h"

#include "api/Logging.h"
#include "api/CLEngine.h"
#include "render/RenderInternal.h"
#include "api/Util.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Core {

        void callback_error(int error, const char* description)
        {
            Log::log_render(ERR) <<  "GLFW ERROR '" << error << "': " << description;
        }

        State _static_state = UNINITIALIZED;
        State state() { return _static_state; }

        bool init(size_t flags) {

            if(flags & 0b00000010) {

                // Initialize GLFW
                Log::log_core(INFO) << "Initializing GLFW";
                glfwSetErrorCallback(callback_error);
                if (!glfwInit()) {
                    Log::log_core(FATAL) << "Failed to Initialize GLFW!";
                    return false;
                }

                // Might need to move this to renderer init
                Render::init();
            }

            if(flags & 0b00000001) {
                CL::init();
            }

            _static_state = STOPPED;

            return true;

        }

        void cleanup() {
            Texture::cleanup();
            Model::cleanup();
            Render::cleanup();
            CL::cleanup();

            glfwTerminate();

            Log::cleanupAll();

            _static_state = UNINITIALIZED;
        }

        void start(GameCycleFunc function) {
            if(_static_state == RUNNING) return;
            if(_static_state == UNINITIALIZED) return; // TODO: Throw Exception if not Initialized when Engine started
            _static_state = RUNNING;

            Util::DeltaTimeController dtc;
            while(_static_state == RUNNING) {

                // Get the Delta Time
                double delta_time = dtc.get();

                // Run the given cycle function
                function(delta_time);

                // Do Engine Specific Updates
                glfwPollEvents();
                Render::checkWindowCloseFlags();
                Render::CameraInternal::updateAll(delta_time);

            }
        }

        void stop() {
            _static_state = STOPPED;
        }


    }
}