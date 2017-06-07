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

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        Log _static_log_cl("CL");

        Log &log_cl() { return _static_log_cl; }
        Log &log_cl(Logging::LogSeverity severity) { return _static_log_cl(severity); }

        std::set<Platform*> all_platforms;
        std::set<Device*> all_devices;

        const std::set<Platform*> Platform::getAll() { return all_platforms; }
        std::set<Device*> Device::getAll() { return all_devices; }

        void init() {

            log_cl().initFile();

            // Get a list of platforms on the current system
            cl_uint platformIDCount = 0;
            clGetPlatformIDs(0, nullptr, &platformIDCount);
            if(platformIDCount < 1) {
                log_cl(ERR) << "No OpenCL Platforms were detected; OpenCL cannot initialize." << Flush();
                return;
            }
            std::vector<cl_platform_id> platformIDs(platformIDCount);
            clGetPlatformIDs(platformIDCount, platformIDs.data(), nullptr);

            // Iterate through found platforms
            for(cl_platform_id pid : platformIDs) {

                // Get the Platform Name
                char pname[256];
                clGetPlatformInfo(pid, CL_PLATFORM_NAME, 256, &pname[0], nullptr);

                // Create new Platform object and register it
                PlatformInternal* platform = new PlatformInternal(pid, std::string(pname));
                all_platforms.insert(&(*platform));

                // Get a list of devices for the current platform
                cl_uint deviceIDCount = 0;
                clGetDeviceIDs(pid, CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceIDCount);
                if(deviceIDCount < 1) {
                    log_cl(INFO) << "No Devices Found for Platform '" << pname << "'" << Flush();
                    break;
                } else log_cl(INFO) << deviceIDCount << " Devices Found for Platform '" << pname << "'" << Flush();
                std::vector<cl_device_id> deviceIDs(deviceIDCount);
                clGetDeviceIDs(pid, CL_DEVICE_TYPE_ALL, deviceIDCount, deviceIDs.data(), nullptr);

                // Iterate through found devices
                for(cl_device_id did : deviceIDs) {

                    // Is this device actually available?
                    cl_bool available;
                    clGetDeviceInfo(did, CL_DEVICE_AVAILABLE, 4, &available, nullptr);
                    if(!available) continue;

                    // Get Device Name
                    char dname[256];
                    clGetDeviceInfo(did, CL_DEVICE_NAME, 256, &dname[0], nullptr);

                    // Get Max Compute Units
                    cl_uint computeUnits = 0;
                    clGetDeviceInfo(did, CL_DEVICE_MAX_COMPUTE_UNITS, 4, &computeUnits, nullptr);

                    // Create a new Device object and register it
                    DeviceInternal* device = new DeviceInternal(*platform, did, std::string(dname), computeUnits);
                    all_devices.insert(&(*device));
                }
            }
        }

        void cleanup() {
            CommandQueueInternal::cleanup();
            KernelInternal::cleanup();
            ProgramInternal::cleanup();
            ContextInternal::cleanup();

            for(Device* device : all_devices) delete device;
            all_devices.clear();

            for(Platform* platform : all_platforms) delete platform;
            all_platforms.clear();
        }
    }
}