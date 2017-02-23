#include "CLInternal.h"

#include "api/Exception.h"
#include "api/Logging.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        std::set<BufferInternalBase*> _static_registered_buffers;

        void BufferInternalBase::cleanup() {
            for(BufferInternalBase* buffer : _static_registered_buffers) delete buffer;
            _static_registered_buffers.clear();
        }

        template<typename T> Buffer<T>::Buffer(const Context* ctx, bool read, bool write, size_t size) {
            _buffer = new BufferInternal<T>((ContextInternal*)ctx, read, write, size);
            _static_registered_buffers.insert(&(*(BufferInternalBase*)_buffer));
        }

        template<typename T> Buffer<T>::Buffer(const Context* ctx, bool read, bool write, size_t size, T data[]) {
            _buffer = new BufferInternal<T>((ContextInternal*)ctx, read, write, size, data);
            _static_registered_buffers.insert(&(*(BufferInternalBase*)_buffer));
        }

        template<typename T> size_t Buffer<T>::size() const { return _buffer->_size; }

        template<typename T> void Buffer<T>::insert(size_t index, const T &value) {
            if(index >= _buffer->_size) throw std::out_of_range("Buffer::insert");
            _buffer->_data[index] = value;
        }

        template<typename T> const T &Buffer<T>::at(size_t index) const {
            if(index >= _buffer->_size) throw std::out_of_range("Buffer::at");
            return _buffer->_data[index];
        }

        template<typename T> T &Buffer<T>::operator[](size_t index) {
            if(index >= _buffer->_size) throw std::out_of_range("Buffer::operator[]");
            return _buffer->_data[index];
        }

        template<typename T> void Buffer<T>::resize(size_t size) {
            _buffer->resize(size);
        }

        template<typename T> T* Buffer<T>::data() const {
            return _buffer->_data;
        }

        template<typename T> cl_mem Buffer<T>::buffer() const {
            return _buffer->_buffer;
        }



        template<typename T> BufferInternal<T>::BufferInternal(const ContextInternal* ctx, bool read, bool write, size_t size) {

            if(ctx == nullptr) throw Exception::CLObjectCreationException::Buffer(CL_INVALID_CONTEXT);
            if(!(read || write)) throw Exception::CLObjectCreationException::Buffer(CL_INVALID_VALUE);

            _flags = CL_MEM_USE_HOST_PTR | read ? (write ? CL_MEM_READ_WRITE : CL_MEM_READ_ONLY) : CL_MEM_WRITE_ONLY;

            _size = size;
            _data = new T[_size];

            cl_int err = 0;
            _buffer = clCreateBuffer(ctx->context(), _flags, sizeof(T) * _size, _data, &err);

            // Check Error
            if(err != CL_SUCCESS) throw Exception::CLObjectCreationException::Buffer(err);

            _context = ctx;
        }

        template<typename T> BufferInternal<T>::BufferInternal(const ContextInternal* ctx, bool read, bool write, size_t size, T data[]) {

            if(ctx == nullptr) throw Exception::CLObjectCreationException::Buffer(CL_INVALID_CONTEXT);
            if(!(read || write)) throw Exception::CLObjectCreationException::Buffer(CL_INVALID_VALUE);

            _flags = CL_MEM_USE_HOST_PTR | read ? (write ? CL_MEM_READ_WRITE : CL_MEM_READ_ONLY) : CL_MEM_WRITE_ONLY;

            _size = size;
            _data = data;

            cl_int err = 0;
            _buffer = clCreateBuffer(ctx->context(), _flags, sizeof(T) * _size, _data, &err);

            // Check Error
            if(err != CL_SUCCESS) throw Exception::CLObjectCreationException::Buffer(err);

            _context = ctx;
        }

        template<typename T> BufferInternal<T>::~BufferInternal() {
            clReleaseMemObject(_buffer);
            delete [] _data;
            _data = nullptr;
            _size = 0;
            _context = nullptr;
        }

        template<typename T> void BufferInternal<T>::resize(size_t size) {

            T* newData = new T[size];
            for(size_t i = 0; i < _size && i < size; i++) newData[i] = _data[i];

            clReleaseMemObject(_buffer);
            delete [] _data;
            _data = newData;
            _size = size;

            cl_int err = 0;
            _buffer = clCreateBuffer(_context->context(), _flags, sizeof(T) * _size, _data, &err);

            // Check Error
            if(err != CL_SUCCESS) throw Exception::CLObjectCreationException::Buffer(err);
        }

    }
}