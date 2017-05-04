#pragma once

#include "VHEInternal.h"
#include "BytecodeDefines.h"

#include <bitset>

namespace Swarm {
    namespace VHE {
        namespace Compiler {

            struct compiler_command {
                virtual void compile(vbyte* result, size_t pos) const = 0;
                virtual size_t size() const = 0;
                virtual vbyte command() const = 0;
                virtual std::string name() const = 0;
                virtual std::string to_string() const {
                    return "[" + std::bitset<8>(command()).to_string() + "][" + name() + "]";
                };
            protected:
                static vbyte widthFlag(BitWidth width, bool shifted = false) {
                    if(shifted) {
                        switch (width) {
                            default:
                            case BIT_8: return CMD_PRECISION_1B_S2;
                            case BIT_16: return CMD_PRECISION_2B_S2;
                            case BIT_32: return CMD_PRECISION_4B_S2;
                            case BIT_64: return CMD_PRECISION_8B_S2;
                        }
                    } else {
                        switch (width) {
                            default:
                            case BIT_8: return CMD_PRECISION_1B;
                            case BIT_16: return CMD_PRECISION_2B;
                            case BIT_32: return CMD_PRECISION_4B;
                            case BIT_64: return CMD_PRECISION_8B;
                        }
                    }
                }
            };

            Environment::Program compileCommandList(const cc_list &cmds, size_t required_memory_size);

            struct cc_nop : public compiler_command {
                virtual void compile(vbyte* result, size_t pos) const { result[pos] = command(); }
                virtual size_t size() const { return 1; }
                virtual vbyte command() const { return CMD_NOP; }
                virtual std::string name() const { return "NOP"; }
            };

            struct cc_halt : public compiler_command {
                virtual void compile(vbyte* result, size_t pos) const { result[pos] = command(); }
                virtual size_t size() const { return 1; }
                virtual vbyte command() const { return CMD_HALT; }
                virtual std::string name() const { return "HALT"; }
            };

            struct cc_move_to_register : public compiler_command {
                vbyte _target_register;
                vbyte _mem_address_register;
                BitWidth _width;
                cc_move_to_register(vbyte target_register, vbyte mem_address_register, BitWidth width)
                        : _target_register(target_register), _mem_address_register(mem_address_register), _width(width) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _target_register;
                    result[pos + 2] = _mem_address_register;
                }
                virtual size_t size() const { return 3; }
                virtual vbyte command() const { return (vbyte)(CMD_MVTOREG | widthFlag(_width)); }
                virtual std::string name() const { return "MVTOREG"; }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", AddressRegister=" + std::to_string(_mem_address_register);
                }
            };

            struct cc_move_to_register_constant : public compiler_command {
                vbyte _target_register;
                VariableValue _mem_address;
                BitWidth _width;
                cc_move_to_register_constant(vbyte target_register, VariableValue mem_address, BitWidth width)
                        : _target_register(target_register), _mem_address(mem_address), _width(width) {}
                cc_move_to_register_constant(vbyte target_register, int64_t mem_address, BitWidth address_width, BitWidth val_width)
                        : _target_register(target_register), _mem_address(mem_address, address_width), _width(val_width) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _target_register;
                    for(unsigned short i = 0; i < _mem_address._width; i++) {
                        switch(_mem_address._width) {
                            default:
                            case BIT_8:  result[pos + 2 + i] = _mem_address._8.bytes[i]; break;
                            case BIT_16: result[pos + 2 + i] = _mem_address._16.bytes[1-i]; break;
                            case BIT_32: result[pos + 2 + i] = _mem_address._32.bytes[3-i]; break;
                            case BIT_64: result[pos + 2 + i] = _mem_address._64.bytes[7-i]; break;
                        }
                    }
                }
                virtual size_t size() const { return (size_t)(2+_mem_address._width); }
                virtual vbyte command() const { return (vbyte)(CMD_MVTOREG_CONST | widthFlag(_width) | widthFlag(_mem_address._width, true)); }
                virtual std::string name() const { return "MVTOREG_CONST"; }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", Address=" + std::to_string(_mem_address);
                }
            };

            struct cc_move_to_memory : public compiler_command {
                vbyte _target_register;
                vbyte _mem_address_register;
                BitWidth _width;
                cc_move_to_memory(vbyte target_register, vbyte mem_address_register, BitWidth width)
                        : _target_register(target_register), _mem_address_register(mem_address_register), _width(width) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _target_register;
                    result[pos + 2] = _mem_address_register;
                }
                virtual size_t size() const { return 3; }
                virtual vbyte command() const { return (vbyte) (CMD_MVTOMEM | widthFlag(_width)); }
                virtual std::string name() const { return "MVTOMEM"; }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", AddressRegister=" + std::to_string(_mem_address_register);
                }
            };

            struct cc_move_to_memory_constant : public compiler_command {
                vbyte _target_register;
                VariableValue _mem_address;
                BitWidth _width;
                cc_move_to_memory_constant(vbyte target_register, VariableValue mem_address, BitWidth width)
                        : _target_register(target_register), _mem_address(mem_address), _width(width) {}
                cc_move_to_memory_constant(vbyte target_register, int64_t mem_address, BitWidth address_width, BitWidth val_width)
                        : _target_register(target_register), _mem_address(mem_address, address_width), _width(val_width) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _target_register;
                    for(unsigned short i = 0; i < _mem_address._width; i++) {
                        switch(_mem_address._width) {
                            default:
                            case BIT_8:  result[pos + 2 + i] = _mem_address._8.bytes[i]; break;
                            case BIT_16: result[pos + 2 + i] = _mem_address._16.bytes[1-i]; break;
                            case BIT_32: result[pos + 2 + i] = _mem_address._32.bytes[3-i]; break;
                            case BIT_64: result[pos + 2 + i] = _mem_address._64.bytes[7-i]; break;
                        }
                    }
                }
                virtual size_t size() const { return (size_t)(2+_mem_address._width); }
                virtual vbyte command() const { return (vbyte) (CMD_MVTOMEM_CONST | widthFlag(_width) | widthFlag(_mem_address._width, true)); }
                virtual std::string name() const { return "MVTOMEM_CONST"; }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", Address=" + std::to_string(_mem_address);
                }
            };

            struct cc_load_constant : public compiler_command {
                vbyte _target_register;
                VariableValue _value;
                cc_load_constant(vbyte target_register, VariableValue value)
                        : _target_register(target_register), _value(value) {}
                cc_load_constant(vbyte target_register, int64_t value, BitWidth width)
                        : _target_register(target_register), _value(value, width) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _target_register;
                    for(unsigned short i = 0; i < _value._width; i++) {
                        switch(_value._width) {
                            default:
                            case BIT_8:  result[pos + 2 + i] = _value._8.bytes[i]; break;
                            case BIT_16: result[pos + 2 + i] = _value._16.bytes[1-i]; break;
                            case BIT_32: result[pos + 2 + i] = _value._32.bytes[3-i]; break;
                            case BIT_64: result[pos + 2 + i] = _value._64.bytes[7-i]; break;
                        }
                    }
                }
                virtual size_t size() const { return (size_t)(2+_value._width); }
                virtual vbyte command() const { return (vbyte) (CMD_LDCONST | widthFlag(_value._width)); }
                virtual std::string name() const { return "LDCONST"; }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", Value=" + std::to_string(_value);
                }
            };

            struct cc_copy_register : public compiler_command {
                vbyte _from_register;
                vbyte _to_register;
                cc_copy_register(vbyte from_register, vbyte to_register)
                        : _from_register(from_register), _to_register(to_register) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _from_register;
                    result[pos + 2] = _to_register;
                }
                virtual size_t size() const { return 3; }
                virtual vbyte command() const { return CMD_CPREG; }
                virtual std::string name() const { return "CPREG"; }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " FromRegister=" + std::to_string(_from_register)
                           + ", ToRegister=" + std::to_string(_to_register);
                }
            };

            struct cc_alu_double_operation : public compiler_command {
                vbyte _in_register_a;
                vbyte _in_register_b;
                vbyte _out_register;
                cc_alu_double_operation(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : _in_register_a(in_register_a), _in_register_b(in_register_b), _out_register(out_register) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _in_register_a;
                    result[pos + 2] = _in_register_b;
                    result[pos + 3] = _out_register;
                }
                virtual size_t size() const { return 4; }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " RegisterInA=" + std::to_string(_in_register_a)
                           + ", RegisterInB=" + std::to_string(_in_register_b)
                           + ", RegisterOut=" + std::to_string(_out_register);
                }
            };

            struct cc_alu_addition : public cc_alu_double_operation {
                virtual vbyte command() const { return CMD_ALU_ADD; }
                virtual std::string name() const { return "ALU_ADD"; }
                cc_alu_addition(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : cc_alu_double_operation(in_register_a, in_register_b, out_register) {}
            };

            struct cc_alu_subtraction : public cc_alu_double_operation {
                virtual vbyte command() const { return CMD_ALU_SUB; }
                virtual std::string name() const { return "ALU_SUB"; }
                cc_alu_subtraction(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : cc_alu_double_operation(in_register_a, in_register_b, out_register) {}
            };

            struct cc_alu_multiplication : public cc_alu_double_operation {
                virtual vbyte command() const { return CMD_ALU_MULT; }
                virtual std::string name() const { return "ALU_MULT"; }
                cc_alu_multiplication(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : cc_alu_double_operation(in_register_a, in_register_b, out_register) {}
            };

            struct cc_alu_division : public cc_alu_double_operation {
                virtual vbyte command() const { return CMD_ALU_DIV; }
                virtual std::string name() const { return "ALU_DIV"; }
                cc_alu_division(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : cc_alu_double_operation(in_register_a, in_register_b, out_register) {}
            };

            struct cc_alu_modulus : public cc_alu_double_operation {
                virtual vbyte command() const { return CMD_ALU_MOD; }
                virtual std::string name() const { return "ALU_MOD"; }
                cc_alu_modulus(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : cc_alu_double_operation(in_register_a, in_register_b, out_register) {}
            };

            struct cc_alu_const_operation : public compiler_command {
                vbyte _in_register;
                vbyte _out_register;
                VariableValue _value;
                cc_alu_const_operation(vbyte in_register, vbyte out_register, VariableValue value)
                        : _in_register(in_register), _out_register(out_register), _value(value) {}
                cc_alu_const_operation(vbyte in_register, vbyte out_register, int64_t value, BitWidth width)
                        : _in_register(in_register), _out_register(out_register), _value(value, width) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _in_register;
                    result[pos + 2] = _out_register;
                    for(unsigned short i = 0; i < _value._width; i++) {
                        switch(_value._width) {
                            default:
                            case BIT_8:  result[pos + 3 + i] = _value._8.bytes[i]; break;
                            case BIT_16: result[pos + 3 + i] = _value._16.bytes[1-i]; break;
                            case BIT_32: result[pos + 3 + i] = _value._32.bytes[3-i]; break;
                            case BIT_64: result[pos + 3 + i] = _value._64.bytes[7-i]; break;
                        }
                    }
                }
                virtual size_t size() const { return (size_t)(3+_value._width); }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " RegisterIn=" + std::to_string(_in_register)
                           + ", Value=" + std::to_string(_value)
                           + ", RegisterOut=" + std::to_string(_out_register);
                }
            };

            struct cc_alu_const_add : public cc_alu_const_operation {
                virtual vbyte command() const { return (vbyte) (CMD_ALU_ADD_CONST | widthFlag(_value._width)); }
                virtual std::string name() const { return "ALU_ADD_CONST"; }
                cc_alu_const_add(vbyte in_register, vbyte out_register, VariableValue value)
                        : cc_alu_const_operation(in_register, out_register, value) {}
                cc_alu_const_add(vbyte in_register, vbyte out_register, int64_t value, BitWidth width)
                        : cc_alu_const_operation(in_register, out_register, value, width) {}
            };

            struct cc_alu_const_subtract : public cc_alu_const_operation {
                const bool _lhs;
                virtual vbyte command() const { return (vbyte)((_lhs ? CMD_ALU_SUB_CONST_LHS : CMD_ALU_SUB_CONST_RHS) | widthFlag(_value._width)); }
                virtual std::string name() const { return _lhs ? "ALU_SUB_CONST_LHS" : "ALU_SUB_CONST_RHS"; }
                cc_alu_const_subtract(vbyte in_register, vbyte out_register, VariableValue value, bool lhs = false)
                        : cc_alu_const_operation(in_register, out_register, value), _lhs(lhs) {}
                cc_alu_const_subtract(vbyte in_register, vbyte out_register, int64_t value, BitWidth width, bool lhs = false)
                        : cc_alu_const_operation(in_register, out_register, value, width), _lhs(lhs) {}
            };

            struct cc_alu_const_multiply : public cc_alu_const_operation {
                virtual vbyte command() const { return (vbyte) (CMD_ALU_MULT_CONST | widthFlag(_value._width)); }
                virtual std::string name() const { return "ALU_MULT_CONST"; }
                cc_alu_const_multiply(vbyte in_register, vbyte out_register, VariableValue value)
                        : cc_alu_const_operation(in_register, out_register, value) {}
                cc_alu_const_multiply(vbyte in_register, vbyte out_register, int64_t value, BitWidth width)
                        : cc_alu_const_operation(in_register, out_register, value, width) {}
            };

            struct cc_alu_const_division : public cc_alu_const_operation {
                const bool _lhs;
                virtual vbyte command() const { return (vbyte)((_lhs ? CMD_ALU_DIV_CONST_LHS : CMD_ALU_DIV_CONST_RHS) | widthFlag(_value._width)); }
                virtual std::string name() const { return _lhs ? "ALU_DIV_CONST_LHS" : "ALU_DIV_CONST_RHS"; }
                cc_alu_const_division(vbyte in_register, vbyte out_register, VariableValue value, bool lhs = false)
                        : cc_alu_const_operation(in_register, out_register, value), _lhs(lhs) {}
                cc_alu_const_division(vbyte in_register, vbyte out_register, int64_t value, BitWidth width, bool lhs = false)
                        : cc_alu_const_operation(in_register, out_register, value, width), _lhs(lhs) {}
            };

            struct cc_alu_const_modulus : public cc_alu_const_operation {
                const bool _lhs;
                virtual vbyte command() const { return (vbyte)((_lhs ? CMD_ALU_MOD_CONST_LHS : CMD_ALU_MOD_CONST_RHS) | widthFlag(_value._width)); }
                virtual std::string name() const { return _lhs ? "ALU_MOD_CONST_LHS" : "ALU_MOD_CONST_RHS"; }
                cc_alu_const_modulus(vbyte in_register, vbyte out_register, VariableValue value, bool lhs = false)
                        : cc_alu_const_operation(in_register, out_register, value), _lhs(lhs) {}
                cc_alu_const_modulus(vbyte in_register, vbyte out_register, int64_t value, BitWidth width, bool lhs = false)
                        : cc_alu_const_operation(in_register, out_register, value, width), _lhs(lhs) {}
            };

            struct cc_alu_single_operation : public compiler_command {
                vbyte _register;
                cc_alu_single_operation(vbyte in_register)
                        : _register(in_register) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _register;
                }
                virtual size_t size() const { return 2; }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " TargetRegister=" + std::to_string(_register);
                }
            };

            struct cc_alu_inversion : public cc_alu_single_operation {
                virtual vbyte command() const { return CMD_ALU_INV; }
                virtual std::string name() const { return "ALU_INV"; }
                cc_alu_inversion(vbyte in_register) : cc_alu_single_operation(in_register) {}
            };

            struct cc_alu_increment : public cc_alu_single_operation {
                virtual vbyte command() const { return CMD_ALU_INC; }
                virtual std::string name() const { return "ALU_INC"; }
                cc_alu_increment(vbyte in_register) : cc_alu_single_operation(in_register) {}
            };

            struct cc_alu_decrement : public cc_alu_single_operation {
                virtual vbyte command() const { return CMD_ALU_DEC; }
                virtual std::string name() const { return "ALU_DEC"; }
                cc_alu_decrement(vbyte in_register) : cc_alu_single_operation(in_register) {}
            };

            struct cc_alu_move_operation : public compiler_command {
                vbyte _in_register;
                vbyte _out_register;
                cc_alu_move_operation(vbyte in_register, vbyte out_register)
                        : _in_register(in_register), _out_register(out_register) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _in_register;
                    result[pos + 2] = _out_register;
                }
                virtual size_t size() const { return 3; }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " InRegister=" + std::to_string(_in_register)
                           + ", OutRegister=" + std::to_string(_out_register);
                }
            };

            struct cc_alu_move_inversion : public cc_alu_move_operation {
                virtual vbyte command() const { return CMD_ALU_INV_MV; }
                virtual std::string name() const { return "ALU_INV_MV"; }
                cc_alu_move_inversion(vbyte in_register, vbyte out_register) : cc_alu_move_operation(in_register, out_register) {}
            };

            struct cc_alu_move_increment : public cc_alu_move_operation {
                virtual vbyte command() const { return CMD_ALU_INC_MV; }
                virtual std::string name() const { return "ALU_INC_MV"; }
                cc_alu_move_increment(vbyte in_register, vbyte out_register) : cc_alu_move_operation(in_register, out_register) {}
            };

            struct cc_alu_move_decrement : public cc_alu_move_operation {
                virtual vbyte command() const { return CMD_ALU_DEC_MV; }
                virtual std::string name() const { return "ALU_DEC_MV"; }
                cc_alu_move_decrement(vbyte in_register, vbyte out_register) : cc_alu_move_operation(in_register, out_register) {}
            };

            class label_map {
            protected:
                typedef struct { vbyte* pos; size_t offset; BitWidth width; } CacheStruct;
                typedef std::unordered_map<std::string, size_t> LabelMap;
                typedef std::unordered_multimap<std::string, CacheStruct> LabelMultimap;
                LabelMap _map;
                LabelMap _uniques;
                LabelMultimap _cache;

                void internSet(size_t index, vbyte* pos, size_t offset, BitWidth width) {
                    VariableValue val(index, width);
                    for(int i = 0; i < width; i++) {
                        switch (width) {
                            default:
                            case BIT_8:  pos[offset + i] = val._8.bytes[i];      break;
                            case BIT_16: pos[offset + i] = val._16.bytes[1 - i]; break;
                            case BIT_32: pos[offset + i] = val._32.bytes[3 - i]; break;
                            case BIT_64: pos[offset + i] = val._64.bytes[7 - i]; break;
                        }
                    }
                }

            public:

                void insert(const std::string &label, size_t index) {
                    if(_map.count(label)) return; // TODO: Throw exception if label exists
                    _map[label] = index;
                    std::pair<LabelMultimap::iterator, LabelMultimap::iterator> range = _cache.equal_range(label);
                    LabelMultimap::iterator it = range.first;
                    while(it != range.second) {
                        internSet(index, it->second.pos, it->second.offset, it->second.width);
                        it++;
                    }
                    _cache.erase(range.first, range.second);
                }

                void setIndex(const std::string &label, vbyte* pos, size_t offset, BitWidth width) {
                    if(_map.count(label)) internSet(_map[label], pos, offset, width);
                    else _cache.insert( {{ label, { pos, offset, width }}} );
                }

                size_t size() const { return _map.size(); }
                size_t cacheSize() const { return _cache.size(); }

                std::string uniqueLabel(const std::string &label) {
                    size_t id = 0;
                    if(_uniques.count(label))
                        id = ++_uniques[label];
                    else
                        _uniques.insert({{ label, 0 }});
                    return label + "_" + std::to_string(id);
                }
            };

            struct cc_label : public compiler_command {
                label_map &_map;
                std::string _label;
                cc_label(label_map &map, const std::string &label) : _map(map), _label(label) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    _map.insert(_label, pos);
                }
                virtual size_t size() const { return 0; }
                virtual vbyte command() const { return CMD_NOP; }
                virtual std::string name() const { return "LABEL"; }
                virtual std::string to_string() const { return "[LABEL] " + _label; }
            };

            struct cc_jump_operation : public compiler_command {
                label_map &_map;
                std::string _label;
                virtual size_t addressOffset() const = 0;
                cc_jump_operation(label_map &map, const std::string &label) : _map(map), _label(label) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    _map.setIndex(_label, result, pos + addressOffset(), BIT_64);
                }
                virtual size_t size() const { return (size_t)(addressOffset()+BIT_64); }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + " Label=" + _label;
                }
            };

            struct cc_jump : public cc_jump_operation {
                virtual size_t addressOffset() const { return 1; }
                virtual vbyte command() const { return (vbyte) (CMD_JMP | widthFlag(BIT_64)); }
                virtual std::string name() const { return "JMP"; }
                cc_jump(label_map &map, const std::string &label)
                        : cc_jump_operation(map, label) {}
            };

            struct cc_jump_equal : public cc_jump_operation {
                vbyte _register_a;
                vbyte _register_b;
                virtual size_t addressOffset() const { return 3; }
                virtual vbyte command() const { return (vbyte) (CMD_JMP_EQL | widthFlag(BIT_64)); }
                virtual std::string name() const { return "JMP_EQL"; }
                cc_jump_equal(label_map &map, const std::string &label, vbyte register_a, vbyte register_b)
                        : cc_jump_operation(map, label), _register_a(register_a), _register_b(register_b) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    cc_jump_operation::compile(result, pos);
                    result[pos + 1] = _register_a;
                    result[pos + 2] = _register_b;
                }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + ", RegisterA=" + std::to_string(_register_a)
                           + ", RegisterB=" + std::to_string(_register_b);
                }
            };

            struct cc_jump_not_equal : public cc_jump_operation {
                vbyte _register_a;
                vbyte _register_b;
                virtual size_t addressOffset() const { return 3; }
                virtual vbyte command() const { return (vbyte) (CMD_JMP_NEQL | widthFlag(BIT_64)); }
                virtual std::string name() const { return "JMP_NEQL"; }
                cc_jump_not_equal(label_map &map, const std::string &label, vbyte register_a, vbyte register_b)
                        : cc_jump_operation(map, label), _register_a(register_a), _register_b(register_b) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    cc_jump_operation::compile(result, pos);
                    result[pos + 1] = _register_a;
                    result[pos + 2] = _register_b;
                }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + ", RegisterA=" + std::to_string(_register_a)
                           + ", RegisterB=" + std::to_string(_register_b);
                }
            };

            struct cc_jump_less : public cc_jump_operation {
                vbyte _register_a;
                vbyte _register_b;
                virtual size_t addressOffset() const { return 3; }
                virtual vbyte command() const { return (vbyte) (CMD_JMP_LESS | widthFlag(BIT_64)); }
                virtual std::string name() const { return "JMP_LESS"; }
                cc_jump_less(label_map &map, const std::string &label, vbyte register_a, vbyte register_b)
                        : cc_jump_operation(map, label), _register_a(register_a), _register_b(register_b) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    cc_jump_operation::compile(result, pos);
                    result[pos + 1] = _register_a;
                    result[pos + 2] = _register_b;
                }
                virtual std::string to_string() const {
                    return compiler_command::to_string()
                           + ", RegisterA=" + std::to_string(_register_a)
                           + ", RegisterB=" + std::to_string(_register_b);
                }
            };

        }
    }
}