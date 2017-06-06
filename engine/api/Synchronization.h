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

#pragma once



// ***************
//  Common Header
// ***************

#define SWARM_REQUEST_BOOST_MUTEX
#include "common_header.h"
#undef SWARM_REQUEST_BOOST_MUTEX


// ***********
//  API Begin
// ***********

namespace Swarm {
    namespace Synchro {

        #if defined(SWARM_BOOST_AVAILABLE)
        //! Wrapper around std::map that synchronizes all methods
        /*!
         * SyncMap is a wrapper around an \c std::map that synchronizes access to every \c std::map method by using a
         * many-read/single-write mutex. Methods that change the size of the \c std::map require write privileges, while
         * all other methods only require read privileges.
         *
         * THIS CLASS REQUIRES \c SWARM_BOOST_AVAILABLE TO BE DEFINED
         *
         * \tparam K The type to use as the key for the \c std::map
         * \tparam V The type to use as the value for the \c std::map
         */
        template<typename K, typename V> class SyncMap {
        private:
            typedef std::map<K,V> Map;
            typedef typename Map::iterator iterator;
            typedef typename Map::const_iterator const_iterator;
            typedef typename Map::reverse_iterator reverse_iterator;
            typedef typename Map::const_reverse_iterator const_reverse_iterator;
            typedef typename Map::size_type size_type;
            typedef typename Map::value_type value_type;
            typedef boost::lock_guard<boost::upgrade_mutex> Lock;
            typedef boost::shared_lock_guard<boost::upgrade_mutex> SharedLock;
            Map _map;
            boost::upgrade_mutex _mutex;

        public:

            // Assignment Operators
            Map& operator=(const Map& x)                         { Lock lock(_mutex); return _map = x; }
            Map& operator=(Map&& x)                              { Lock lock(_mutex); return _map = x; }
            Map& operator=(std::initializer_list<value_type> il) { Lock lock(_mutex); return _map = il; }

            // Iterators
            iterator               begin()         { SharedLock lock(_mutex); return _map.begin(); }
            const_iterator         begin()   const { SharedLock lock(_mutex); return _map.begin(); }
            iterator               end()           { SharedLock lock(_mutex); return _map.end(); }
            const_iterator         end()     const { SharedLock lock(_mutex); return _map.end(); }
            reverse_iterator       rbegin()        { SharedLock lock(_mutex); return _map.rbegin(); }
            const_reverse_iterator rbegin()  const { SharedLock lock(_mutex); return _map.rbegin(); }
            reverse_iterator       rend()          { SharedLock lock(_mutex); return _map.rend(); }
            const_reverse_iterator rend()    const { SharedLock lock(_mutex); return _map.rend(); }
            const_iterator         cbegin()  const { SharedLock lock(_mutex); return _map.cbegin(); }
            const_iterator         cend()    const { SharedLock lock(_mutex); return _map.cend(); }
            const_reverse_iterator crbegin() const { SharedLock lock(_mutex); return _map.crbegin(); }
            const_reverse_iterator crend()   const { SharedLock lock(_mutex); return _map.crend(); }

            // Capacity
            bool      empty()    const { SharedLock lock(_mutex); return _map.empty(); }
            size_type size()     const { SharedLock lock(_mutex); return _map.size(); }
            size_type max_size() const { SharedLock lock(_mutex); return _map.max_size(); }

            // Element Access
                  V& at(const K& key)       { SharedLock lock(_mutex); return _map.at(key); }
            const V& at(const K& key) const { SharedLock lock(_mutex); return _map.at(key); }
            V& operator[](const K& key) {
                boost::upgrade_lock<boost::upgrade_mutex> lock(_mutex);
                if(_map.count(key) < 1) {
                    boost::upgrade_to_unique_lock<boost::upgrade_mutex> ulock(lock);
                    return _map[key];
                } else return _map.at(key);
            }

            // Modifiers
            std::pair<iterator, bool> insert(const value_type& val)                          { Lock lock(_mutex); return _map.insert(val); }
            iterator                  insert(const_iterator position, const value_type& val) { Lock lock(_mutex); return _map.insert(position, val); }
            void                      insert(std::initializer_list<value_type> il)           { Lock lock(_mutex); return _map.insert(il); }
            iterator  erase(const_iterator position)                   { Lock lock(_mutex); return _map.erase(position); }
            size_type erase(const K& key)                              { Lock lock(_mutex); return _map.erase(key); }
            iterator  erase(const_iterator first, const_iterator last) { Lock lock(_mutex); return _map.erase(first, last); }
            void      swap(Map& x) { Lock lock(_mutex); _map.swap(x); }
            void      clear()      { Lock lock(_mutex); _map.clear(); }

            // Operations
            iterator       find(const K& key)              { SharedLock lock(_mutex); return _map.find(key); }
            const_iterator find(const K& key)        const { SharedLock lock(_mutex); return _map.find(key); }
            size_type      count(const K& key)       const { SharedLock lock(_mutex); return _map.count(key); }
            iterator       lower_bound(const K& key)       { SharedLock lock(_mutex); return _map.lower_bound(key); }
            const_iterator lower_bound(const K& key) const { SharedLock lock(_mutex); return _map.lower_bound(key); }
            iterator       upper_bound(const K& key)       { SharedLock lock(_mutex); return _map.upper_bound(key); }
            const_iterator upper_bound(const K& key) const { SharedLock lock(_mutex); return _map.upper_bound(key); }
        };
        #endif

    }
}