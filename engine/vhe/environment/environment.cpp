#include "../VHEInternal.h"

#include <bitset>

namespace Swarm {
    namespace VHE {
        namespace Environment {

            std::set<VEInternal*> _static_registered_ves;

            void VirtualEnvironment::cleanup() {
                for(VEInternal* ve : _static_registered_ves)
                    delete ve;
                _static_registered_ves.clear();
            }

            VirtualEnvironment::VirtualEnvironment(BitWidth max_bit_width, vbyte register_count,
                                                   size_t mem_size, MemoryPrefix mem_prefix,
                                                   size_t stack_size, MemoryPrefix stack_prefix) {
                _ve = new VEInternal(max_bit_width, register_count, mem_size, mem_prefix, stack_size, stack_prefix);
                _static_registered_ves.insert(_ve);
            }

            Register &VirtualEnvironment::getRegister(vbyte id) { return _ve->_registers[ id % _ve->_register_count ]; }
            Memory &VirtualEnvironment::memory() { return _ve->_memory; }
            size_t VirtualEnvironment::stackSizeInBytes() const { return _ve->_stack_size_in_bytes; }
            BitWidth VirtualEnvironment::maxBitWidth() const { return _ve->_max_bit_width; }

            std::string VirtualEnvironment::printRegisters() const {
                std::string result("");
                for(vbyte i = 0; i < _ve->_register_count; i++)
                    result += std::to_string(i) + ":\t[" + std::to_string(_ve->_registers[i]._data.get()) + "]\n";
                return result;
            }

            std::string VirtualEnvironment::printMemory() const {
                std::string result("");
                size_t memSize = _ve->_memory.sizeInBytes();
                for(size_t i = 0; i < memSize;) {
                    for(int j = 0; j < _ve->_max_bit_width; j++)
                        if(i < memSize) result += std::bitset<8>(_ve->_memory._memory->_data[i++]).to_string() + "   ";
                    result += "\n";
                }
                return result;
            }

        }
    }
}