#include "../CLEngine.h"
#include "../Core.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        void init() {

            /*
            // Get a list of platforms on the current system
            cl_uint platformIDCount = 0;
            clGetPlatformIDs(0, nullptr, &platformIDCount);
            if(platformIDCount < 1) {
                Log::log_cl(ERR) << "No OpenCL Platforms were detected; OpenCL cannot initialize.";
                return;
            }
            std::vector<cl_platform_id> platformIDs(platformIDCount);
            clGetPlatformIDs(platformIDCount, platformIDs.data(), nullptr);

            // Iterate through found platforms
            for(cl_platform_id pid : platformIDs) {

                // Get the Platform Name
                char pname[256];
                clGetPlatformInfo(pid, CL_PLATFORM_NAME, 256, &pname[0], nullptr);

                // Get a list of devices for the current platform
                cl_uint deviceIDCount = 0;
                clGetDeviceIDs(pid, CL_DEVICE_TYPE_ALL, 0, nullptr, &deviceIDCount);
                if(deviceIDCount < 1) {
                    Log::log_cl(INFO) << "No Devices Found for Platform '" << pname << "'";
                    break;
                } else Log::log_cl(INFO) << deviceIDCount << " Devices Found for Platform '" << pname << "'";
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

                    // Register Device Info
                    all_device_info.insert({pid, did, computeUnits, string(dname)});
                }
            }
             */
        }

        void cleanup() {
            //Context::cleanup();
        }
    }
}