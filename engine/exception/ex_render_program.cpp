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

#include "api/Exception.h"

namespace Swarm {
    namespace Exception {

        RenderProgramException::RenderProgramException(Type type, const std::string &message)
                : runtime_error(message), _type(type) {};

        RenderProgramException RenderProgramException::Link(SWMuint ID, const std::string &err) {
            return RenderProgramException(LINK,
                                          "Failed to link Program with ID '" + std::to_string(ID) + "':\n" + err);
        }
    }
}