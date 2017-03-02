#define SWARM_INCLUDE_GLEW
#include "api/Render.h"

namespace Swarm {
    namespace Texture {
        namespace Type {

            TargetType TargetType::TEX_1D                   (GL_TEXTURE_1D);
            TargetType TargetType::TEX_1D_ARRAY             (GL_TEXTURE_1D_ARRAY);
            TargetType TargetType::TEX_2D                   (GL_TEXTURE_2D);
            TargetType TargetType::TEX_2D_ARRAY             (GL_TEXTURE_2D_ARRAY);
            TargetType TargetType::TEX_3D                   (GL_TEXTURE_3D);
            TargetType TargetType::TEX_RECTANGLE            (GL_TEXTURE_RECTANGLE);
            TargetType TargetType::TEX_CUBE_MAP             (GL_TEXTURE_CUBE_MAP);
            TargetType TargetType::TEX_CUBE_MAP_ARRAY       (GL_TEXTURE_CUBE_MAP_ARRAY);
            TargetType TargetType::TEX_BUFFER               (GL_TEXTURE_BUFFER);
            TargetType TargetType::TEX_2D_MULTISAMPLE       (GL_TEXTURE_2D_MULTISAMPLE);
            TargetType TargetType::TEX_2D_MULTISAMPLE_ARRAY (GL_TEXTURE_2D_MULTISAMPLE_ARRAY);

            MapType &MapType::operator=(const MapType &other) {
                _active = other._active;
                return *this;
            }
        }
    }
}