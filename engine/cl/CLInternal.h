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

#define SWARM_INCLUDE_CL
#include "api/CLEngine.h"

#include "api/Logging.h"


#include <map>



// ************
//  Code Begin
// ************

namespace Swarm {
    namespace CL {

        Logging::Log &log_cl();
        Logging::Log &log_cl(Logging::LogSeverity severity);

        class PlatformInternal : public Platform {
        public:
            PlatformInternal(cl_platform_id platform, const std::string &name) : _platform(platform), _name(name) {}
            virtual const std::string &name() const { return _name; }
            cl_platform_id platform() const { return _platform; }
            operator cl_platform_id() const { return _platform; }
        protected:
            std::string _name;
            cl_platform_id _platform;
        };

        class DeviceInternal : public Device {
        public:
            DeviceInternal(PlatformInternal &platform, cl_device_id device, const std::string &name, unsigned int compute_units)
                    : _platform(platform), _device(device), _name(name), _compute_units(compute_units) {}
            virtual const Platform* platform() const { return &_platform; }
            cl_device_id device() const { return _device; }
            operator cl_device_id() const { return _device; }
            virtual const std::string &name() const { return _name; }
            virtual unsigned int computeUnits() const { return _compute_units; }
        protected:
            PlatformInternal &_platform;
            cl_device_id _device;
            std::string _name;
            unsigned int _compute_units;
            friend class ContextInternal;
        };

        class ContextInternal : public Context {
        public:
            ContextInternal(const DeviceInternal* devices[], size_t count);
            virtual ~ContextInternal();
            virtual std::set<Device*> devices() const { return _devices; }
            cl_context context() const { return _context; }
            operator cl_context() const { return _context; }
            static void cleanup();
        protected:
            cl_context _context = nullptr;
            std::set<Device*> _devices;
        };

        struct ProgramInternal {

            ProgramInternal(const std::string &src, const ContextInternal* ctx);
            virtual ~ProgramInternal();

            cl_program _program;
            std::map<std::string, Kernel> _kernel_map;

            static void cleanup();
        };

        struct BufferInternal {
            cl_mem _buffer = nullptr;
            const ContextInternal* _context = nullptr;
            cl_mem_flags _flags = 0;

            BufferInternal(const ContextInternal* ctx, bool read, bool write);
            virtual ~BufferInternal();
            void recreateBuffer(size_t size_in_bytes, void* data);
        };

        struct KernelInternal {

            KernelInternal(const Program &program, const std::string &name);
            virtual ~KernelInternal();

            cl_kernel _kernel;

            static void cleanup();
        };

        struct CommandQueueInternal {

            CommandQueueInternal(const ContextInternal* context, const DeviceInternal* device);
            virtual ~CommandQueueInternal();

            cl_command_queue _command_queue;

            static void cleanup();
        };
    }
}