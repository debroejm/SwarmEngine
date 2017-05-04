#pragma once

#include "api/VHE.h"

#include <list>
#include <math.h>
#include <set>

// ************
//  Code Begin
// ************

namespace Swarm {
    namespace VHE {

        struct compiler_command;
        typedef std::list<compiler_command*> cc_list;
        typedef std::list<compiler_command*>::iterator cc_iter;

        struct abstract_statement;
        typedef std::list<abstract_statement*> stmt_list;
        typedef std::list<abstract_statement*>::iterator stmt_iter;

        namespace Environment {

            struct MemoryInternal {

                struct FreeSector {
                    const size_t begin = 0; // Inclusive
                    const size_t end = 0; // Exclusive
                    FreeSector* prev = nullptr;
                    FreeSector* next = nullptr;
                    FreeSector(size_t begin, size_t end, FreeSector* prev, FreeSector* next)
                            : begin(begin), end(end), prev(prev), next(next) {}
                };

                struct FreeCompare {
                    bool operator() (const FreeSector *lhs, const FreeSector *rhs) const
                    { return (lhs == nullptr ? 0 : (lhs->end-lhs->begin)) < (rhs == nullptr ? 0 : (rhs->end-rhs->begin)); }
                };

                typedef std::multiset<FreeSector*, FreeCompare> FreeSet;

                vbyte* _data = nullptr;
                size_t _size_in_bytes = 0;
                FreeSector* _free_start;
                FreeSector* _free_end;
                FreeSet _free_set;

                MemoryInternal(size_t mem_size, MemoryPrefix prefix) {
                    _size_in_bytes = mem_size * prefix;
                    _data = new vbyte[_size_in_bytes];
                    for(size_t i = 0; i < _size_in_bytes; i++) _data[i] = 0;
                    _free_start = _free_end = new FreeSector( 0, _size_in_bytes, nullptr, nullptr );
                    _free_set.insert(_free_start);
                }

                ~MemoryInternal() {
                    delete [] _data;
                    for(FreeSector* sect : _free_set) delete sect;
                }

                // DOES NOT UPDATE REFERENCES
                void removeFromFreeSet(FreeSector* sect) {
                    if(sect == nullptr) return;
                    std::pair<FreeSet::iterator, FreeSet::iterator> range = _free_set.equal_range(sect);
                    FreeSet::iterator it = range.first;
                    while(it != range.second) {
                        if(*it == sect) {
                            _free_set.erase(it);
                            break;
                        }
                        it++;
                    }
                    delete sect;
                }

            };

            struct VEInternal {

                Memory _memory;
                Register _registers[];
                vbyte _register_count;
                size_t _stack_size_in_bytes;
                BitWidth _max_bit_width;

                VEInternal(BitWidth max_bit_width, vbyte register_count,
                           size_t mem_size, MemoryPrefix mem_prefix,
                           size_t stack_size, MemoryPrefix stack_prefix)
                        : _memory(mem_size, mem_prefix), _registers(new Register[register_count]),
                          _register_count(register_count), _max_bit_width(max_bit_width),
                          _stack_size_in_bytes(stack_size*stack_prefix) {
                    for(vbyte i = 0; i < _register_count; i++) _registers[i] = Register(_max_bit_width);
                    if(pow((size_t)2, (size_t)max_bit_width*8) < (mem_size * mem_prefix))
                        throw Exception::EnvironmentException::MemorySizeInvalid(max_bit_width, mem_size, mem_prefix);
                }

                ~VEInternal() {
                    delete [] _registers;
                }
            };

            struct ProgramInternal {

                vbyte* _exec;
                size_t _size;
                size_t _required_memory_size;
                Register _counter;
                Register _stack;

                ProgramInternal(size_t size, vbyte exec[], size_t required_memory_size)
                        : _size(size), _required_memory_size(required_memory_size) {
                    _exec = new vbyte[_size];
                    for(size_t i = 0; i < _size; i++) _exec[i] = exec[i];
                }

                ~ProgramInternal() {
                    delete [] _exec;
                }

                Register &getRegister(VirtualEnvironment &ve, vbyte id) {
                    switch(id) {
                        case (vbyte)SWM_REG_STACK: return _stack;
                        case (vbyte)SWM_REG_COUNTER: return _counter;
                        default: return ve.getRegister(id);
                    }
                }

            };
        }

    }
}