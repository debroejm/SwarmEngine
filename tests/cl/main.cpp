#include "api/Core.h"
#include "api/Logging.h"
#include "api/CLEngine.h"

using namespace Swarm;
using namespace Swarm::Logging;

// Logger
Log log_test("Test");

int main() {

    // Initialization
    if(!Core::init(SWM_INIT_CL)) {
        return -1;
    }

    log_test.initFile();

    if(CL::Device::getAll().empty()) {
        log_test(WARNING) << "No CL Devices Found, Exiting Test" << Flush();
        Core::cleanup();
        return 0;
    }

    // Get the Best Device
    CL::Device* device = nullptr;
    for(CL::Device* d : CL::Device::getAll()) {
        log_test(DEBUG)
                << "Device '" << d->name()
                << "' on Platform '" << d->platform()->name()
                << "' with " << d->computeUnits()
                << " compute cores";
        if(device == nullptr) device = d;
        else if(d->computeUnits() > device->computeUnits()) device = d;
    }
    log_test.flush();
    if(device == nullptr) { Core::cleanup(); return 0; }
    log_test(INFO) << "Auto-Selected CL Device: " << device->name() << Flush();

    // Create Our Context
    const CL::Device* ctx_device_listing[]{ device };
    CL::Context* context = CL::Context::create(ctx_device_listing, 1);

    const char *src = R"(
__kernel void SAXPY (__global float* x, __global float* y, float a)
{
    const int i = get_global_id(0);

    y [i] += a * x [i];
}
)";

    // Create Our Program
    CL::Program program(std::string(src), context);
    CL::Kernel kernel = program.kernel("SAXPY");

    // Create Some Test Data
    static const size_t testDataSize = 1 << 10;
    float a[testDataSize], b[testDataSize];
    for (int i = 0; i < testDataSize; ++i) {
        a[i] = static_cast<float> (23 ^ i);
        b[i] = static_cast<float> (42 ^ i);
    }

    // Put Our Data in Buffers
    CL::Buffer<float> buff_a(context, true, false, testDataSize, &a[0]);
    CL::Buffer<float> buff_b(context, true, true,  testDataSize, &b[0]);

    // Create Our Command Queue
    CL::CommandQueue queue(context, device);

    // Set Our Kernel Arguments
    kernel.argument(0, buff_a);
    kernel.argument(1, buff_b);
    static const float two = 2.0f;
    kernel.argument(2, 1, &two);

    // Enqueue Our Commands
    queue.enqueueCommand(kernel, testDataSize);
    queue.enqueueRead(buff_b, true);

    // Print the Resulting Data
    log_test(INFO) << "SAXPY Results:";
    for(int i = 0; i < testDataSize; i++)
        log_test(INFO) << "\t" << b[i];
    log_test.flush();

    // Cleanup Everything Up When Done
    Core::cleanup();

}