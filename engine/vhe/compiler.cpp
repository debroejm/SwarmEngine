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

#include "Compiler.h"

#include "api/Logging.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace VHE {
        namespace Compiler {

            Environment::Program compileCommandList(const CCList &cmds, size_t required_memory_size) {

                // Calculate the size in bytes of the program
                size_t program_size = 0;
                for(CompilerCommand* cmd : cmds)
                    program_size += cmd->size();

                // Create the program byte array
                vbyte* program_data = new vbyte[program_size];

                log_vhe(INFO) << "Compiling VHE Program of size " << program_size << " bytes" << Flush();

                // Compile the commands into the byte array
                size_t index = 0;
                for(CompilerCommand* cmd : cmds) {
                    log_vhe(DEBUG) << "  " << cmd->to_string();
                    cmd->compile(program_data, index);
                    index += cmd->size();
                }
                log_vhe().flush();

                return Environment::Program(program_size, program_data, required_memory_size);
            }

        }
    }
}