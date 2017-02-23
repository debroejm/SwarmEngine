#define SWARM_INCLUDE_GLEW
#include "api/Render.h"

namespace Swarm {
    namespace Render {

        ShaderType ShaderType::VERTEX       (GL_VERTEX_SHADER,      "Vertex");
        ShaderType ShaderType::GEOMETRY     (GL_GEOMETRY_SHADER,    "Geometry");
        ShaderType ShaderType::FRAGMENT     (GL_FRAGMENT_SHADER,    "Fragment");

        ShaderType &ShaderType::operator=(const ShaderType &rhs) {
            _type = rhs._type;
            _name = rhs._name;
            return *this;
        }

    }
}