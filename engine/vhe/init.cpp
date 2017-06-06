#include "VHEInternal.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace VHE {

        Log _static_log_vhe("VHE");

        Log &log_vhe() { return _static_log_vhe; }
        Log &log_vhe(Logging::LogSeverity severity) { return _static_log_vhe(severity); }

        void init() {
            log_vhe().initFile();
        }

        void cleanup() {

            Environment::Memory::cleanup();
            Environment::Program::cleanup();
            Environment::VirtualEnvironment::cleanup();

        }
    }
}