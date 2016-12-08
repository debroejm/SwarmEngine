#pragma once

#include "CL/cl.h"

#include <set>
#include <string>
#include <vector>

namespace Swarm {
    namespace CL {

        void init();
        void cleanup();

        struct DeviceInfo {
            cl_platform_id platform;
            cl_device_id device;
            cl_uint maxComputeUnits;
            std::string deviceName;
            DeviceInfo(cl_platform_id platform, cl_device_id device, cl_uint maxComputeUnits, std::string deviceName) {
                this->platform = platform;
                this->device = device;
                this->maxComputeUnits = maxComputeUnits;
                this->deviceName = deviceName;
            }
            operator cl_platform_id() const { return platform; }
            operator cl_device_id() const { return device; }
            friend bool operator<(const DeviceInfo &lhs, const DeviceInfo &rhs) { return lhs.maxComputeUnits < rhs.maxComputeUnits; }
            DeviceInfo &operator=(const DeviceInfo &other) {
                platform = other.platform;
                device = other.device;
                maxComputeUnits = other.maxComputeUnits;
                deviceName = other.deviceName;
                return *this;
            }
        };

        std::set<DeviceInfo> &getAllDevices();

        struct DeviceContextListing {
            void addDevice(const DeviceInfo &info);
            DeviceContextListing() : platform(nullptr) {}
            DeviceContextListing(const DeviceInfo &info);
        private:
            friend struct Context;
            friend struct Program;
            cl_platform_id platform;
            std::vector<cl_device_id> devices;
        };

        struct Context {
            cl_context context;
            operator cl_context() const { return context; }
            Context(DeviceInfo &info);
            Context(DeviceContextListing &listing);
            static void cleanup();
        private:
            friend struct Program;
            DeviceContextListing info;
            cl_context createContext(DeviceContextListing &listing);
        };

        struct Program {
            cl_program program;
            operator cl_program() const { return program; }
            Program(std::string src, const Context &ctx);
            static void cleanup();
        };

        struct Buffer {
            cl_mem buffer;
            operator cl_mem() const { return buffer; }
            Buffer(const Context &ctx, cl_mem_flags flags, size_t size, void* data);
            void release();
            static void cleanup();
        };

        struct Kernel {
            cl_kernel kernel;
            operator cl_kernel() const { return kernel; }
            Kernel(const Program &program, std::string name);
            void setArgument(cl_uint index, size_t size, const void* data);
            void setArgument(cl_uint index, const Buffer &buffer);
            static void cleanup();
        };

        struct CommandQueue {
            cl_command_queue queue;
            operator cl_command_queue() const { return queue; }
            CommandQueue(const Context &ctx, const DeviceInfo &device);
            // TODO: Error Checking on Enqueues
            // TODO: Event Handling on Enqueues
            void enqueueCommand(const Kernel &kernel, size_t size);
            void enqueueRead(const Buffer &buffer, bool blocking, size_t size, void* data);
            static void cleanup();
        };

    }
}