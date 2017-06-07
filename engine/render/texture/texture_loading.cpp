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

#include "api/Exception.h"

#include <lodepng.h>

namespace Swarm {
    namespace Texture {

        std::set<GLuint> registered_textures;

        void cleanup() {
            for(GLuint tex : registered_textures) glDeleteTextures(1, &tex);
        }

        GLuint loadTexInternal_PNG(const std::string &path) {
            std::vector<unsigned char> image;
            unsigned width, height;
            unsigned error = lodepng::decode(image, width, height, path.c_str());

            if(error != 0) throw Exception::TextureException::FileLoadPNG(path, std::string(lodepng_error_text(error)));

            GLuint textureID;
            glGenTextures(1, &textureID);

            glBindTexture(GL_TEXTURE_2D, textureID);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
            glGenerateMipmap(GL_TEXTURE_2D); // Is this really needed?

            glBindTexture(GL_TEXTURE_2D, 0);

            return textureID;
        }

        TexMap loadTexFromFile(const std::string &path, FileType type) {
            GLuint texID = 0;
            switch(type) {
                case PNG: texID = loadTexInternal_PNG(path); break;
                default: texID = 0; break;
            }
            registered_textures.insert(texID);
            return TexMap(Type::TargetType::TEX_2D, texID);
        }
    }
}