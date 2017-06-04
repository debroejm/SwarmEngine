#include "../VHEInternal.h"

namespace Swarm {
    namespace VHE {
        namespace Environment {

            std::set<MemoryInternal*> _static_registered_memory;

            void Memory::cleanup() {
                for(MemoryInternal* mem : _static_registered_memory)
                    delete mem;
                _static_registered_memory.clear();
            }

            Memory::Memory(size_t mem_size, MemoryPrefix prefix) {
                _memory = new MemoryInternal(mem_size, prefix);
                _static_registered_memory.insert(_memory);
            }

            vbyte* Memory::allocMemChunk(size_t size, size_t *begin, size_t *end) {

                // Start with the smallest and iterate bigger
                MemoryInternal::FreeSet::iterator it = _memory->_free_set.begin();
                while(it != _memory->_free_set.end()) {
                    MemoryInternal::FreeSector* sect = (*it);

                    // Select the first free region that can fit the requested size
                    if((sect->end - sect->begin) >= size) {

                        // Save/output the begin/end indices
                        vbyte* resultByte = &_memory->_data[sect->begin];
                        if(begin != nullptr) *begin = sect->begin;
                        if(end != nullptr)   *end   = sect->begin+size-1;

                        // Change free region begin index to match up to remaining size
                        size_t newBegin = sect->begin+size;
                        if(newBegin < sect->end) { // Redirect references of the prev/next to the new section
                            MemoryInternal::FreeSector* newSect = new MemoryInternal::FreeSector(newBegin, sect->end, sect->prev, sect->next);
                            if(sect->prev != nullptr) sect->prev->next = newSect;
                            if(sect->next != nullptr) sect->next->prev = newSect;
                            if(_memory->_free_start == sect) _memory->_free_start = newSect;
                            if(_memory->_free_end == sect) _memory->_free_end = newSect;
                            _memory->_free_set.insert(newSect);
                        } else { // Close the reference gap between the prev/next of this removed section
                            if(sect->prev != nullptr) sect->prev->next = sect->next;
                            if(sect->next != nullptr) sect->next->prev = sect->prev;
                            if(_memory->_free_start == sect) _memory->_free_start = sect->next;
                            if(_memory->_free_end == sect) _memory->_free_end = sect->prev;
                        }

                        _memory->_free_set.erase(it);
                        delete sect;

                        // Return a pointer to the beginning of the allocated region
                        return resultByte;
                    }
                    it++;
                }

                throw Exception::EnvironmentException::MemoryOutOfSpace(size);
            }

            void Memory::freeMemChunk(size_t begin, size_t end) {

                // Swap indices if they are out of order
                if(begin > end) {
                    size_t t = end;
                    end = begin;
                    begin = t;
                }

                // Range Check
                if(begin < 0 || end >= _memory->_size_in_bytes)
                    throw Exception::EnvironmentException::MemoryFreeOutOfRange(begin, end, _memory->_size_in_bytes);

                if(_memory->_free_start == nullptr) {
                    // No existing free space, make a hole
                    _memory->_free_start = _memory->_free_end = new MemoryInternal::FreeSector( begin, end, nullptr, nullptr );
                    _memory->_free_set.insert(_memory->_free_start);
                } else {

                    // Cache variables
                    MemoryInternal::FreeSector* prev = nullptr;
                    MemoryInternal::FreeSector* next = nullptr;
                    bool lookingForBegin = true;

                    // Start iteration through in physical location order
                    MemoryInternal::FreeSector* it = _memory->_free_start;
                    while(it != nullptr) {
                        if(lookingForBegin) {

                            // Look for a section after or encompassing the begin index
                            if (begin <= it->begin) {
                                prev = it->prev;
                                lookingForBegin = false;
                                continue;
                            } else if (begin <= it->end) {
                                begin = it->begin;
                                prev = it->prev;
                            } else {
                                it = it->next;
                                continue;
                            }
                            lookingForBegin = false;
                        } else {

                            // Look for a section after or encompassing the end index
                            if (end < it->begin) {
                                next = it;
                                break;
                            } else if(end <= it->end) {
                                next = it->next;
                                end = it->end;
                                _memory->removeFromFreeSet(it);
                                break;
                            }
                        }

                        MemoryInternal::FreeSector* n = it->next;
                        // Remove section from the free set (note: does not update references)
                        _memory->removeFromFreeSet(it);
                        it = n;
                    }

                    MemoryInternal::FreeSector* newSect = new MemoryInternal::FreeSector( begin, end, prev, next );

                    // Update boundary references
                    if(prev == nullptr) _memory->_free_start = newSect;
                    else prev->next = newSect;
                    if(next == nullptr) _memory->_free_end = newSect;
                    else next->prev = newSect;

                    _memory->_free_set.insert(newSect);
                }
            }

            size_t Memory::sizeInBytes() const { return _memory->_size_in_bytes; }

            std::string Memory::printFreeSectionsChronological() const {
                if(_memory->_free_start == nullptr) {
                    return "No Free Sections";
                } else {
                    MemoryInternal::FreeSector* it = _memory->_free_start;
                    std::string result("");
                    while (it != nullptr) {
                        result += "[" + std::to_string(it->begin) + ":" + std::to_string(it->end) + "|" + std::to_string(it->end - it->begin) + "]-";
                        it = it->next;
                    }
                    return result;
                }
            }

            std::string Memory::printFreeSectionsOrdered() const {
                if(_memory->_free_start == nullptr) {
                    return "No Free Sections";
                } else {
                    MemoryInternal::FreeSet::iterator it = _memory->_free_set.begin();
                    std::string result("");
                    while(it != _memory->_free_set.end()) {
                        MemoryInternal::FreeSector* sect = *it;
                        result += "[" + std::to_string(sect->begin) + ":" + std::to_string(sect->end) + "|" + std::to_string(sect->end - sect->begin) + "]-";
                        it++;
                    }
                    return result;
                }
            }

        }
    }
}