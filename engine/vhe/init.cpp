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