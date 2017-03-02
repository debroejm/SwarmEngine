#define SWARM_INCLUDE_GLFW
#include "api/Util.h"



namespace Swarm {
    namespace Util {

        DeltaTimeController::DeltaTimeController() {
            _last_time = glfwGetTime();
        }

        double DeltaTimeController::get() {
            double time = glfwGetTime();
            double delta = time - _last_time;
            _last_time = time;
            return delta;
        }
    }
}