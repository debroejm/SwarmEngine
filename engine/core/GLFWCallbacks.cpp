#include "GLFWCallbacks.h"

#include "CoreInternal.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace GLFW {

        void error_callback(int error, const char* description)
        {
            Core::log_core(ERR) << " " << error << ": " << description << Flush();
        }

    }
}