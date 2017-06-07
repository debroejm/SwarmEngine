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

        std::set<ProgramInternal*> registered_programs;

        void ProgramInternal::cleanup() {
            for(ProgramInternal* program : registered_programs) {
                Render::log_render(DEBUG) << "Deleting Program with ID '" << program->ID() << "'";
                delete program;
            }
            Render::log_render().flush();
        }

        ProgramInternal::~ProgramInternal() {
            glDeleteProgram(_ID);
        }

        Program* Program::compile(Shader* shaders[], size_t count) {
            ProgramInternal* program = new ProgramInternal(shaders, count);
            registered_programs.insert(program);
            return program;
        }

        ProgramInternal::ProgramInternal(Shader* shaders[], size_t count) {

            // Create ID
            _ID = glCreateProgram();
            Render::log_render(DEBUG) << "Linking Program with ID '" << _ID << "'" << Flush();

            // Attach Shaders
            for(size_t i = 0; i < count; i++) {
                if(shaders[i] != nullptr) {
                    glAttachShader(_ID, shaders[i]->ID());
                    Render::log_render(DEBUG) << "Attaching " << shaders[i]->type().name() << " Shader with ID '" << shaders[i]->ID() << "'";
                }
            }
            Render::log_render().flush();

            // Link Program
            glLinkProgram(_ID);

            // Check Link Status
            GLint result = GL_FALSE;
            GLint log_length = 0;
            glGetProgramiv(_ID, GL_LINK_STATUS,     &result);
            glGetProgramiv(_ID, GL_INFO_LOG_LENGTH, &log_length);
            char log_contents[log_length+1];
            glGetProgramInfoLog(_ID, log_length, nullptr, &log_contents[0]);
            if(!result) throw Exception::RenderProgramException::Link(_ID, std::string(&log_contents[0]));

            glUseProgram(0);

            Render::log_render(DEBUG) << "Successfully Linked Program with ID '" << _ID << "'" << Flush();
        }

        SWMint ProgramInternal::uniform(const std::string &name) {
            if(_uniform_cache.count(name) < 1) {
                GLint unif = glGetUniformLocation(_ID, (const GLchar*)name.c_str());
                _uniform_cache[name] = unif;

            }
            return _uniform_cache[name];
        }
    }
}