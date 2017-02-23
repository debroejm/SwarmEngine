#include "api/Exception.h"

namespace Swarm {
    namespace Exception {

        RenderProgramException::RenderProgramException(Type type, const std::string &message)
                : runtime_error(message), _type(type) {};

        RenderProgramException RenderProgramException::Link(SWMuint ID, const std::string &err) {
            return RenderProgramException(LINK,
                                          "Failed to link Program with ID '" + std::to_string(ID) + "':\n" + err);
        }
    }
}