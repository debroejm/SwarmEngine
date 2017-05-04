#pragma once



// ***************
//  STD Libraries
// ***************

#include <string>
#include <unordered_map>



// ***************
//  Common Header
// ***************

#include "common_header.h"



// Stack IDs
#define SWM_REG_STACK (vbyte)-1
#define SWM_REG_COUNTER (vbyte)-2

// Return Codes
#define SWM_RET_NO_PROGRAM          -4
#define SWM_RET_SUCCESS             0
#define SWM_RET_HALTED              1
#define SWM_RET_UNEXPECTED_END      -8
#define SWM_RET_UNKNOWN_COMMAND     -2
#define SWM_RET_JUMP_OUT_OF_RANGE   -16



// ***********
//  API Begin
// ***********

namespace Swarm {
    namespace VHE {

        void init();
        void cleanup();

        typedef uint8_t     vbyte;
        typedef int64_t     retcode;

        struct VariableValue {
            BitWidth _width;
            union {
                union {
                    int8_t number;
                    vbyte bytes[1];
                } _8;
                union {
                    int16_t number;
                    vbyte bytes[2];
                } _16;
                union {
                    int32_t number;
                    vbyte bytes[4];
                } _32;
                union {
                    int64_t number;
                    vbyte bytes[8];
                } _64;
            };

            explicit VariableValue(vbyte bytes[], BitWidth width) : _width(width) {
                switch(_width) {
                    case BIT_8:
                        _8.bytes[0] = bytes[0];
                        break;
                    case BIT_16:
                        for(vbyte i = 0; i < 2; i++) _16.bytes[i] = bytes[1-i];
                        break;
                    case BIT_32:
                        for(vbyte i = 0; i < 4; i++) _32.bytes[i] = bytes[3-i];
                        break;
                    case BIT_64:
                        for(vbyte i = 0; i < 8; i++) _64.bytes[i] = bytes[7-i];
                        break;
                }
            }

            VariableValue(int64_t number, BitWidth width) : _width(width) {
                switch(_width) {
                    case BIT_8: _8.number = (int8_t)number; break;
                    case BIT_16: _16.number = (int16_t)number; break;
                    case BIT_32: _32.number = (int32_t)number; break;
                    case BIT_64:  _64.number = (int64_t)number; break;
                }
            }

            VariableValue &operator=(int64_t rhs) {
                switch(_width) {
                    case BIT_8: _8.number = (int8_t)rhs; break;
                    case BIT_16: _16.number = (int16_t)rhs; break;
                    case BIT_32: _32.number = (int32_t)rhs; break;
                    case BIT_64: _64.number = (int64_t)rhs; break;
                }
                return *this;
            }

            VariableValue &operator++() {
                switch(_width) {
                    case BIT_8: ++_8.number; break;
                    case BIT_16: ++_16.number; break;
                    case BIT_32: ++_32.number; break;
                    case BIT_64: ++_64.number; break;
                    default: break;
                }
                return *this;
            }

            VariableValue operator++(int) {
                VariableValue result(*this);
                operator++();
                return result;
            }

            VariableValue &operator--() {
                switch(_width) {
                    case BIT_8: --_8.number; break;
                    case BIT_16: --_16.number; break;
                    case BIT_32: --_32.number; break;
                    case BIT_64: --_64.number; break;
                    default: break;
                }
                return *this;
            }

            VariableValue operator--(int) {
                VariableValue result(*this);
                operator--();
                return result;
            }

            void operator+=(int64_t rhs) { *this = get() + rhs; }
            void operator-=(int64_t rhs) { *this = get() - rhs; }

            int64_t get() const {
                switch(_width) {
                    case BIT_8: return _8.number;
                    case BIT_16: return _16.number;
                    case BIT_32: return _32.number;
                    case BIT_64: return _64.number;
                    default: return 0;
                }
            }

            uint64_t getu() const {
                switch(_width) {
                    case BIT_8: return (uint8_t)_8.number;
                    case BIT_16: return (uint16_t)_16.number;
                    case BIT_32: return (uint32_t)_32.number;
                    case BIT_64: return (uint64_t)_64.number;
                    default: return 0;
                }
            }
        };

    }
}

namespace std {
    inline string to_string(const Swarm::VHE::VariableValue &val) {
        switch(val._width) {
            case Swarm::BIT_8: return "[" + std::to_string(val._8.number) + ":" + std::to_string(val._width) + "]";
            case Swarm::BIT_16: return "[" + std::to_string(val._16.number) + ":" + std::to_string(val._width) + "]";
            case Swarm::BIT_32: return "[" + std::to_string(val._32.number) + ":" + std::to_string(val._width) + "]";
            case Swarm::BIT_64: return "[" + std::to_string(val._64.number) + ":" + std::to_string(val._width) + "]";
            default: return "[?:?]";
        }
    }
}

namespace Swarm {

    namespace Exception {
        class EnvironmentException : public std::runtime_error {
        public:
            enum Type {
                SIZE_INVALID,
                OUT_OF_RANGE,
                OUT_OF_MEMORY
            };

            Type type() { return _type; }

            static EnvironmentException MemorySizeInvalid(BitWidth width, size_t memSize, MemoryPrefix memPrefix) {
                return EnvironmentException(SIZE_INVALID,
                                            "A Bit Width of " + std::to_string(width) + " cannot map a Memory of the size " + std::to_string(memSize) + " " + std::to_string(memPrefix));
            }
            static EnvironmentException MemoryFreeOutOfRange(size_t begin, size_t end, size_t size_in_bytes) {
                return EnvironmentException(OUT_OF_RANGE,
                                            "Attempted to free Memory Chunk of range ["
                                            + std::to_string(begin) + "-" + std::to_string(end)
                                            + "] for Memory of size " + std::to_string(size_in_bytes));
            }
            static EnvironmentException MemoryOutOfSpace(size_t size) {
                return EnvironmentException(OUT_OF_MEMORY,
                                            "Ran out of Memory when attempted to allocate Chunk of size " + std::to_string(size));
            }

        protected:
            EnvironmentException(Type type, const std::string &msg) : _type(type), runtime_error(msg) {}
            Type _type;
        };
    }

    namespace VHE {

        namespace Environment {

            struct MemoryInternal;
            class VirtualEnvironment;
            class Memory {
            public:
                Memory(size_t mem_size, MemoryPrefix prefix = MEM_BYTE);

                vbyte* allocMemChunk(size_t size, size_t *begin, size_t *end);
                void freeMemChunk(size_t begin, size_t end);

                size_t sizeInBytes() const;

                std::string printFreeSectionsChronological() const;
                std::string printFreeSectionsOrdered() const;

                static void cleanup();

            private:
                friend class VirtualEnvironment;
                MemoryInternal* _memory;
            };

            struct VEInternal;
            class Program;
            class VirtualEnvironment {
            public:
                VirtualEnvironment(BitWidth max_bit_width, vbyte register_count,
                                   size_t mem_size, MemoryPrefix mem_prefix,
                                   size_t stack_size, MemoryPrefix stack_prefix);

                Register &getRegister(vbyte id);
                Memory &memory();
                size_t stackSizeInBytes() const;
                BitWidth maxBitWidth() const;

                std::string printRegisters() const;
                std::string printMemory() const;

                static void cleanup();

            private:
                friend class Program;
                VEInternal* _ve;
            };

            struct ProgramInternal;
            class Program {
            public:
                Program(size_t size, vbyte exec[], size_t required_memory_size);
                retcode run(VirtualEnvironment &ve);

                static void cleanup();

            protected:
                retcode run(VirtualEnvironment &ve, vbyte* stack_mem, vbyte* heap_mem, size_t stack_size);

            private:
                ProgramInternal* _program;
            };

            struct Register {
                VariableValue _data;
                BitWidth _width;

                Register() : _data((int64_t)0, BIT_8), _width(BIT_8) {}
                Register(BitWidth width) : _data((int64_t)0, width), _width(width) {}

                void clear() { _data = 0; }

                Register &operator=(const VariableValue &rhs) {
                    _data = rhs.get();
                    return *this;
                }

                Register &operator=(uint64_t rhs) {
                    _data = rhs;
                    return *this;
                }

                Register &operator++() {
                    ++_data;
                    return *this;
                }

                uint64_t operator++(int) {
                    uint64_t result = _data.getu();
                    ++_data;
                    return result;
                }

                Register &operator--() {
                    --_data;
                    return *this;
                }

                uint64_t operator--(int) {
                    uint64_t result = _data.getu();
                    --_data;
                    return result;
                }

                void operator+=(int64_t rhs) { _data += rhs; }
                void operator-=(int64_t rhs) { _data -= rhs; }

                operator uint64_t() { return _data.getu(); }
            };

        }

    }
}