#include "Core.h"
#include "CLEngine.h"

using namespace Swarm;

using namespace Swarm::Logging;

int main() {

    // Initialization
    if(!Init::init(SWM_INIT_CL)) {
        return -1;
    }

    if(CL::getAllDevices().empty()) { Log::log_cl(DEBUG) << "No CL Devices Found"; return 0; }


    // Get the Best Device
    CL::DeviceInfo device = *CL::getAllDevices().rbegin();
    Log::log_cl(DEBUG) << "Auto-Selected CL Device: " << device.deviceName;

    // Create Our Context
    CL::DeviceContextListing ctxInfo(device);
    CL::Context ctx(ctxInfo);

    const char *src = R"(
__kernel void SAXPY (__global float* x, __global float* y, float a)
{
    const int i = get_global_id(0);

    y [i] += a * x [i];
}
)";

    // Create Our Program
    CL::Program program(std::string(src), ctx);
    CL::Kernel kernel(program, "SAXPY");

    // Create Some Test Data
    static const size_t testDataSize = 1 << 10;
    std::vector<float> a (testDataSize), b (testDataSize);
    for (int i = 0; i < testDataSize; ++i) {
        a[i] = static_cast<float> (23 ^ i);
        b[i] = static_cast<float> (42 ^ i);
    }

    // Put Our Data in Buffers
    CL::Buffer buff_a(ctx, CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR, sizeof(float) * (testDataSize), a.data());
    CL::Buffer buff_b(ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * (testDataSize), b.data());

    // Create Our Command Queue
    CL::CommandQueue queue(ctx, device);

    // Set Our Kernel Arguments
    kernel.setArgument(0, buff_a);
    kernel.setArgument(1, buff_b);
    static const float two = 2.0f;
    kernel.setArgument(2, sizeof(float), &two);

    // Enqueue Our Commands
    queue.enqueueCommand(kernel, testDataSize);
    queue.enqueueRead(buff_b, true, sizeof(float) * testDataSize, b.data());

    // Print the Resulting Data
    Log::log_core(INFO) << "SAXPY Results:\n";
    for(int i = 0; i < b.size(); i++)
        Log::log_core << "\t" << b[i] << "\n";

    // Cleanup Everything Up When Done
    Init::cleanup();

}