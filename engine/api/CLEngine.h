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

#pragma once



// ***************
//  STD Libraries
// ***************

#include <math.h>
#include <set>
#include <string>
#include <unordered_map>



// ***************
//  Common Header
// ***************

#include "common_header.h"



// ***********
//  API Begin
// ***********

namespace Swarm {
    namespace CL {

        void init();
        void cleanup();

        //! API Representation of a CL Platform
        /*!
         * The Platform class is a pure-virtual API representation of an OpenCL Platform. It cannot be instantiated or
         * extended from. An internal listing of Platform objects is populated when the CL module is initialized with
         * \ref init(), usually called from \ref Init::init(). This listing can be queried with \ref Platform::getAll().
         * For more information on CL Platforms in general, see the
         * <a href="https://www.khronos.org/registry/cl/specs/opencl-1.1.pdf">OpenCL Specification</a>
         *
         * \sa CL::init(), Init::init(), Platform::getAll()
         */
        class Platform {
        public:
            virtual const std::string &name() const = 0;
            virtual ~Platform() {}
            static const std::set<Platform*> getAll();
        private:
            Platform() {}
            friend class PlatformInternal;
        };

        //! API Representation of a CL Device
        /*!
         * The Device class is a pure-virtual API representation of an OpenCL Device. It cannot be instantiated or
         * extended from. An internal listing of Device objects is populated when the CL module is initialized with
         * \ref init(), usually called from \ref Init::init(). This listing can be queried with \ref Device::getAll().
         * Device objects also contain a reference to the Platform which provides their interface drivers.
         * For more information on CL Devices in general, see the
         * <a href="https://www.khronos.org/registry/cl/specs/opencl-1.1.pdf">OpenCL Specification</a>
         *
         * \sa CL::init(), Init::init(), Device::getAll(), Device::platform()
         */
        class Device {
        public:
            virtual const std::string &name() const = 0;
            virtual unsigned int computeUnits() const = 0;
            virtual const Platform* platform() const = 0;
            virtual ~Device() {}
            static std::set<Device*> getAll();
        private:
            Device() {}
            friend class DeviceInternal;
        };

        //! API Representation of a CL Context
        /*!
         * The Context class is a pure-virtual API representation of an OpenCL Context. It cannot be instantiated or
         * extended from. A Context object can be created by using the static function Context::create().
         * For more information on CL Contexts in general, see the
         * <a href="https://www.khronos.org/registry/cl/specs/opencl-1.1.pdf">OpenCL Specification</a>
         *
         * \sa Context::create()
         */
        class Context {
        public:
            virtual std::set<Device*> devices() const = 0;
            virtual ~Context() {}
            static Context* create(const Device* devices[], size_t count);
        private:
            Context() {}
            friend class ContextInternal;
        };

        //template<typename T> class BufferTemplateBase;
        struct BufferInternal;
        class BufferBase {
            //template<typename T> friend class BufferTemplateBase;
        protected:
            BufferInternal* _buffer;
            BufferBase(const Context* ctx, bool read, bool write);
            virtual ~BufferBase();
            void recreate(size_t size, void* data);
        public:
            #if defined(SWARM_INCLUDE_CL)
            cl_mem buffer() const;
            operator cl_mem() const { return buffer(); }
            #endif
        };

        template<typename T> class Buffer : public BufferBase {
        protected:
            size_t _size = 0;
            size_t _capacity = 0;
            T* _data = nullptr;

            virtual void checkBounds(size_t index) {
                if(index >= _size) throw std::out_of_range("Buffer::checkBounds");
            }

            virtual void checkBounds(size_t index) const {
                if(index >= _size) throw std::out_of_range("Buffer::checkBounds");
            }

            Buffer(const Context* ctx, bool read, bool write, size_t size, size_t capacity)
                    : _size(size), _capacity(capacity), BufferBase(ctx, read, write) {
                _data = new T[_capacity];
                recreate(sizeof(T) * _capacity, _data);
            }

            Buffer(const Context* ctx, bool read, bool write, size_t size, size_t capacity, T data[])
                    : _size(size), _capacity(capacity), _data(data), BufferBase(ctx, read, write) {
                recreate(sizeof(T) * _capacity, _data);
            }

        public:

            Buffer(const Context* ctx, bool read, bool write, size_t size)
                    : Buffer(ctx, read, write, size, size) {}

            Buffer(const Context* ctx, bool read, bool write, size_t size, T data[])
                    : Buffer(ctx, read, write, size, size, data) {}

            virtual ~Buffer() {
                if(_data != nullptr) delete [] _data;
            }

            size_t size() const { return _size; }
            size_t capacity() const { return _capacity; }

            void set(size_t index, const T &value) {
                checkBounds(index);
                _data[index] = value;
            }

            T &at(size_t index) {
                checkBounds(index);
                return _data[index];
            }

            const T &at(size_t index) const {
                checkBounds(index);
                return _data[index];
            }

            T &operator[](size_t index) {
                return _data[index];
            }

            const T &operator[](size_t index) const {
                return _data[index];
            }

            virtual void resize(size_t size) {
                T* newData = new T[size];
                for(size_t i = 0; i < _size && i < size; i++) newData[i] = _data[i];
                _size = size;
                _capacity = size;
                recreate(sizeof(T) * _capacity, newData);
                delete [] _data;
                _data = newData;
            }

            T* data() { return _data; }
            const T* data() const { return _data; }
        };

        template<typename T> class BufferVector : public Buffer<T> {
        protected:

            size_t calculateCapacity(size_t size) {
                float base = ceil(log2((float)size));
                return (size_t)pow(2.0f, base);
            }

        public:

            BufferVector(const Context* ctx, bool read, bool write, size_t initial_size)
                    : Buffer<T>(ctx, read, write, initial_size, calculateCapacity(initial_size)) {}

            BufferVector(const Context* ctx, bool read, bool write, size_t initial_size, T data[])
                    : Buffer<T>(ctx, read, write, initial_size, calculateCapacity(initial_size)) {
                // Copy initial data
                for(size_t i = 0; i < this->_size; i++) this->_data[i] = data[i];
            }

            virtual void resize(size_t size) {
                size_t cap = calculateCapacity(size);
                if(cap > this->_capacity) Buffer<T>::resize(cap);
                else this->_size = size;
            }

            void push_back(const T &value) {
                size_t endIndex = this->_size;
                resize(this->_size+1);
                this->_data[endIndex] = value;
            }

            void pop_back() {
                if(this->_size > 0) resize(this->_size-1);
            }

            T &front() { return this->_data[0]; }
            const T &front() const { return this->_data[0]; }

            T &back() { return this->_data[this->_size-1]; }
            const T &back() const { return this->_data[this->_size-1]; }

            void reserve(size_t capacity) {
                size_t cap = calculateCapacity(capacity);
                if(cap > this->_capacity) {
                    size_t size = this->_size;
                    Buffer<T>::resize(cap);
                    this->_size = size;
                }
            }
        };

        struct KernelInternal;
        class Program;

        class Kernel {
        public:
            Kernel() : _kernel(nullptr) {}

            template<typename T> void argument(unsigned int index, const Buffer<T> &buffer) {
                argumentInternalBuffer(index, buffer);
            }

            template<typename T> void argument(unsigned int index, size_t size, const T* data) {
                argumentInternal(index, sizeof(T), data);
            }

            #if defined(SWARM_INCLUDE_CL)
            cl_kernel kernel() const;
            operator cl_kernel() const { return kernel(); }
            #endif

        protected:
            friend struct KernelInternal;
            KernelInternal* _kernel;

            void argumentInternalBuffer(unsigned int index, const BufferBase &buffer);
            void argumentInternal(unsigned int index, size_t size, const void* data);

        private:
            friend class Program;
            Kernel(const Program &program, const std::string &name);
        };

        struct ProgramInternal;

        //! API Representation of a CL Program
        /*!
         * The Program class is a pure-virtual API representation of an OpenCL Program. It cannot be instantiated or
         * extended from. A Program object can be created by using the static function Program::compile().
         * For more information on CL Programs in general, see the
         * <a href="https://www.khronos.org/registry/cl/specs/opencl-1.1.pdf">OpenCL Specification</a>
         *
         * \sa Program::compile()
         */
        class Program {
        public:
            Program(const std::string &src, const Context* ctx);

            Kernel kernel(const std::string &name);

            #if defined(SWARM_INCLUDE_CL)
            cl_program program() const;
            operator cl_program() const { return program(); }
            #endif

        protected:
            friend struct ProgramInternal;
            ProgramInternal* _program;
        };

        struct CommandQueueInternal;

        class CommandQueue {
        public:
            CommandQueue(const Context* context, const Device* device);

            // TODO: Error Checking and Event Handling on CL Enqueues
            void enqueueCommand(const Kernel &kernel, size_t size);

            template<typename T> void enqueueRead(const Buffer<T> &buffer, bool blocking) {
                enqueueReadInternal(buffer, blocking, sizeof(T) * buffer.size(), buffer.data());
            }

            #if defined(SWARM_INCLUDE_CL)
            cl_command_queue queue() const;
            operator cl_command_queue() const { return queue(); }
            #endif

        protected:
            friend struct CommandQueueInternal;
            CommandQueueInternal* _command_queue;

            void enqueueReadInternal(const BufferBase &buffer, bool blocking, size_t size, void* data);
        };


    }
}