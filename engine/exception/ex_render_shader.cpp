#include "api/Exception.h"

namespace Swarm {
    namespace Exception {

        RenderShaderException::RenderShaderException(Type type, const std::string &message)
                : runtime_error(message), _type(type) {};

        RenderShaderException RenderShaderException::FileLoad(const std::string &path) {
            return RenderShaderException(FILE_LOAD,
                                         "Failed to open file '" + path + "'");
        }

        RenderShaderException RenderShaderException::Compile(SWMuint ID, const std::string &err) {
            return RenderShaderException(COMPILE,
                                         "Failed to compile Shader with ID '" + std::to_string(ID) + "':\n" + err);
        }
    }
}