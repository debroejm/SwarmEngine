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

                Log::log_vhe(INFO) << "Compiling VHE Program of size " << program_size << " bytes:\n";

                // Compile the commands into the byte array
                size_t index = 0;
                for(CompilerCommand* cmd : cmds) {
                    Log::log_vhe << "  " << cmd->to_string() << "\n";
                    cmd->compile(program_data, index);
                    index += cmd->size();
                }

                return Environment::Program(program_size, program_data, required_memory_size);
            }

        }
    }
}