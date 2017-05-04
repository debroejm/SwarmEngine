#include "VHEInternal.h"

namespace Swarm {
    namespace VHE {

        void init() {
            // NOOP
        }

        void cleanup() {

            Environment::Memory::cleanup();

        }
    }
}