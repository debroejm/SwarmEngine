#include "GLFWCallbacks.h"

#include "api/Core.h"
#include "api/Logging.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace GLFW {

        void error_callback(int error, const char* description)
        {
            Log::log_core(ERR) << " " << error << ": " << description;
        }

    }
}