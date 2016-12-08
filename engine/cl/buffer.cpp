#include "../CLEngine.h"
#include "../Core.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        std::vector<cl_mem> registered_buffers;

        Buffer::Buffer(const Context &ctx, cl_mem_flags flags, size_t size, void* data) {
            cl_int err = 0;
            buffer = clCreateBuffer(ctx, flags, size, data, &err);

            // Check Error
            if(err != CL_SUCCESS) {
                switch(err) {

                }
                buffer = nullptr;
                return;
            }

            registered_buffers.push_back(buffer);
        }

        void Buffer::release() {
            for(unsigned long long i = registered_buffers.size()-1; i >= 0; i--) {
                if(registered_buffers[i] == buffer) registered_buffers.erase(registered_buffers.begin()+i);
            }
            clReleaseMemObject(buffer);
        }

        void Buffer::cleanup() {
            for(int i = 0; i < registered_buffers.size(); i++) clReleaseMemObject(registered_buffers[i]);
            registered_buffers.clear();
        }

    }
}