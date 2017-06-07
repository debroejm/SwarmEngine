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