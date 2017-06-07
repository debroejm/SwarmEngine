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

#define SWARM_INCLUDE_CL
#include "api/Exception.h"

namespace Swarm {
    namespace Exception {

        CLObjectCreationException::CLObjectCreationException(Type type, const std::string &message)
                : runtime_error(message), _type(type) {};

        CLObjectCreationException CLObjectCreationException::Buffer(int err_code) {
            switch(err_code) {
                case CL_INVALID_CONTEXT:
                    return CLObjectCreationException(BUFFER, "Attempted to create CL Buffer for an Invalid Context");
                case CL_INVALID_VALUE:
                    return CLObjectCreationException(BUFFER, "Attempted to create CL Buffer with Invalid Flags");
                case CL_INVALID_BUFFER_SIZE:
                    return CLObjectCreationException(BUFFER, "Attempted to create CL Buffer with size zero or too large for device");
                case CL_INVALID_HOST_PTR:
                    return CLObjectCreationException(BUFFER, "Attempted to create CL Buffer with Invalid host data pointer");
                case CL_MEM_OBJECT_ALLOCATION_FAILURE:
                    return CLObjectCreationException(BUFFER, "Failture to allocated memory for CL Buffer");
                case CL_OUT_OF_HOST_MEMORY:
                    return CLObjectCreationException(BUFFER, "Host out of Memory when Creating CL Buffer");
                default:
                    return CLObjectCreationException(BUFFER, "Unknown Error when Creating CL Buffer; '" + std::to_string(err_code) + "'");
            }
        }

        CLObjectCreationException CLObjectCreationException::Kernel(int err_code, const std::string &kernel_name) {
            switch(err_code) {
                case CL_INVALID_PROGRAM:
                    return CLObjectCreationException(KERNEL, "Attempted to attach CL Kernel to Invalid Program");
                case CL_INVALID_PROGRAM_EXECUTABLE:
                    return CLObjectCreationException(KERNEL, "Attempted to attach CL Kernel to Program with no executable");
                case CL_INVALID_KERNEL_NAME:
                    return CLObjectCreationException(KERNEL, "CL Kernel '" + kernel_name + "' not found in Program");
                case CL_INVALID_KERNEL_DEFINITION:
                    return CLObjectCreationException(KERNEL, "CL Kernel has differing definitions in more than one Program");
                case CL_INVALID_VALUE:
                    return CLObjectCreationException(KERNEL, "Attempted to create CL Kernel with NULL cstring name");
                case CL_OUT_OF_HOST_MEMORY:
                    return CLObjectCreationException(KERNEL, "Host out of Memory when Creating CL Kernel");
                default:
                    return CLObjectCreationException(KERNEL, "Unknown Error when Creating CL Kernel; '" + std::to_string(err_code) + "'");
            }
        }

        CLObjectCreationException CLObjectCreationException::Program(int err_code) {
            switch(err_code) {
                case CL_INVALID_CONTEXT:
                    return CLObjectCreationException(PROGRAM, "Attempted to create CL Program for an Invalid Context");
                case CL_INVALID_VALUE:
                    return CLObjectCreationException(PROGRAM, "Attempted to create CL Program with NULL cstring source");
                case CL_OUT_OF_HOST_MEMORY:
                    return CLObjectCreationException(PROGRAM, "Host out of Memory when Creating CL Program");
                default:
                    return CLObjectCreationException(PROGRAM, "Unknown Error when Creating CL Program; '" + std::to_string(err_code) + "'");
            }
        }

        CLObjectCreationException CLObjectCreationException::ProgramBuild(int err_code) {
            switch(err_code) {
                case CL_INVALID_PROGRAM:
                    return CLObjectCreationException(PROGRAM_BUILD, "Attempted to build Invalid CL Program");
                case CL_INVALID_VALUE:
                    return CLObjectCreationException(PROGRAM_BUILD, "Attempted to build CL Program with Invalid Value; no/null Devices, or failture to provide callback with user_data");
                case CL_INVALID_DEVICE:
                    return CLObjectCreationException(PROGRAM_BUILD, "Attempted to build CL Program for an unassociated Device");
                case CL_INVALID_BINARY:
                    return CLObjectCreationException(PROGRAM_BUILD, "Attempted to build CL Program without binary loaded (nothing to build)");
                case CL_INVALID_BUILD_OPTIONS:
                    return CLObjectCreationException(PROGRAM_BUILD, "Attempted to build CL Program with Invalid Build Options");
                case CL_COMPILER_NOT_AVAILABLE:
                    return CLObjectCreationException(PROGRAM_BUILD, "Attempted to build CL Program without available compiler");
                case CL_INVALID_OPERATION:
                    return CLObjectCreationException(PROGRAM_BUILD, "Attempted to build CL Program while previous Program is still building or with Kernels already attached to Program");
                case CL_OUT_OF_HOST_MEMORY:
                    return CLObjectCreationException(PROGRAM_BUILD, "Host out of Memory when Building CL Program");
                default:
                    return CLObjectCreationException(PROGRAM_BUILD, "Unknown Error when Building CL Program; '" + std::to_string(err_code) + "'");
            }
        }

        CLObjectCreationException CLObjectCreationException::CommandQueue(int err_code) {
            switch(err_code) {
                case CL_INVALID_CONTEXT:
                    return CLObjectCreationException(COMMAND_QUEUE, "Attempted to create CL Command Queue for an Invalid Context");
                case CL_INVALID_DEVICE:
                    return CLObjectCreationException(COMMAND_QUEUE, "Attempted to create CL Command Queue for an Invalid Device or Device not attached to given Context");
                case CL_INVALID_VALUE:
                    return CLObjectCreationException(COMMAND_QUEUE, "Attempted to create CL Command Queue with one or more Invalid Property Values");
                case CL_INVALID_QUEUE_PROPERTIES:
                    return CLObjectCreationException(COMMAND_QUEUE, "Attempted to create CL Command Queue with one or more Properties unsupported by given Device");
                case CL_OUT_OF_RESOURCES:
                    return CLObjectCreationException(COMMAND_QUEUE, "Device out of Resources when creating CL Command Queue");
                case CL_OUT_OF_HOST_MEMORY:
                    return CLObjectCreationException(COMMAND_QUEUE, "Host out of Memory when Creating CL Command Queue");
                default:
                    return CLObjectCreationException(COMMAND_QUEUE, "Unknown Error when Creating CL Kernel; '" + std::to_string(err_code) + "'");
            }
        }
    }
}