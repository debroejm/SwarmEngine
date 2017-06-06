#include "PhysicsInternal.h"

#include "api/CLEngine.h"
#include "api/Logging.h"

#include <boost/thread.hpp>

using namespace Swarm::Logging;

// Kernel String Constants
#include "kernels/kernel_listing.cpp"

namespace Swarm {
    namespace Physics {

        // CL Data
        //CL::Context* _static_cl_context;

        void init() {

            // CL Creation
            // -----------

            // Check if there are any Devices
            if(CL::Device::getAll().empty()) {
                //Log::log_physics(ERR) << "No CL Devices Found to run Physics on (not even a CPU ?!)";
                // TODO: Throw Physics Init Exception
                return;
            }

            // Get the Best Device to use
            // TODO: Allow selecting of different and/or more CL Devices for Physics once a GUI exists
            CL::Device* device = nullptr;
            for(CL::Device* d : CL::Device::getAll()) {
                if(device == nullptr) device = d;
                else if(d->computeUnits() > device->computeUnits()) device = d;
            }
            //Log::log_physics(DEBUG) << "CL Device Selected for PHysics Calculations: " << device->name();

            // Create the Context
            const CL::Device* ctx_device_listing[]{ device };
            CL::Context* context = CL::Context::create(ctx_device_listing, 1);

            CL::Program program_movement(std::string(_static_kernel_calculation_movement), context);
            CL::Kernel kernel_movement = program_movement.kernel("SimpleMovement");

            // Start Thread
            startPhysicsThread();
        }

        void cleanup() {

            stopPhysicsThread();
        }




        // ***********
        //  Threading
        // ***********

        boost::thread _static_physics_thread;
        std::atomic<bool> _static_run_physics_thread(false);

        struct ThreadFunctr_Physics {
            void operator()() {
                try {

                    //Log::log_physics(DEBUG) << "Starting Physics Thread";
                    while (_static_run_physics_thread) {


                        boost::this_thread::interruption_point();
                    }

                } catch(std::exception &e) {
                    //Log::log_physics(ERR) << "Physics Thread Fatal Error: " << e.what();
                }
            }
        };

        void startPhysicsThread() {
            _static_run_physics_thread = true;
            _static_physics_thread = boost::thread(ThreadFunctr_Physics());
        }

        void stopPhysicsThread() {
            _static_run_physics_thread = false;
            _static_physics_thread.interrupt();
            _static_physics_thread.join();
        }
    }
}