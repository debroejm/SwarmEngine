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

        ModelLoadingException::ModelLoadingException(Type type, const std::string &message)
                : runtime_error(message), _type(type) {}

        ModelLoadingException ModelLoadingException::FileLoad(const std::string& path) {
            return ModelLoadingException(FILE_LOAD,
                                         "Failed to open file '" + path + "'");
        }

        ModelLoadingException ModelLoadingException::IndexOutOfBounds(const std::string& type, size_t inv_index, size_t max_index) {
            return ModelLoadingException(INDEX_OUT_OF_BOUNDS,
                                         "'" + type + "' Index Out of Bounds; " + std::to_string(inv_index)
                                         + " greater than maximum of " + std::to_string(max_index));
        }

        ModelLoadingException ModelLoadingException::ParseFail(std::exception ex_list[], size_t count) {
            std::string message("");
            for(int i = 0; i < count; i++) message += (std::string(ex_list[i].what()) + "\n");
            return ModelLoadingException(PARSE_FAIL, message);
        }

    }
}
