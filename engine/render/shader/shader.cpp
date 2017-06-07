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

#include "render/RenderInternal.h"

#include "api/Exception.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace Render {

        std::set<ShaderInternal*> registered_shaders;

        void ShaderInternal::cleanup() {
            for(ShaderInternal* shader : registered_shaders) delete shader;
        }

        ShaderInternal::~ShaderInternal() {
            glDeleteShader(_ID);
        }

        Shader* Shader::compileFromFile(const std::string &path, const ShaderType &type) {

            // Create File Input Stream
            Render::log_render(DEBUG) << "Loading " << type.name() << " Shader From File: " << path << Flush();
            std::string src("");
            std::ifstream src_input(path, std::ios::in);

            // Read File Input Stream
            if(src_input.is_open()) {
                std::string line("");
                while( getline(src_input, line) ) src += (line + "\n");
                src_input.close();
            } else throw Exception::RenderShaderException::FileLoad(path);

            return compileFromSource(src, type);
        }

        Shader* Shader::compileFromSource(const std::string &src, const ShaderType &type) {
            ShaderInternal* shader = new ShaderInternal(src, type);
            registered_shaders.insert(shader);
            return shader;
        }

        ShaderInternal::ShaderInternal(const std::string &src, const ShaderType &type)
                : _type(type) {

            // Create ID
            _ID = glCreateShader(type.type());
            Render::log_render(DEBUG) << "Compiling " << type.name() << " Shader with ID '" << _ID << "'" << Flush();

            // Compile Shader
            const GLchar *source = (const GLchar*)src.c_str();
            glShaderSource(_ID, 1, &source, nullptr);
            glCompileShader(_ID);

            // Check Compile Status
            GLint result = GL_FALSE;
            GLint log_length = 0;
            glGetShaderiv(_ID, GL_COMPILE_STATUS,  &result);
            glGetShaderiv(_ID, GL_INFO_LOG_LENGTH, &log_length);
            char log_contents[log_length+1];
            glGetShaderInfoLog(_ID, log_length, nullptr, &log_contents[0]);
            if(!result) throw Exception::RenderShaderException::Compile(_ID, std::string(&log_contents[0]));

            Render::log_render(DEBUG) << "Successfully Compiled " << type.name() << " Shader with ID '" << _ID << "'" << Flush();
        }
    }
}