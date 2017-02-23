#include "CLInternal.h"

#include "api/Exception.h"
#include "api/Logging.h"

#include <map>

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        std::set<ProgramInternal*> _static_registered_programs;

        void ProgramInternal::cleanup() {
            for(ProgramInternal* program : _static_registered_programs) delete program;
            _static_registered_programs.clear();
        }

        Program::Program(const std::string &src, const Context* ctx) {
            _program = new ProgramInternal(src, (ContextInternal*)ctx);
            _static_registered_programs.insert(_program);
        }

        Kernel Program::kernel(const std::string &name) {
            if(_program->_kernel_map.count(name)) return _program->_kernel_map.at(name);
            else {
                _program->_kernel_map[name] = Kernel(*this, name);
                return _program->_kernel_map[name];
            }
        }

        cl_program Program::program() const {
            return _program->_program;
        }



        ProgramInternal::ProgramInternal(const std::string &src, const ContextInternal* ctx) {

            if(ctx == nullptr) throw Exception::CLObjectCreationException::Program(CL_INVALID_CONTEXT);

            size_t lengths[1]{ src.size() };
            const char* srcs[1]{ src.data() };

            // Create Program
            cl_int err = 0;
            _program = clCreateProgramWithSource(ctx->context(), 1, srcs, lengths, &err);

            // Check Error
            if(err != CL_SUCCESS) throw Exception::CLObjectCreationException::Program(err);

            // Build Program
            std::set<Device*> devices = ctx->devices();
            cl_device_id* device_data = new cl_device_id[devices.size()];
            size_t i = 0;
            for(Device* dvc : devices) device_data[i++] = ((DeviceInternal*)dvc)->device();
            err = clBuildProgram(_program, (cl_uint)devices.size(), device_data, nullptr, nullptr, nullptr);

            // Check Error
            if(err != CL_SUCCESS) throw Exception::CLObjectCreationException::ProgramBuild(err);
        }

        ProgramInternal::~ProgramInternal() {
            if(_program != nullptr) clReleaseProgram(_program);
        }





        std::set<KernelInternal*> _static_registered_kernels;

        void KernelInternal::cleanup() {
            for(KernelInternal* kernel : _static_registered_kernels) delete kernel;
            _static_registered_kernels.clear();
        }

        Kernel::Kernel(const Program &program, const std::string &name) {
            _kernel = new KernelInternal(program, name);
            _static_registered_kernels.insert(_kernel);
        }

        template<typename T> void Kernel::argument(unsigned int index, const Buffer<T> &buffer) {
            if(_kernel == nullptr) return;
            clSetKernelArg(_kernel->_kernel, index, sizeof(cl_mem), buffer.buffer());
        }

        template<typename T> void Kernel::argument(unsigned int index, size_t size, T* data) {
            if(_kernel == nullptr) return;
            clSetKernelArg(_kernel->_kernel, index, sizeof(T)*size, data);
        }

        cl_kernel Kernel::kernel() const {
            if(_kernel == nullptr) return nullptr;
            else return _kernel->_kernel;
        }



        KernelInternal::KernelInternal(const Program &program, const std::string &name) {

            //if(program == nullptr) throw Exception::CLObjectCreationException::Kernel(CL_INVALID_PROGRAM, name);

            cl_int err = 0;
            _kernel = clCreateKernel(program.program(), name.c_str(), &err);

            // Error Checking
            if(err != CL_SUCCESS) throw Exception::CLObjectCreationException::Kernel(err, name);
        }

        KernelInternal::~KernelInternal() {
            if(_kernel != nullptr) clReleaseKernel(_kernel);
        }
    }
}