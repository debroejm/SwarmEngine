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

#define SWARM_BOOST_AVAILABLE
#include "api/Util.h"

#include <unordered_map>

namespace Swarm {
    namespace Util {

        // Static Mutex maps because can't put them in the Header definition; keeping 3rd party libraries out of headers
        std::unordered_map<size_t, boost::shared_mutex> _static_buffer_mutex_read;
        std::unordered_map<size_t, boost::mutex> _static_buffer_mutex_write;

        UIDPool _static_buffer_id_pool;

        ReadWriteBufferBase::ReadWriteBufferBase() {
            _uid = _static_buffer_id_pool.next();
        }

        ReadWriteBufferBase::~ReadWriteBufferBase() {
            _static_buffer_id_pool.free(_uid);
        }

        void ReadWriteBufferBase::lock() {
            _static_buffer_mutex_write[_uid].lock();
        }

        void ReadWriteBufferBase::unlock() {
            _static_buffer_mutex_write[_uid].unlock();
        }

        void ReadWriteBufferBase::lock_shared() {
            _static_buffer_mutex_read[_uid].lock_shared();
        }

        void ReadWriteBufferBase::unlock_shared() {
            _static_buffer_mutex_read[_uid].unlock_shared();
        }

        void ReadWriteBufferBase::lock_shared_explicit() {
            _static_buffer_mutex_read[_uid].lock();
        }

        void ReadWriteBufferBase::unlock_shared_explicit() {
            _static_buffer_mutex_read[_uid].unlock();
        }

        boost::mutex& ReadWriteBufferBase::write_mutex() const {
            return _static_buffer_mutex_write[_uid];
        }

        boost::shared_mutex& ReadWriteBufferBase::read_mutex() const {
            return _static_buffer_mutex_read[_uid];
        }

    }
}