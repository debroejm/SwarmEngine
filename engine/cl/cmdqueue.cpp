#include "CLInternal.h"

#include "api/Exception.h"
#include "api/Logging.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        std::set<CommandQueueInternal*> _static_registered_queues;

        void CommandQueueInternal::cleanup() {
            for(CommandQueueInternal* queue : _static_registered_queues) delete queue;
            _static_registered_queues.clear();
        }

        CommandQueue::CommandQueue(const Context* context, const Device* device) {
            _command_queue = new CommandQueueInternal((ContextInternal*)context, (DeviceInternal*)device);
            _static_registered_queues.insert(_command_queue);
        }

        void CommandQueue::enqueueCommand(const Kernel &kernel, size_t size) {
            const size_t gws[]{ size, 0, 0 };
            clEnqueueNDRangeKernel(_command_queue->_command_queue, kernel.kernel(), 1, nullptr, gws, nullptr, 0, nullptr, nullptr);
        }

        template<typename T> void CommandQueue::enqueueRead(const Buffer<T> &buffer, bool blocking) {
            clEnqueueReadBuffer(_command_queue->_command_queue, buffer.buffer(), blocking ? CL_TRUE : CL_FALSE, 0, buffer.size(), buffer.data(), 0, nullptr, nullptr);
        }

        cl_command_queue CommandQueue::queue() const {
            return _command_queue->_command_queue;
        }



        CommandQueueInternal::CommandQueueInternal(const ContextInternal* context, const DeviceInternal* device) {

            if(context == nullptr) throw Exception::CLObjectCreationException::CommandQueue(CL_INVALID_CONTEXT);
            if(device == nullptr) throw Exception::CLObjectCreationException::CommandQueue(CL_INVALID_DEVICE);

            cl_int err = 0;
            // TODO: Update to non-deprecated Command Queue creation method
            _command_queue = clCreateCommandQueue(context->context(), device->device(), 0, &err);

            // Check Error
            if(err != CL_SUCCESS) throw Exception::CLObjectCreationException::CommandQueue(err);
        }

        CommandQueueInternal::~CommandQueueInternal() {
            clReleaseCommandQueue(_command_queue);
        }
    }
}