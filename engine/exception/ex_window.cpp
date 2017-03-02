#include "api/Exception.h"

namespace Swarm {
    namespace Exception {

        WindowException::WindowException(Type type, const std::string &message)
                : runtime_error(message), _type(type) {};

        WindowException WindowException::GlewInit(const std::string &window_name, const std::string &err) {
            return WindowException(GLEW_INIT,
                                    "Failed to initialize GLEW for Window '"+ window_name +"':\n" + err);
        }

    }
}