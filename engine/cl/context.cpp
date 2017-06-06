#include "CLInternal.h"
#include "api/Logging.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace CL {

        std::set<ContextInternal*> registered_contexts;

        Context* Context::create(const Device* devices[], size_t count) {
            ContextInternal* ctx = new ContextInternal((const DeviceInternal**)devices, count);
            registered_contexts.insert(ctx);
            return ctx;
        }

        ContextInternal::ContextInternal(const DeviceInternal* devices[], size_t count) {

            std::vector<const DeviceInternal*> valid_devices;
            cl_platform_id chosen_platform = nullptr;
            for(size_t i = 0; i < count; i++) {
                if(devices[i] == nullptr) continue;
                if(chosen_platform == nullptr) chosen_platform = devices[i]->_platform.platform();
                else if(devices[i]->_platform.platform() != chosen_platform) {
                    log_cl(WARNING) << "Attempted to create CL Context using Devices with mismatched Platforms; ignoring Device '"
                                         << devices[i]->name() << "' from Platform '" << devices[i]->_platform.name() << "'.";
                    continue;
                }

                cl_bool available;
                cl_int result = clGetDeviceInfo(devices[i]->device(), CL_DEVICE_AVAILABLE, 4, &available, nullptr);
                if(result == CL_SUCCESS && available) valid_devices.push_back(devices[i]);
                else {
                    log_cl(WARNING) << "Attempted to create CL Context using unavailable Device '" << devices[i]->name()
                                         << "'; ignoring Device.";
                }
            }
            log_cl().flush();

            // Check for no valid platform (most likely from having no devices passed in)
            if(chosen_platform == nullptr) {
                log_cl(WARNING) << "Attempted to create CL Context with no Devices; aborting creation." << Flush();
                return;
            }

            log_cl(DEBUG) << "Creating CL Context..." << Flush();

            // Create the context properties array
            const cl_context_properties props[] = {
                    CL_CONTEXT_PLATFORM,
                    reinterpret_cast<cl_context_properties> (chosen_platform),
                    0, 0
            };

            // Convert DeviceInternal vector to cl_device_id array
            cl_device_id* device_data = new cl_device_id[valid_devices.size()];
            _devices.clear();
            for(int i = 0; i < valid_devices.size(); i++) {
                device_data[i] = valid_devices[i]->device();
                _devices.insert((Device*)&(*valid_devices[i]));
            }

            // Create the context
            cl_int error;
            _context = clCreateContext( props, (cl_uint)valid_devices.size(), device_data, nullptr, nullptr, &error );

            // Check Error
            if(error != CL_SUCCESS) {
                switch(error) {
                    case CL_INVALID_PLATFORM:
                        log_cl(ERR) << "Invalid Platform to create CL Context with";
                        break;
                    case CL_INVALID_VALUE:
                        log_cl(ERR) << "Invalid Value when creating CL Context; unsupported Property name, no/null Devices, or failure to provide callback with user_data";
                        break;
                    case CL_INVALID_DEVICE:
                        log_cl(ERR) << "Invalid Device or Device not associated with Platform when creating CL Context";
                        break;
                    case CL_DEVICE_NOT_AVAILABLE:
                        log_cl(ERR) << "Device Not Available to create CL Context with";
                        break;
                    case CL_OUT_OF_HOST_MEMORY:
                        log_cl(ERR) << "Host out of Memory when creating CL Context";
                        break;
                    default:
                        log_cl(ERR) << "Unknown Error when creating CL Context; '" << error << "'";
                        break;
                }
                log_cl().flush();
                _context = nullptr;
                _devices.clear();
                return;
            }
        }

        ContextInternal::~ContextInternal() {
            if(_context != nullptr) clReleaseContext(_context);
        }

        void ContextInternal::cleanup() {
            for(ContextInternal* ctx : registered_contexts) delete ctx;
            registered_contexts.clear();
        }
    }
}