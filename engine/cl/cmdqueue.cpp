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

#include "CLInternal.h"

#include "api/Exception.h"

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

        void CommandQueue::enqueueReadInternal(const BufferBase &buffer, bool blocking, size_t size, void* data) {
            clEnqueueReadBuffer(_command_queue->_command_queue, buffer.buffer(), blocking ? CL_TRUE : CL_FALSE, 0, size, data, 0, nullptr, nullptr);
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