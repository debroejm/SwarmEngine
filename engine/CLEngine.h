#pragma once

#include "CL/cl.h"

#include <set>
#include <string>
#include <vector>

namespace Swarm {
    namespace CL {

        void init();
        void cleanup();

        /*
        struct DeviceInfo {
            cl_platform_id platform;
            operator cl_platform_id() const { return platform; }
            cl_device_id device;
            operator cl_device_id() const { return device; }
            cl_uint maxComputeUnits;
            std::string deviceName;
            friend bool operator<(const DeviceInfo &lhs, const DeviceInfo &rhs) { return lhs.maxComputeUnits < rhs.maxComputeUnits; }
            DeviceInfo &operator=(const DeviceInfo &other) {
                platform = other.platform;
                device = other.device;
                maxComputeUnits = other.maxComputeUnits;
                deviceName = other.deviceName;
                return *this;
            }
        };

        static std::set<DeviceInfo> all_device_info;

        struct DeviceContextListing {
            void addDevice(DeviceInfo &info);
            DeviceContextListing() : platform(nullptr) {}
            DeviceContextListing(DeviceInfo &info);
        private:
            friend struct Context;
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
            cl_context createContext(DeviceContextListing &listing);
        };
         */

    }
}