#include "../VHEInternal.h"
#include "../BytecodeDefines.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace VHE {
        namespace Environment {

            std::set<ProgramInternal*> _static_registered_programs;

            void Program::cleanup() {
                for(ProgramInternal* program : _static_registered_programs)
                    delete program;
                _static_registered_programs.clear();
            }

            Program::Program(size_t size, vbyte exec[], size_t required_memory_size) {
                _program = new ProgramInternal(size, exec, required_memory_size);
                _static_registered_programs.insert(_program);
            }

            retcode Program::run(VirtualEnvironment &ve) {
                size_t stack_begin, stack_end;
                vbyte* stack_mem = ve.memory().allocMemChunk( ve.stackSizeInBytes(), &stack_begin, &stack_end );
                size_t heap_begin, heap_end;
                vbyte* heap_mem = ve.memory().allocMemChunk( _program->_required_memory_size, &heap_begin, &heap_end );
                retcode rc = run( ve, stack_mem, heap_mem, ve.stackSizeInBytes() );
                ve.memory().freeMemChunk(stack_begin, stack_end);
                ve.memory().freeMemChunk(heap_begin, heap_end);
                return rc;
            }
            
            retcode Program::run(VirtualEnvironment &ve, vbyte* stack_mem, vbyte* heap_mem, size_t stack_size) {
                if(_program->_exec == nullptr) return SWM_RET_UNEXPECTED_END;

                _program->_counter._data = 0;
                _program->_stack = Register(ve.maxBitWidth());

                log_vhe(INFO) << "Starting VHE Program of size: " << _program->_size << " bytes" << Flush();

                while(_program->_counter <_program->_size) {
                    vbyte cmd = _program->_exec[_program->_counter];
                    //DEBUG_PRINT_CMD(_program->_counter,(int)cmd);

                    // [NOP]
                    if(cmd == CMD_NOP) {
                        ++_program->_counter;
                        continue;
                    }

                    // [HALT]
                    if(cmd == CMD_HALT) return SWM_RET_HALTED;

                    // Register Commands
                    if((cmd & 0b11000000) == 0b11000000) {
                        switch(cmd & 0b00110000) {
                            case 0b000000: // [MVTOREG]
                                if(cmd & 0b00001100) { // Other Register Command
                                    switch(cmd & 0b00001100) {
                                        case 0b0100: { // [LDCONST]

                                            // Get Width of Value from the command
                                            BitWidth width;
                                            switch(cmd & 0b00000011) {
                                                case 0b00: width = BIT_8; break;
                                                case 0b01: width = BIT_16; break;
                                                case 0b10: width = BIT_32; break;
                                                case 0b11: width = BIT_64; break;
                                                default: width = BIT_8; break;
                                            }

                                            //DEBUG_PRINT("LDCONST");

                                            // Check for EOF
                                            if (_program->_size - _program->_counter < 1 + width) return SWM_RET_UNEXPECTED_END;

                                            // Get Register to Load into
                                            Register &reg_out = _program->getRegister(ve, _program->_exec[++_program->_counter]);

                                            // Get Constant Value
                                            vbyte byte_in[width];
                                            for(vbyte i = 0; i < width; i++) byte_in[i] = _program->_exec[++_program->_counter];

                                            // Set Data to Register; use .get() instead of direct assignment to keep original register bitwidth
                                            reg_out._data = VariableValue(byte_in, width).get();
                                        } break;
                                        case 0b1000: { // [CPREG]

                                            // Check for EOF
                                            if (_program->_size - _program->_counter < 2) return SWM_RET_UNEXPECTED_END;

                                            // Get Registers
                                            Register &reg_in = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                            Register &reg_out = _program->getRegister(ve, _program->_exec[++_program->_counter]);

                                            //DEBUG_PRINT("CPREG FromVal=" << reg_in._data.get() << ", ToVal=" << reg_out._data.get());

                                            // Copy Data from first Register to second; use .get() instead of direct assignment to keep original register bitwidth
                                            reg_out._data = reg_in._data.get();
                                            //DEBUG_PRINT("Result=" << reg_out._data.get());
                                        } break;
                                        default: return SWM_RET_UNKNOWN_COMMAND;
                                    }
                                    break;
                                } // Else continue
                            case 0b100000: // [MVTOREG_CONST]
                            {
                                // Get Width of Value from the command
                                BitWidth width;
                                switch(cmd & 0b00000011) {
                                    default:
                                    case 0b00: width = BIT_8; break;
                                    case 0b01: width = BIT_16; break;
                                    case 0b10: width = BIT_32; break;
                                    case 0b11: width = BIT_64; break;
                                }
                                // Get Width of Address from the command; not used for MVTOREG
                                BitWidth width_const;
                                switch(cmd & 0b00001100) {
                                    default:
                                    case 0b0000: width_const = BIT_8; break;
                                    case 0b0100: width_const = BIT_16; break;
                                    case 0b1000: width_const = BIT_32; break;
                                    case 0b1100: width_const = BIT_64; break;
                                }
                                bool flag_const = (cmd & 0b100000) != 0;
                                //DEBUG_PRINT( (flag_const ? "MVTOREG_CONST" : "MVTOREG") );

                                // Check for EOF
                                if ( (_program->_size - _program->_counter) < (1 + (flag_const ? width_const : 1)) ) return SWM_RET_UNEXPECTED_END;

                                // Get the Register to move Data to
                                Register &reg_data = _program->getRegister(ve, _program->_exec[++_program->_counter]);

                                // Get Memory Info
                                size_t mem_pos;
                                vbyte* mem;
                                size_t max_size;
                                if(flag_const) {
                                    vbyte byte_in[width];
                                    for(vbyte i = 0; i < width_const; i++) byte_in[i] = _program->_exec[++_program->_counter];
                                    mem_pos = VariableValue(byte_in, width).getu();
                                    mem = heap_mem;
                                    max_size = _program->_required_memory_size;
                                    //mem = ve.getMemory()._data;
                                    //max_size = ve.getMemory()._size_in_bytes;
                                } else {
                                    Register &reg_pos = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                    mem_pos = reg_pos._data.getu();
                                    if(&reg_pos == &_program->_stack) {
                                        mem = stack_mem;
                                        max_size = stack_size;
                                    } else {
                                        mem = heap_mem;
                                        max_size = _program->_required_memory_size;
                                        //mem = ve.getMemory()._data;
                                        //max_size = ve.getMemory()._size_in_bytes;
                                    }
                                }

                                // Get Memory Data
                                vbyte mem_data[width];
                                for(vbyte i = 0; i < width; i++) {
                                    if(mem_pos+i < max_size) mem_data[i] = mem[mem_pos+i];
                                    else mem_data[i] = 0;
                                }

                                // Set Data to Register; use .get() instead of direct assignment to keep original register bitwidth
                                reg_data._data = VariableValue(mem_data, width).get();
                            } break;
                            case 0b010000: // [MVTOMEM]
                            case 0b110000: // [MVTOMEM_CONST]
                            {
                                // Get Width of Value from the command
                                BitWidth width;
                                switch(cmd & 0b00000011) {
                                    case 0b00: width = BIT_8; break;
                                    case 0b01: width = BIT_16; break;
                                    case 0b10: width = BIT_32; break;
                                    case 0b11: width = BIT_64; break;
                                    default: width = BIT_8; break;
                                }
                                // Get Width of Address from the command; not used for MVTOMEM
                                BitWidth width_const;
                                switch(cmd & 0b00001100) {
                                    default:
                                    case 0b0000: width_const = BIT_8; break;
                                    case 0b0100: width_const = BIT_16; break;
                                    case 0b1000: width_const = BIT_32; break;
                                    case 0b1100: width_const = BIT_64; break;
                                }
                                bool flag_const = (cmd & 0b100000) != 0;
                                //DEBUG_PRINT( (flag_const ? "MVTOMEM_CONST" : "MVTOMEM") );

                                // Check for EOF
                                if ( (_program->_size - _program->_counter) < (1 + (flag_const ? width_const : 1)) ) return SWM_RET_UNEXPECTED_END;

                                // Get the Register to move Data to
                                Register &reg_data = _program->getRegister(ve, _program->_exec[++_program->_counter]);

                                // Get Memory Info
                                size_t mem_pos;
                                vbyte* mem;
                                size_t max_size;
                                if(flag_const) {
                                    vbyte byte_in[width];
                                    for(vbyte i = 0; i < width_const; i++) byte_in[i] = _program->_exec[++_program->_counter];
                                    mem_pos = VariableValue(byte_in, width).getu();
                                    mem = heap_mem;
                                    max_size = _program->_required_memory_size;
                                    //mem = ve.getMemory()._data;
                                    //max_size = ve.getMemory()._size_in_bytes;
                                } else {
                                    Register &reg_pos = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                    mem_pos = reg_pos._data.getu();
                                    if(&reg_pos == &_program->_stack) {
                                        mem = stack_mem;
                                        max_size = stack_size;
                                    } else {
                                        mem = heap_mem;
                                        max_size = _program->_required_memory_size;
                                        //mem = ve.getMemory()._data;
                                        //max_size = ve.getMemory()._size_in_bytes;
                                    }
                                }

                                // Get Register Data
                                vbyte* tdata;
                                switch(reg_data._width) {
                                    default:
                                    case BIT_8:  tdata = &reg_data._data._8 .bytes[0]; break;
                                    case BIT_16: tdata = &reg_data._data._16.bytes[0]; break;
                                    case BIT_32: tdata = &reg_data._data._32.bytes[0]; break;
                                    case BIT_64: tdata = &reg_data._data._64.bytes[0]; break;
                                }

                                // Set to Memory
                                BitWidth least_width = width;
                                if(reg_data._width < least_width) least_width = reg_data._width;
                                for(size_t i = 0; i < least_width; i++) {
                                    if(mem_pos+i < max_size) {
                                        switch(least_width) {
                                            default:
                                            case BIT_8:  mem[mem_pos+i] = tdata[i];   break;
                                            case BIT_16: mem[mem_pos+i] = tdata[1-i]; break;
                                            case BIT_32: mem[mem_pos+i] = tdata[3-i]; break;
                                            case BIT_64: mem[mem_pos+i] = tdata[7-i]; break;
                                        }
                                    }
                                }
                            } break;
                            default: return SWM_RET_UNKNOWN_COMMAND;
                        }
                        _program->_counter++;
                        continue;
                    }

                    // ALU Commands
                    if((cmd & 0b11000000) == 0b01000000) {
                        switch(cmd & 0b00110000) {
                            case 0b000000: { // Basic ALU Commands
                                switch(cmd & 0b00001111) {
                                    case 0b0000: { // [ADD]
                                        //DEBUG_PRINT("ALU_ADD");
                                        if (_program->_size - _program->_counter < 3) return SWM_RET_UNEXPECTED_END;
                                        Register &reg_in_1 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_in_2 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_out  = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        //DEBUG_PRINT("A=" << reg_in_1._data.get() << ", B=" << reg_in_2._data.get());
                                        reg_out._data = reg_in_1._data.get() + reg_in_2._data.get();
                                        //DEBUG_PRINT("C=" << reg_out._data.get());
                                    } break;
                                    case 0b0001: { // [SUB]
                                        //DEBUG_PRINT("ALU_SUB");
                                        if (_program->_size - _program->_counter < 3) return SWM_RET_UNEXPECTED_END;
                                        Register &reg_in_1 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_in_2 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_out  = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        //DEBUG_PRINT("A=" << reg_in_1._data.get() << ", B=" << reg_in_2._data.get());
                                        reg_out._data = reg_in_1._data.get() - reg_in_2._data.get();
                                        //DEBUG_PRINT("C=" << reg_out._data.get());
                                    } break;
                                    case 0b0010: { // [MULT]
                                        //DEBUG_PRINT("ALU_MULT");
                                        if (_program->_size - _program->_counter < 3) return SWM_RET_UNEXPECTED_END;
                                        Register &reg_in_1 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_in_2 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_out  = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        //DEBUG_PRINT("A=" << reg_in_1._data.get() << ", B=" << reg_in_2._data.get());
                                        reg_out._data = reg_in_1._data.get() * reg_in_2._data.get();
                                        //DEBUG_PRINT("C=" << reg_out._data.get());
                                    } break;
                                    case 0b0011: { // [DIV]
                                        //DEBUG_PRINT("ALU_DIV");
                                        if (_program->_size - _program->_counter < 3) return SWM_RET_UNEXPECTED_END;
                                        Register &reg_in_1 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_in_2 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_out  = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        //DEBUG_PRINT("A=" << reg_in_1._data.get() << ", B=" << reg_in_2._data.get());
                                        reg_out._data = reg_in_1._data.get() / reg_in_2._data.get();
                                        //DEBUG_PRINT("C=" << reg_out._data.get());
                                    } break;
                                    case 0b0100: { // [MOD]
                                        //DEBUG_PRINT("ALU_MOD");
                                        if (_program->_size - _program->_counter < 3) return SWM_RET_UNEXPECTED_END;
                                        Register &reg_in_1 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_in_2 = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_out  = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        //DEBUG_PRINT("A=" << reg_in_1._data.get() << ", B=" << reg_in_2._data.get());
                                        reg_out._data = reg_in_1._data.get() % reg_in_2._data.get();
                                        //DEBUG_PRINT("C=" << reg_out._data.get());
                                    } break;
                                    case 0b0101: { // [INV]
                                        //DEBUG_PRINT("ALU_INV");
                                        if (_program->_size - _program->_counter < 1) return SWM_RET_UNEXPECTED_END;
                                        Register &reg = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        reg._data = reg._data.get() * -1;
                                        //DEBUG_PRINT("Result=" << reg._data.get());
                                    } break;
                                    case 0b0110: { // [INC]
                                        //DEBUG_PRINT("ALU_INC");
                                        if (_program->_size - _program->_counter < 1) return SWM_RET_UNEXPECTED_END;
                                        Register &reg = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        reg._data = reg._data.get() + 1;
                                        //DEBUG_PRINT("Result=" << reg._data.get());
                                    } break;
                                    case 0b0111: { // [DEC]
                                        //DEBUG_PRINT("ALU_DEC");
                                        if (_program->_size - _program->_counter < 1) return SWM_RET_UNEXPECTED_END;
                                        Register &reg = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        reg._data = reg._data.get() - 1;
                                        //DEBUG_PRINT("Result=" << reg._data.get());
                                    } break;
                                    default: return SWM_RET_UNKNOWN_COMMAND;
                                }
                            } break;
                            case 0b010000: { // Single Move Commands
                                switch(cmd & 0b00001111) {
                                    case 0b0101: { // [INV_MV]
                                        //DEBUG_PRINT("ALU_INV_MV");
                                        if (_program->_size - _program->_counter < 2) return SWM_RET_UNEXPECTED_END;
                                        Register &reg_in  = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_out = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        reg_out._data = reg_in._data.get() * -1;
                                        //DEBUG_PRINT("Result=" << reg_out._data.get());
                                    } break;
                                    case 0b0110: { // [INC_MV]
                                        //DEBUG_PRINT("ALU_INC_MV");
                                        if (_program->_size - _program->_counter < 2) return SWM_RET_UNEXPECTED_END;
                                        Register &reg_in  = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_out = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        reg_out._data = reg_in._data.get() + 1;
                                        //DEBUG_PRINT("Result=" << reg_out._data.get());
                                    } break;
                                    case 0b0111: { // [DEC_MV]
                                        //DEBUG_PRINT("ALU_DEC_MV");
                                        if (_program->_size - _program->_counter < 2) return SWM_RET_UNEXPECTED_END;
                                        Register &reg_in  = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        Register &reg_out = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                        reg_out._data = reg_in._data.get() - 1;
                                        //DEBUG_PRINT("Result=" << reg_out._data.get());
                                    } break;
                                    default: return SWM_RET_UNKNOWN_COMMAND;
                                }
                            } break;
                            case 0b100000:
                            case 0b110000: // Double Constant Commands
                            {
                                // Get Width of Constant from the command
                                BitWidth width;
                                switch(cmd & 0b00000011) {
                                    default:
                                    case 0b00: width = BIT_8; break;
                                    case 0b01: width = BIT_16; break;
                                    case 0b10: width = BIT_32; break;
                                    case 0b11: width = BIT_64; break;
                                }

                                // Check for EOF
                                if (_program->_size - _program->_counter < (2+width)) return SWM_RET_UNEXPECTED_END;

                                // Get Registers
                                Register &reg_in  = _program->getRegister(ve, _program->_exec[++_program->_counter]);
                                Register &reg_out = _program->getRegister(ve, _program->_exec[++_program->_counter]);

                                // Get Constant Value
                                vbyte byte_in[width];
                                for(vbyte i = 0; i < width; i++) byte_in[i] = _program->_exec[++_program->_counter];
                                VariableValue const_val(byte_in, width);

                                //DEBUG_PRINT("A=" << reg_in._data.get() << ", B=" << const_val.get());

                                // Perform Operation
                                switch(cmd & 0b00011100) {
                                    case 0b00000: // [ADD_CONST]
                                        //DEBUG_PRINT("ALU_ADD_CONST");
                                        reg_out._data = reg_in._data.get() + const_val.get();
                                        break;
                                    case 0b00100: // [SUB_CONST_RHS]
                                        //DEBUG_PRINT("ALU_SUB_CONST_RHS");
                                        reg_out._data = reg_in._data.get() - const_val.get();
                                        break;
                                    case 0b01000: // [SUB_CONST_LHS]
                                        //DEBUG_PRINT("ALU_SUB_CONST_LHS");
                                        reg_out._data = const_val.get() - reg_in._data.get();
                                        break;
                                    case 0b01100: // [MULT_CONST]
                                        //DEBUG_PRINT("ALU_MULT_CONST");
                                        reg_out._data = reg_in._data.get() * const_val.get();
                                        break;
                                    case 0b10000: // [DIV_CONST_RHS]
                                        //DEBUG_PRINT("ALU_DIV_CONST_RHS");
                                        reg_out._data = reg_in._data.get() / const_val.get();
                                        break;
                                    case 0b10100: // [DIV_CONST_LHS]
                                        //DEBUG_PRINT("ALU_DIV_CONST_LHS");
                                        reg_out._data = const_val.get() / reg_in._data.get();
                                        break;
                                    case 0b11000: // [MOD_CONST_RHS]
                                        //DEBUG_PRINT("ALU_MOD_CONST_RHS");
                                        reg_out._data = reg_in._data.get() % const_val.get();
                                        break;
                                    case 0b11100: // [MOD_CONST_LHS]
                                        //DEBUG_PRINT("ALU_MOD_CONST_LHS");
                                        reg_out._data = const_val.get() % reg_in._data.get();
                                        break;
                                    default: return SWM_RET_UNKNOWN_COMMAND;
                                }

                                //DEBUG_PRINT("Result=" << reg_out._data.get());
                            } break;
                            default: return SWM_RET_UNKNOWN_COMMAND;
                        }
                        _program->_counter++;
                        continue;
                    }

                    // JUMP Commands
                    if((cmd & 0b11100000) == 0b00100000) {

                        // Get Width of Constant from the command
                        BitWidth width;
                        switch (cmd & 0b00000011) {
                            default:
                            case 0b00: width = BIT_8; break;
                            case 0b01: width = BIT_16; break;
                            case 0b10: width = BIT_32; break;
                            case 0b11: width = BIT_64; break;
                        }

                        switch(cmd & 0b00011000) {
                            case 0b00000: { // [JMP]
                                //DEBUG_PRINT("JMP");
                                if (_program->_size - _program->_counter < width) return SWM_RET_UNEXPECTED_END;
                                vbyte loc_data[width];
                                for(vbyte i = 0; i < width; i++) loc_data[i] = _program->_exec[++_program->_counter];
                                VariableValue loc_val(loc_data, width);
                                if(cmd & CMD_JUMP_RELATIVE) {
                                    int64_t relative = loc_val.get();
                                    if(_program->_counter + relative < 0) return SWM_RET_JUMP_OUT_OF_RANGE;
                                    if(_program->_counter + relative >=_program->_size) return SWM_RET_JUMP_OUT_OF_RANGE;
                                    _program->_counter += relative;
                                    continue;
                                } else {
                                    uint64_t location = loc_val.getu();
                                    //DEBUG_PRINT("Jump Address: " << location);
                                    if(location >=_program->_size) return SWM_RET_JUMP_OUT_OF_RANGE;
                                    _program->_counter = location;
                                    continue;
                                }
                            } break;
                            case 0b01000: { // [JMP_LESS]
                                //DEBUG_PRINT("JMP_LESS");
                                if (_program->_size - _program->_counter < 2 + width) return SWM_RET_UNEXPECTED_END;
                                vbyte regID1 = _program->_exec[++_program->_counter];
                                vbyte regID2 = _program->_exec[++_program->_counter];
                                //DEBUG_PRINT("Registers: " << (int)regID1 << " : " << (int)regID2);
                                Register &reg1 = _program->getRegister(ve, regID1);
                                Register &reg2 = _program->getRegister(ve, regID2);
                                vbyte loc_data[width];
                                for(vbyte i = 0; i < width; i++) loc_data[i] = _program->_exec[++_program->_counter];
                                VariableValue loc_val(loc_data, width);
                                if(reg1._data.get() < reg2._data.get()) {
                                    if (cmd & CMD_JUMP_RELATIVE) {
                                        int64_t relative = loc_val.get();
                                        if (_program->_counter + relative < 0) return SWM_RET_JUMP_OUT_OF_RANGE;
                                        if (_program->_counter + relative >=_program->_size) return SWM_RET_JUMP_OUT_OF_RANGE;
                                        _program->_counter += relative;
                                        continue;
                                    } else {
                                        uint64_t location = loc_val.getu();
                                        //DEBUG_PRINT("Jump Address: " << location);
                                        if (location >=_program->_size) return SWM_RET_JUMP_OUT_OF_RANGE;
                                        _program->_counter = location;
                                        continue;
                                    }
                                }
                            } break;
                            case 0b10000: { // [JMP_EQL]
                                //DEBUG_PRINT("JMP_EQL");
                                if (_program->_size - _program->_counter < 2 + width) return SWM_RET_UNEXPECTED_END;
                                vbyte regID1 = _program->_exec[++_program->_counter];
                                vbyte regID2 = _program->_exec[++_program->_counter];
                                //DEBUG_PRINT("Registers: " << (int)regID1 << " : " << (int)regID2);
                                Register &reg1 = _program->getRegister(ve, regID1);
                                Register &reg2 = _program->getRegister(ve, regID2);
                                vbyte loc_data[width];
                                for(vbyte i = 0; i < width; i++) loc_data[i] = _program->_exec[++_program->_counter];
                                VariableValue loc_val(loc_data, width);
                                if(reg1._data.get() == reg2._data.get()) {
                                    if (cmd & CMD_JUMP_RELATIVE) {
                                        int64_t relative = loc_val.get();
                                        if (_program->_counter + relative < 0) return SWM_RET_JUMP_OUT_OF_RANGE;
                                        if (_program->_counter + relative >=_program->_size) return SWM_RET_JUMP_OUT_OF_RANGE;
                                        _program->_counter += relative;
                                        continue;
                                    } else {
                                        uint64_t location = loc_val.getu();
                                        //DEBUG_PRINT("Jump Address: " << location);
                                        if (location >=_program->_size) return SWM_RET_JUMP_OUT_OF_RANGE;
                                        _program->_counter = location;
                                        continue;
                                    }
                                }
                            } break;
                            case 0b11000: { // [JMP_NEQL]
                                //DEBUG_PRINT("JMP_NEQL");
                                if (_program->_size - _program->_counter < 2 + width) return SWM_RET_UNEXPECTED_END;
                                vbyte regID1 = _program->_exec[++_program->_counter];
                                vbyte regID2 = _program->_exec[++_program->_counter];
                                //DEBUG_PRINT("Registers: " << (int)regID1 << " : " << (int)regID2);
                                Register &reg1 = _program->getRegister(ve, regID1);
                                Register &reg2 = _program->getRegister(ve, regID2);
                                vbyte loc_data[width];
                                for(vbyte i = 0; i < width; i++) loc_data[i] = _program->_exec[++_program->_counter];
                                VariableValue loc_val(loc_data, width);
                                if(reg1._data.get() != reg2._data.get()) {
                                    if (cmd & CMD_JUMP_RELATIVE) {
                                        int64_t relative = loc_val.get();
                                        if (_program->_counter + relative < 0) return SWM_RET_JUMP_OUT_OF_RANGE;
                                        if (_program->_counter + relative >=_program->_size) return SWM_RET_JUMP_OUT_OF_RANGE;
                                        _program->_counter += relative;
                                        continue;
                                    } else {
                                        uint64_t location = loc_val.getu();
                                        //DEBUG_PRINT("Jump Address: " << location);
                                        if (location >=_program->_size) return SWM_RET_JUMP_OUT_OF_RANGE;
                                        _program->_counter = location;
                                        continue;
                                    }
                                }
                            } break;
                            default: return SWM_RET_UNKNOWN_COMMAND;
                        }
                        _program->_counter++;
                        continue;
                    }

                    // Command Not Known
                    return SWM_RET_UNKNOWN_COMMAND;
                }

                return SWM_RET_SUCCESS;
            }

        }
    }
}