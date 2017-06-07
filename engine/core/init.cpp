/*
 * Copyright 2017 James De Broeck
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define SWARM_INCLUDE_GLFW
#include "api/Core.h"

#include "api/Logging.h"
#include "../cl/CLInternal.h"
#include "render/RenderInternal.h"
#include "physics/PhysicsInternal.h"
#include "vhe/VHEInternal.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Core {

        Log _static_log_core("Core");

        Log &log_core() { return _static_log_core; }
        Log &log_core(Logging::LogSeverity severity) { return _static_log_core(severity); }

        void callback_error(int error, const char* description)
        {
            log_core(ERR) <<  "GLFW ERROR '" << error << "': " << description << Flush();
        }

        State _static_state = UNINITIALIZED;
        State state() { return _static_state; }

        bool init(size_t flags) {

            log_core().initFile();

            // Scan for Dependencies
            if(flags & SWM_INIT_PHYSICS) flags |= SWM_INIT_CL;

            // Render Init
            if(flags & SWM_INIT_RENDER) {

                // Initialize GLFW
                // TODO: Move GLFW Initialization to Render::init()
                log_core(INFO) << "Initializing GLFW" << Flush();
                glfwSetErrorCallback(callback_error);
                if (!glfwInit()) {
                    log_core(FATAL) << "Failed to Initialize GLFW!" << Flush();
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
                log_core().flushAll();
                CL::log_cl().flushAll();
                VHE::log_vhe().flushAll();

            }
        }

        void stop() {
            _static_state = STOPPED;
        }


    }
}