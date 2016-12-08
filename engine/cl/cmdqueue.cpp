#include "../CLEngine.h"
#include "../Core.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        std::vector<cl_command_queue> registered_queues;

        CommandQueue::CommandQueue(const Context &ctx, const DeviceInfo &device) {
            cl_int err = 0;
            queue = clCreateCommandQueue(ctx, device, 0, &err);

            // Check Error
            if(err != CL_SUCCESS) {
                switch(err) {
                    case CL_INVALID_CONTEXT:
                        Log::log_cl(ERR) << "Invalid Context to create CL Command Queue with";
                        break;
                    case CL_INVALID_DEVICE:
                        Log::log_cl(ERR) << "Attepted to create CL Command Queue with Invalid Device or Device not attached to given Context";
                        break;
                    case CL_INVALID_VALUE:
                        Log::log_cl(ERR) << "One or more Invalid Values in Properties used to create CL Command Queue";
                        break;
                    case CL_INVALID_QUEUE_PROPERTIES:
                        Log::log_cl(ERR) << "One or more Properties used to create CL Command Queue are not supported by the given Device";
                        break;
                    case CL_OUT_OF_RESOURCES:
                        Log::log_cl(ERR) << "Device out of Resources when creating CL Command Queue";
                        break;
                    case CL_OUT_OF_HOST_MEMORY:
                        Log::log_cl(ERR) << "Host out of Memory when creating CL Command Queue";
                        break;
                    default:
                        Log::log_cl(ERR) << "Unknown Error when creating CL Command Queue; '" << err << "'";
                        break;
                }
                queue = nullptr;
                return;
            }

            registered_queues.push_back(queue);
        }

        void CommandQueue::enqueueCommand(const Kernel &kernel, size_t size) {
            const size_t gws[]{ size, 0, 0 };
            clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, gws, nullptr, 0, nullptr, nullptr);
        }

        void CommandQueue::enqueueRead(const Buffer &buffer, bool blocking, size_t size, void* data) {
            clEnqueueReadBuffer(queue, buffer, blocking ? CL_TRUE : CL_FALSE, 0, size, data, 0, nullptr, nullptr);
        }

        void CommandQueue::cleanup() {
            for(int i = 0; i < registered_queues.size(); i++) clReleaseCommandQueue(registered_queues[i]);
            registered_queues.clear();
        }
    }
}