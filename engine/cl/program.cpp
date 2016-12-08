#include "../CLEngine.h"
#include "../Core.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        std::vector<cl_program> registered_programs;
        std::unordered_map<cl_program, std::unordered_map<std::string, cl_kernel>> registered_kernels;

        Program::Program(std::string src, const Context &ctx) {
            size_t lengths[1]{ src.size() };
            const char* srcs[1]{ src.data() };

            cl_int err = 0;
            program = clCreateProgramWithSource(ctx, 1, srcs, lengths, &err);

            // Check Error
            if(err != CL_SUCCESS) {
                switch(err) {
                    case CL_INVALID_CONTEXT:
                        Log::log_cl(ERR) << "Invalid Context to create CL Program with";
                        break;
                    case CL_INVALID_VALUE:
                        Log::log_cl(ERR) << "Missing Data to create CL Program with";
                        break;
                    case CL_OUT_OF_HOST_MEMORY:
                        Log::log_cl(ERR) << "Host out of Memory when creating CL Program";
                        break;
                    default:
                        Log::log_cl(ERR) << "Unknown Error when creating CL Program; '" << err << "'";
                        break;
                }
                program = nullptr;
                return;
            }

            err = clBuildProgram(program, (cl_uint)ctx.info.devices.size(), ctx.info.devices.data(), nullptr, nullptr, nullptr);

            // Check Error
            if(err != CL_SUCCESS) {
                switch(err) {
                    case CL_INVALID_PROGRAM:
                        Log::log_cl(ERR) << "Invalid CL Program to build";
                        break;
                    case CL_INVALID_VALUE:
                        Log::log_cl(ERR) << "Invalid Value when building CL Program; no/null Devices, or failure to provide callback with user_data";
                        break;
                    case CL_INVALID_DEVICE:
                        Log::log_cl(ERR) << "Device not associated with CL Program trying to build";
                        break;
                    case CL_INVALID_BINARY:
                        Log::log_cl(ERR) << "No Binary loaded for Devices associated with CL Program trying to build";
                        break;
                    case CL_INVALID_BUILD_OPTIONS:
                        Log::log_cl(ERR) << "Invalid specified build options when building CL Program";
                        break;
                    case CL_COMPILER_NOT_AVAILABLE:
                        Log::log_cl(ERR) << "Compiler not available to build CL Program with";
                        break;
                    case CL_BUILD_PROGRAM_FAILURE:
                        Log::log_cl(ERR) << "CL Program failed to build";
                        break;
                    case CL_INVALID_OPERATION:
                        Log::log_cl(ERR) << "Invalid Operation when building CL Program; previous Program still building or Kernels already attached to Program";
                        break;
                    case CL_OUT_OF_HOST_MEMORY:
                        Log::log_cl(ERR) << "Host out of Memory when building CL Program";
                        break;
                    default:
                        Log::log_cl(ERR) << "Unknown Error when building CL Program; '" << err << "'";
                        break;
                }
                program = nullptr;
                return;
            }

            registered_programs.push_back(program);
        }

        void Program::cleanup() {
            for(int i = 0; i < registered_programs.size(); i++) clReleaseProgram(registered_programs[i]);
            registered_programs.clear();
        }

        Kernel::Kernel(const Program &program, std::string name) {
            if(registered_kernels.count(program.program) > 1 && registered_kernels[program.program].count(name) > 1) {
                kernel = registered_kernels[program.program][name];
            }  else {
                cl_int err = 0;
                kernel = clCreateKernel(program, name.c_str(), &err);

                // Error Checking
                if(err != CL_SUCCESS) {
                    switch(err) {
                        case CL_INVALID_PROGRAM:
                            Log::log_cl(ERR) << "Invalid Program to attach CL Kernel to";
                            break;
                        case CL_INVALID_PROGRAM_EXECUTABLE:
                            Log::log_cl(ERR) << "Attempting to attach CL Kernel to Program with no executable";
                            break;
                        case CL_INVALID_KERNEL_NAME:
                            Log::log_cl(ERR) << "CL Kernel '" << name << "' not found in Program";
                            break;
                        case CL_INVALID_KERNEL_DEFINITION:
                            Log::log_cl(ERR) << "CL Kernel has a definition mismatch";
                            break;
                        case CL_INVALID_VALUE:
                            Log::log_cl(ERR) << "Trying to create CL Kernel with null name";
                            break;
                        case CL_OUT_OF_HOST_MEMORY:
                            Log::log_cl(ERR) << "Host out of Memory when creating CL Kernel";
                            break;
                        default:
                            Log::log_cl(ERR) << "Unknown Error when creating CL Kernel; '" << err << "'";
                            break;
                    }
                    kernel = nullptr;
                    return;
                }

                registered_kernels[program.program][name] = kernel;
            }
        }

        void Kernel::setArgument(cl_uint index, size_t size, const void* data) {
            clSetKernelArg(kernel, index, size, data);
        }

        void Kernel::setArgument(cl_uint index, const Buffer &buffer) {
            clSetKernelArg(kernel, index, sizeof(cl_mem), &buffer.buffer);
        }

        void Kernel::cleanup() {
            for(auto && iter_prog : registered_kernels) {
                for(auto && iter_name : iter_prog.second) {
                    clReleaseKernel(iter_name.second);
                }
            }
            registered_kernels.clear();
        }
    }
}