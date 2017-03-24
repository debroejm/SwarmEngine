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
#include <iostream>
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
            void freeAll();
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
            size_t _next_index = 0;
            size_t _max_capacity = 0;
        public:
            ReadWriteBuffer() : ReadWriteBufferBase() { /* NOOP */ }
            ReadWriteBuffer(size_t capacity) : ReadWriteBufferBase(),
                                               _buffer_write(capacity),
                                               _max_capacity(capacity) {
                _buffer_read = new T[capacity];
            }

            virtual ~ReadWriteBuffer() {
                _buffer_write.clear();
                if(_buffer_read != nullptr) delete [] _buffer_read;
            }

            size_t capacity() const { return _buffer_write.size(); }
            size_t size() const { return _buffer_write.size() - _free_indices.size(); }

            void reserve(size_t capacity) {

                // Can only grow, not shrink
                if(capacity < _max_capacity || capacity < _buffer_write.size()) return;

                // Lock
                lock();
                lock_shared_explicit();

                // Resize and Flush
                if(_max_capacity != 0) _max_capacity = capacity;
                _buffer_write.resize(capacity);
                if(_buffer_read != nullptr) delete [] _buffer_read;
                _buffer_read = new T[capacity];
                std::memcpy(_buffer_read, _buffer_write.data(), sizeof(T) * _buffer_write.size());

                // Unlock
                unlock();
                unlock_shared_explicit();
            }

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

            void printBuffers() {

                // Lock
                lock();
                lock_shared_explicit();

                std::cout << "Size of Buffer: " << _buffer_write.size() << std::endl;
                for(int i = 0; i < _buffer_write.size(); i++) {
                    std::cout << "[Write-" << i << "] " << _buffer_write[i] << std::endl;
                    std::cout << "[Read -" << i << "] " << _buffer_read[i]  << std::endl;
                }

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
            BufferData(ReadWriteBuffer<T> &buffer, const T &val = T()) : _buffer(buffer) {

                // Lock
                _buffer.lock();
                _buffer.lock_shared_explicit();

                // Get the next free Index
                if(_buffer._free_indices.empty()) {
                    if(_buffer._max_capacity > 0 && _buffer._next_index >= _buffer._max_capacity) throw std::out_of_range("ReadWriteBuffer at maximum capacity");
                    else {
                        _index = _buffer._next_index++;
                        if(_buffer._max_capacity == 0) {
                            _buffer._buffer_write.resize(_buffer._next_index);

                            // Recreate read buffer
                            if(_buffer._buffer_read != nullptr) delete [] _buffer._buffer_read;
                            _buffer._buffer_read = new T[_buffer._buffer_write.size()];
                        }
                    }
                } else {
                    _index = *_buffer._free_indices.begin();
                    _buffer._free_indices.erase(_buffer._free_indices.begin());
                }

                // Add value and flush
                _buffer._buffer_write[_index] = val;
                std::memcpy(_buffer._buffer_read, _buffer._buffer_write.data(), sizeof(T) * _buffer._buffer_write.size());

                // Unlock
                _buffer.unlock();
                _buffer.unlock_shared_explicit();
            }

            ~BufferData() {
                _buffer._free_indices.insert(_index);
            }

            size_t index() { return _index; }

            void set(const T &val) {
                _buffer.lock();
                _buffer._buffer_write[_index] = val;
                _buffer.unlock();
            }

            T get() const {
                _buffer.lock_shared();
                T val = _buffer._buffer_read[_index];
                _buffer.unlock_shared();
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

            operator T() { return get(); }

            #if defined(SWARM_BOOST_AVAILABLE)
            void set(const T &val, boost::lock_guard<boost::mutex>&) { _buffer._buffer_write[_index] = val; }
            T get(boost::shared_lock<boost::shared_mutex>&) const { return _buffer._buffer_read[_index]; }
            T &getWriteAccess(boost::lock_guard<boost::mutex>&) { return _buffer._buffer_write[_index]; }
            #endif

        };

    }
}