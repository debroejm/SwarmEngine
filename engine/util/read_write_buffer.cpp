#define SWARM_BOOST_AVAILABLE
#include "api/Util.h"

#include <cstring>

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