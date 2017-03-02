#pragma once



// ***************
//  STD Libraries
// ***************

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

        template<typename T> class Buffer;
        struct BufferInternal;
        class BufferBase {
        private:
            template<typename T> friend class Buffer;
            BufferBase(const Context* ctx, bool read, bool write);
            virtual ~BufferBase();
            void recreate(size_t size, void* data);
            BufferInternal* _buffer;
        public:
            #if defined(SWARM_INCLUDE_CL)
            cl_mem buffer() const;
            operator cl_mem() const { return buffer(); }
            #endif
        };

        //! API Representation of a CL Buffer
        /*!
         * The Buffer class is a pure-virtual API representation of an OpenCL Buffer. It cannot be instantiated or
         * extended from. A Buffer object can be created by using the static function Buffer::create(). A Buffer
         * created this way has a static size determined at creation time, and is typed with the type of value used to
         * initially create the Buffer. Buffer types must be able to default construct with an empty parameter set.
         * Buffer contents can be modified using with standard container functions, but the Buffer's size cannot change.
         * For more information on CL Buffers in general, see the
         * <a href="https://www.khronos.org/registry/cl/specs/opencl-1.1.pdf">OpenCL Specification</a>
         *
         * \sa Buffer::create()
         */
        template<typename T> class Buffer : public BufferBase {
        public:

            Buffer(const Context* ctx, bool read, bool write, size_t size)
                    : _size(size), BufferBase(ctx, read, write) {
                _data = new T[_size];
                recreate(sizeof(T) * _size, _data);
            }

            Buffer(const Context* ctx, bool read, bool write, size_t size, T data[])
                    : _size(size), _data(data), BufferBase(ctx, read, write) {
                recreate(sizeof(T) * _size, _data);
            }

            virtual ~Buffer() {
                if(_data != nullptr) delete [] _data;
            }

            size_t size() const { return _size; }

            void insert(size_t index, const T &value) {
                if(index >= _size) throw std::out_of_range("Buffer::insert");
                _data[index] = value;
            }

            const T &at(size_t index) const {
                if(index >= _size) throw std::out_of_range("Buffer::at");
                return _data[index];
            }

            T &operator[](size_t index) {
                if(index >= _size) throw std::out_of_range("Buffer::operator[]");
                return _data[index];
            }

            void resize(size_t size) {
                T* newData = new T[size];
                for(size_t i = 0; i < _size && i < size; i++) newData[i] = _data[i];
                _size = size;
                recreate(sizeof(T) * _size, newData);
                delete [] _data;
                _data = newData;
            }

            T* data() const { return _data; }

        protected:
            size_t _size = 0;
            T* _data = nullptr;
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