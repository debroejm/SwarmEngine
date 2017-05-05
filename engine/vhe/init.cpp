#include "VHEInternal.h"

namespace Swarm {
    namespace VHE {

        void init() {
            // NOOP
        }

        void cleanup() {

            Environment::Memory::cleanup();
            Environment::Program::cleanup();
            Environment::VirtualEnvironment::cleanup();

        }
    }
}