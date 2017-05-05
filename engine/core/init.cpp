#define SWARM_INCLUDE_GLFW
#include "api/Core.h"

#include "api/Logging.h"
#include "api/CLEngine.h"
#include "render/RenderInternal.h"
#include "physics/PhysicsInternal.h"
#include "vhe/VHEInternal.h"
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

            // Scan for Dependencies
            if(flags & SWM_INIT_PHYSICS) flags |= SWM_INIT_CL;

            // Render Init
            if(flags & SWM_INIT_RENDER) {

                // Initialize GLFW
                // TODO: Move GLFW Initialization to Render::init()
                Log::log_core(INFO) << "Initializing GLFW";
                glfwSetErrorCallback(callback_error);
                if (!glfwInit()) {
                    Log::log_core(FATAL) << "Failed to Initialize GLFW!";
                    return false;
                }

                Render::init();
            }

            // CL Init
            if(flags & SWM_INIT_CL) {
                CL::init();
            }

            // Physics Init
            /*
            if(flags & SWM_INIT_PHYSICS) {
                Physics::init();
            }
             */

            // VHE Init
            if(flags & SWM_INIT_VHE) {
                VHE::init();
            }

            _static_state = STOPPED;

            return true;

        }

        void cleanup() {
            Texture::cleanup();
            Model::cleanup();
            Render::cleanup();
            CL::cleanup();
            VHE::cleanup();

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