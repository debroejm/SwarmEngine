#pragma once



// ***************
//  Common Header
// ***************

#define SWARM_REQUEST_BOOST_MUTEX
#include "common_header.h"
#undef SWARM_REQUEST_BOOST_MUTEX

#include <algorithm>
#include <atomic>
#include <cstring>
#include <set>
#include <vector>



namespace Swarm {
    namespace Util {

        // TODO: Util Documention

        #if defined(SWARM_INCLUDE_GLM)
        inline glm::vec3 movePoint(glm::vec3 point, glm::vec3 target, float distance) {
            return point + ( glm::normalize(target-point) * std::max( distance, glm::distance(point, target) ) );
        }
        #endif

        class UIDPool {
        public:
            size_t next();
            void free(size_t id);
        protected:
            size_t _next = 0;
            std::set<size_t> _free_ids;
        };

        class DeltaTimeController {
        public:
            DeltaTimeController();
            double get();
        protected:
            double _last_time;
        };

        template<typename T> class BufferData;

        template<typename T> class ReadWriteBuffer;
        template<typename T> class BufferData;

        class ReadWriteBufferBase {
        private:
            template<typename T> friend class ReadWriteBuffer;
            ReadWriteBufferBase();
            virtual ~ReadWriteBufferBase();
        protected:
            template<typename T> friend class BufferData;
            void lock();
            void unlock();
            void lock_shared();
            void unlock_shared();
            void lock_shared_explicit();
            void unlock_shared_explicit();
            size_t _uid;
        public:
            #if defined(SWARM_BOOST_AVAILABLE)
            boost::mutex& write_mutex() const;
            boost::shared_mutex& read_mutex() const;
            #endif
        };

        template<typename T> class ReadWriteBuffer : public ReadWriteBufferBase {
        protected:
            friend class BufferData<T>;
            std::vector<T> _buffer_write;
            T* _buffer_read = nullptr;
            std::set<size_t> _free_indices;
        public:
            virtual ~ReadWriteBuffer() {
                _buffer_write.clear();
                delete [] _buffer_read;
            }

            size_t capacity() const { return _buffer_write.size(); }
            size_t size() const { return _buffer_write.size() - _free_indices.size(); }

            void flush() {

                // Lock
                lock();
                lock_shared_explicit();

                // Flush
                if(_buffer_read != nullptr) delete [] _buffer_read;
                _buffer_read = new T[_buffer_write.size()];
                std::memcpy(_buffer_read, _buffer_write.data(), sizeof(T) * _buffer_write.size());

                // Unlock
                unlock();
                unlock_shared_explicit();
            }
        };

        template<typename T> class BufferData {
        protected:
            ReadWriteBuffer<T> &_buffer;
            size_t _index;

        public:
            BufferData(ReadWriteBuffer<T> &buffer) : _buffer(buffer) {

                // Lock
                _buffer.lock();
                _buffer.lock_shared_explicit();

                // Modify
                if(_buffer._free_indices.empty()) {
                    _index = _buffer._buffer_write.size();
                    _buffer._buffer_write.push_back(T());
                } else {
                    _index = *_buffer._free_indices.begin();
                    _buffer._free_indices.erase(_buffer._free_indices.begin());
                    _buffer._buffer_write[_index] = T();
                }

                // Flush
                if(_buffer._buffer_read != nullptr) delete [] _buffer._buffer_read;
                _buffer._buffer_read = new T[_buffer._buffer_write.size()];
                std::memcpy(_buffer._buffer_read,_buffer. _buffer_write.data(), sizeof(T) * _buffer._buffer_write.size());

                // Unlock
                _buffer.unlock();
                _buffer.unlock_shared_explicit();
            }

            BufferData(ReadWriteBuffer<T> &buffer, const T &val) : _buffer(buffer) {

                // Lock
                _buffer.lock();
                _buffer.lock_shared_explicit();

                // Modify
                if(_buffer._free_indices.empty()) {
                    _index = _buffer._buffer_write.size();
                    _buffer._buffer_write.push_back(val);
                } else {
                    _index = *_buffer._free_indices.begin();
                    _buffer._free_indices.erase(_buffer._free_indices.begin());
                    _buffer._buffer_write[_index] = val;
                }

                // Flush
                if(_buffer._buffer_read != nullptr) delete [] _buffer._buffer_read;
                _buffer._buffer_read = new T[_buffer._buffer_write.size()];
                std::memcpy(_buffer._buffer_read, _buffer._buffer_write.data(), sizeof(T) * _buffer._buffer_write.size());

                // Unlock
                _buffer.unlock();
                _buffer.unlock_shared_explicit();
            }

            ~BufferData() {
                _buffer._free_indices.insert(_index);
            }

            void set(const T &val) {
                _buffer.lock();
                _buffer._buffer_write[_index] = val;
                _buffer.unlock();
            }

            const T get() const {
                _buffer.lock_shared();
                T val = _buffer._buffer_read[_index];
                _buffer.unlock_shared();
                return val;
            }

            T getModified() {
                _buffer.lock();
                T val = _buffer._buffer_write[_index];
                _buffer.unlock();
                return val;
            }

            BufferData<T> &operator=(const T &rhs) {
                set(rhs);
                return *this;
            }

            BufferData<T> &operator=(const BufferData<T> &rhs) {
                set(rhs.get());
                return *this;
            }

            T operator*() { return get(); }

            #if defined(SWARM_BOOST_AVAILABLE)
            void set(const T &val, boost::lock_guard<boost::mutex>&) { _buffer._buffer_write[_index] = val; }
            const T get(boost::shared_lock<boost::shared_mutex>&) const { return _buffer._buffer_read[_index]; }
            T &getModified(boost::lock_guard<boost::mutex>&) { return _buffer._buffer_write[_index]; }
            #endif

        };

    }
}