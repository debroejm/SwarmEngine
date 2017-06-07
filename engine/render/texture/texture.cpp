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

#define SWARM_INCLUDE_GLEW
#include "api/Render.h"

namespace Swarm {
    namespace Texture {

        void TexMap::bind(SWMuint active) const {
            glActiveTexture(GL_TEXTURE0 + active);
            glBindTexture(_target, _id);
        }

        bool TexMap::operator==(const TexMap &rhs) const {
            return _target == rhs._target && _id == rhs._id;
        }

        bool TexMap::operator<(const TexMap &rhs) const {
            return _target == rhs._target ? _id < rhs._id : _target < rhs._target;
        }

        // TODO: Check to see if already bound
        void Texture::bind() const {
            for(auto && iter : _data) iter.second.bind(iter.first);
        }

        void Texture::put(SWMuint active, const TexMap &tex) {
            _data[active] = tex;
        }

        TexMap Texture::at(SWMuint active) const {
            if(_data.count(active)) return _data.at(active);
            else return TexMap{ Type::TargetType::TEX_2D, 0 };
        }

        TexMap &Texture::operator[](SWMuint active) {
            if(!_data.count(active))
                _data[active] = TexMap{ Type::TargetType::TEX_2D, 0 };
            return _data[active];
        }

        bool Texture::operator==(const Texture &rhs) const {
            const Texture &lhs = *this;
            std::map<SWMuint, TexMap>::const_iterator it_lhs = lhs._data.begin();
            std::map<SWMuint, TexMap>::const_iterator it_rhs = rhs._data.begin();
            while(it_lhs != lhs._data.end() && it_rhs != rhs._data.end()) {
                if(it_lhs->first  != it_rhs->first)  return false;
                if(it_lhs->second != it_rhs->second) return false;
                it_lhs++; it_rhs++;
            }
            return true;
        }

        bool Texture::operator<(const Texture &rhs) const {
            const Texture &lhs = *this;
            std::map<SWMuint, TexMap>::const_iterator it_lhs = lhs._data.begin();
            std::map<SWMuint, TexMap>::const_iterator it_rhs = rhs._data.begin();
            while(it_lhs != lhs._data.end() && it_rhs != rhs._data.end()) {
                if(it_lhs->first  != it_rhs->first)  return it_lhs->first  < it_rhs->first;
                if(it_lhs->second != it_rhs->second) return it_lhs->second < it_rhs->second;
                it_lhs++; it_rhs++;
            }
            return false;
        }
    }
}