#include "CLInternal.h"

#include "api/Exception.h"

namespace Swarm {
    namespace CL {

        BufferInternal::BufferInternal(const ContextInternal* ctx, bool read, bool write) {
            if(ctx == nullptr) throw Exception::CLObjectCreationException::Buffer(CL_INVALID_CONTEXT);
            if(!(read || write)) throw Exception::CLObjectCreationException::Buffer(CL_INVALID_VALUE);

            _flags = CL_MEM_USE_HOST_PTR | (cl_mem_flags)(read ? (write ? CL_MEM_READ_WRITE : CL_MEM_READ_ONLY) : CL_MEM_WRITE_ONLY);
            _context = ctx;
        }

        BufferInternal::~BufferInternal() {
            if(_buffer != nullptr) clReleaseMemObject(_buffer);
        }

        void BufferInternal::recreateBuffer(size_t size_in_bytes, void* data) {
            if(_buffer != nullptr) clReleaseMemObject(_buffer);

            cl_int err = CL_SUCCESS;
            _buffer = clCreateBuffer(_context->context(), _flags, size_in_bytes, data, &err);

            // Check Error
            if(err != CL_SUCCESS) throw Exception::CLObjectCreationException::Buffer(err);
        }

        BufferBase::BufferBase(const Context* ctx, bool read, bool write) {
            _buffer = new BufferInternal((ContextInternal*)ctx, read, write);
        }

        BufferBase::~BufferBase() {
            delete _buffer;
        }

        void BufferBase::recreate(size_t size, void* data) {
            _buffer->recreateBuffer(size, data);
        }

        cl_mem BufferBase::buffer() const {
            return _buffer->_buffer;
        }

    }
}