#include "../CLEngine.h"
#include "../Core.h"

#include <unordered_map>
#include <vector>

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        DeviceContextListing::DeviceContextListing(const DeviceInfo &info)
                : platform(info.platform) {
            devices.clear();
            devices.push_back(info.device);
        }

        void DeviceContextListing::addDevice(const DeviceInfo &info) {
            if(platform != info.platform) {
                Log::log_cl(WARNING) << "Tried to add a Device with a different Platform to a ContextListing; Device '" << info.deviceName << "'";
                return;
            } else devices.push_back(info.device);
        }

        // This is a pointer to pointer map
        std::unordered_map<cl_platform_id, cl_context> registered_contexts;

        Context::Context(DeviceInfo &info) {
            DeviceContextListing listing(info);
            this->info = info;
            if(registered_contexts.count(info.platform) > 1)
                context = registered_contexts[info.platform];
            else {
                context = createContext(listing);
            }
        }

        Context::Context(DeviceContextListing &listing) {
            this->info = listing;
            if (registered_contexts.count(listing.platform) > 1)
                context = registered_contexts[listing.platform];
            else
                context = createContext(listing);
        }

        cl_context Context::createContext(DeviceContextListing &listing) {

            const cl_context_properties props[] = {
                    CL_CONTEXT_PLATFORM,
                    reinterpret_cast<cl_context_properties> (listing.platform),
                    0, 0
            };

            // Pick out the 'available' devices
            std::vector<cl_device_id> devices;
            for(int i = 0; i < listing.devices.size(); i++) {
                cl_bool available;
                cl_int result = clGetDeviceInfo(listing.devices[i], CL_DEVICE_AVAILABLE, 4, &available, nullptr);
                if(result == CL_SUCCESS && available) devices.push_back(listing.devices[i]);
            }

            cl_int error;
            cl_context context = clCreateContext( props, devices.size(), devices.data(), nullptr, nullptr, &error );

            // Check Error
            if(error != CL_SUCCESS) {
                switch(error) {
                    case CL_INVALID_PLATFORM:
                        Log::log_cl(ERR) << "Invalid Platform to create CL Context with";
                        break;
                    case CL_INVALID_VALUE:
                        Log::log_cl(ERR) << "Invalid Value when creating CL Context; unsupported Property name, no/null Devices, or failure to provide callback with user_data";
                        break;
                    case CL_INVALID_DEVICE:
                        Log::log_cl(ERR) << "Invalid Device or Device not associated with Platform when creating CL Context";
                        break;
                    case CL_DEVICE_NOT_AVAILABLE:
                        Log::log_cl(ERR) << "Device Not Available to create CL Context with";
                        break;
                    case CL_OUT_OF_HOST_MEMORY:
                        Log::log_cl(ERR) << "Host out of Memory when creating CL Context";
                        break;
                    default:
                        Log::log_cl(ERR) << "Unknown Error when creating CL Context; '" << error << "'";
                        break;
                }
                return nullptr;
            }

            registered_contexts[listing.platform] = context;

            return context;
        }

        void Context::cleanup() {
            for(auto && iter : registered_contexts) clReleaseContext( iter.second );
            registered_contexts.clear();
        }
    }
}