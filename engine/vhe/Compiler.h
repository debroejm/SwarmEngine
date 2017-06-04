#pragma once

#include "VHEInternal.h"
#include "BytecodeDefines.h"

#include <bitset>

namespace Swarm {
    namespace VHE {
        namespace Compiler {

            struct CompilerCommand {
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

            Environment::Program compileCommandList(const CCList &cmds, size_t required_memory_size);

            struct CCNOP : public CompilerCommand {
                virtual void compile(vbyte* result, size_t pos) const { result[pos] = command(); }
                virtual size_t size() const { return 1; }
                virtual vbyte command() const { return CMD_NOP; }
                virtual std::string name() const { return "NOP"; }
            };

            struct CCHalt : public CompilerCommand {
                virtual void compile(vbyte* result, size_t pos) const { result[pos] = command(); }
                virtual size_t size() const { return 1; }
                virtual vbyte command() const { return CMD_HALT; }
                virtual std::string name() const { return "HALT"; }
            };

            struct CCMoveToRegister : public CompilerCommand {
                vbyte _target_register;
                vbyte _mem_address_register;
                BitWidth _width;
                CCMoveToRegister(vbyte target_register, vbyte mem_address_register, BitWidth width)
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
                    return CompilerCommand::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", AddressRegister=" + std::to_string(_mem_address_register);
                }
            };

            struct CCMoveToRegisterConstant : public CompilerCommand {
                vbyte _target_register;
                VariableValue _mem_address;
                BitWidth _width;
                CCMoveToRegisterConstant(vbyte target_register, VariableValue mem_address, BitWidth width)
                        : _target_register(target_register), _mem_address(mem_address), _width(width) {}
                CCMoveToRegisterConstant(vbyte target_register, int64_t mem_address, BitWidth address_width, BitWidth val_width)
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
                    return CompilerCommand::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", Address=" + std::to_string(_mem_address);
                }
            };

            struct CCMoveToMemory : public CompilerCommand {
                vbyte _target_register;
                vbyte _mem_address_register;
                BitWidth _width;
                CCMoveToMemory(vbyte target_register, vbyte mem_address_register, BitWidth width)
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
                    return CompilerCommand::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", AddressRegister=" + std::to_string(_mem_address_register);
                }
            };

            struct CCMoveToMemoryConstant : public CompilerCommand {
                vbyte _target_register;
                VariableValue _mem_address;
                BitWidth _width;
                CCMoveToMemoryConstant(vbyte target_register, VariableValue mem_address, BitWidth width)
                        : _target_register(target_register), _mem_address(mem_address), _width(width) {}
                CCMoveToMemoryConstant(vbyte target_register, int64_t mem_address, BitWidth address_width, BitWidth val_width)
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
                    return CompilerCommand::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", Address=" + std::to_string(_mem_address);
                }
            };

            struct CCLoadConstant : public CompilerCommand {
                vbyte _target_register;
                VariableValue _value;
                CCLoadConstant(vbyte target_register, VariableValue value)
                        : _target_register(target_register), _value(value) {}
                CCLoadConstant(vbyte target_register, int64_t value, BitWidth width)
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
                    return CompilerCommand::to_string()
                           + " TargetRegister=" + std::to_string(_target_register)
                           + ", Value=" + std::to_string(_value);
                }
            };

            struct CCCopyRegister : public CompilerCommand {
                vbyte _from_register;
                vbyte _to_register;
                CCCopyRegister(vbyte from_register, vbyte to_register)
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
                    return CompilerCommand::to_string()
                           + " FromRegister=" + std::to_string(_from_register)
                           + ", ToRegister=" + std::to_string(_to_register);
                }
            };

            struct CCALUDoubleOperation : public CompilerCommand {
                vbyte _in_register_a;
                vbyte _in_register_b;
                vbyte _out_register;
                CCALUDoubleOperation(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : _in_register_a(in_register_a), _in_register_b(in_register_b), _out_register(out_register) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _in_register_a;
                    result[pos + 2] = _in_register_b;
                    result[pos + 3] = _out_register;
                }
                virtual size_t size() const { return 4; }
                virtual std::string to_string() const {
                    return CompilerCommand::to_string()
                           + " RegisterInA=" + std::to_string(_in_register_a)
                           + ", RegisterInB=" + std::to_string(_in_register_b)
                           + ", RegisterOut=" + std::to_string(_out_register);
                }
            };

            struct CCALUAddition : public CCALUDoubleOperation {
                virtual vbyte command() const { return CMD_ALU_ADD; }
                virtual std::string name() const { return "ALU_ADD"; }
                CCALUAddition(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : CCALUDoubleOperation(in_register_a, in_register_b, out_register) {}
            };

            struct CCALUSubtraction : public CCALUDoubleOperation {
                virtual vbyte command() const { return CMD_ALU_SUB; }
                virtual std::string name() const { return "ALU_SUB"; }
                CCALUSubtraction(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : CCALUDoubleOperation(in_register_a, in_register_b, out_register) {}
            };

            struct CCALUMultiplication : public CCALUDoubleOperation {
                virtual vbyte command() const { return CMD_ALU_MULT; }
                virtual std::string name() const { return "ALU_MULT"; }
                CCALUMultiplication(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : CCALUDoubleOperation(in_register_a, in_register_b, out_register) {}
            };

            struct CCALUDivision : public CCALUDoubleOperation {
                virtual vbyte command() const { return CMD_ALU_DIV; }
                virtual std::string name() const { return "ALU_DIV"; }
                CCALUDivision(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : CCALUDoubleOperation(in_register_a, in_register_b, out_register) {}
            };

            struct CCALUModulus : public CCALUDoubleOperation {
                virtual vbyte command() const { return CMD_ALU_MOD; }
                virtual std::string name() const { return "ALU_MOD"; }
                CCALUModulus(vbyte in_register_a, vbyte in_register_b, vbyte out_register)
                        : CCALUDoubleOperation(in_register_a, in_register_b, out_register) {}
            };

            struct CCALUConstantOperation : public CompilerCommand {
                vbyte _in_register;
                vbyte _out_register;
                VariableValue _value;
                CCALUConstantOperation(vbyte in_register, vbyte out_register, VariableValue value)
                        : _in_register(in_register), _out_register(out_register), _value(value) {}
                CCALUConstantOperation(vbyte in_register, vbyte out_register, int64_t value, BitWidth width)
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
                    return CompilerCommand::to_string()
                           + " RegisterIn=" + std::to_string(_in_register)
                           + ", Value=" + std::to_string(_value)
                           + ", RegisterOut=" + std::to_string(_out_register);
                }
            };

            struct CCALUConstantAddition : public CCALUConstantOperation {
                virtual vbyte command() const { return (vbyte) (CMD_ALU_ADD_CONST | widthFlag(_value._width)); }
                virtual std::string name() const { return "ALU_ADD_CONST"; }
                CCALUConstantAddition(vbyte in_register, vbyte out_register, VariableValue value)
                        : CCALUConstantOperation(in_register, out_register, value) {}
                CCALUConstantAddition(vbyte in_register, vbyte out_register, int64_t value, BitWidth width)
                        : CCALUConstantOperation(in_register, out_register, value, width) {}
            };

            struct CCALUConstantSubtraction : public CCALUConstantOperation {
                const bool _lhs;
                virtual vbyte command() const { return (vbyte)((_lhs ? CMD_ALU_SUB_CONST_LHS : CMD_ALU_SUB_CONST_RHS) | widthFlag(_value._width)); }
                virtual std::string name() const { return _lhs ? "ALU_SUB_CONST_LHS" : "ALU_SUB_CONST_RHS"; }
                CCALUConstantSubtraction(vbyte in_register, vbyte out_register, VariableValue value, bool lhs = false)
                        : CCALUConstantOperation(in_register, out_register, value), _lhs(lhs) {}
                CCALUConstantSubtraction(vbyte in_register, vbyte out_register, int64_t value, BitWidth width, bool lhs = false)
                        : CCALUConstantOperation(in_register, out_register, value, width), _lhs(lhs) {}
            };

            struct CCALUConstantMultiplication : public CCALUConstantOperation {
                virtual vbyte command() const { return (vbyte) (CMD_ALU_MULT_CONST | widthFlag(_value._width)); }
                virtual std::string name() const { return "ALU_MULT_CONST"; }
                CCALUConstantMultiplication(vbyte in_register, vbyte out_register, VariableValue value)
                        : CCALUConstantOperation(in_register, out_register, value) {}
                CCALUConstantMultiplication(vbyte in_register, vbyte out_register, int64_t value, BitWidth width)
                        : CCALUConstantOperation(in_register, out_register, value, width) {}
            };

            struct CCALUConstantDivision : public CCALUConstantOperation {
                const bool _lhs;
                virtual vbyte command() const { return (vbyte)((_lhs ? CMD_ALU_DIV_CONST_LHS : CMD_ALU_DIV_CONST_RHS) | widthFlag(_value._width)); }
                virtual std::string name() const { return _lhs ? "ALU_DIV_CONST_LHS" : "ALU_DIV_CONST_RHS"; }
                CCALUConstantDivision(vbyte in_register, vbyte out_register, VariableValue value, bool lhs = false)
                        : CCALUConstantOperation(in_register, out_register, value), _lhs(lhs) {}
                CCALUConstantDivision(vbyte in_register, vbyte out_register, int64_t value, BitWidth width, bool lhs = false)
                        : CCALUConstantOperation(in_register, out_register, value, width), _lhs(lhs) {}
            };

            struct CCALUConstantModulus : public CCALUConstantOperation {
                const bool _lhs;
                virtual vbyte command() const { return (vbyte)((_lhs ? CMD_ALU_MOD_CONST_LHS : CMD_ALU_MOD_CONST_RHS) | widthFlag(_value._width)); }
                virtual std::string name() const { return _lhs ? "ALU_MOD_CONST_LHS" : "ALU_MOD_CONST_RHS"; }
                CCALUConstantModulus(vbyte in_register, vbyte out_register, VariableValue value, bool lhs = false)
                        : CCALUConstantOperation(in_register, out_register, value), _lhs(lhs) {}
                CCALUConstantModulus(vbyte in_register, vbyte out_register, int64_t value, BitWidth width, bool lhs = false)
                        : CCALUConstantOperation(in_register, out_register, value, width), _lhs(lhs) {}
            };

            struct CCALUSingleOperation : public CompilerCommand {
                vbyte _register;
                CCALUSingleOperation(vbyte in_register)
                        : _register(in_register) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _register;
                }
                virtual size_t size() const { return 2; }
                virtual std::string to_string() const {
                    return CompilerCommand::to_string()
                           + " TargetRegister=" + std::to_string(_register);
                }
            };

            struct CCALUInversion : public CCALUSingleOperation {
                virtual vbyte command() const { return CMD_ALU_INV; }
                virtual std::string name() const { return "ALU_INV"; }
                CCALUInversion(vbyte in_register) : CCALUSingleOperation(in_register) {}
            };

            struct CCALUIncrement : public CCALUSingleOperation {
                virtual vbyte command() const { return CMD_ALU_INC; }
                virtual std::string name() const { return "ALU_INC"; }
                CCALUIncrement(vbyte in_register) : CCALUSingleOperation(in_register) {}
            };

            struct CCALUDecrement : public CCALUSingleOperation {
                virtual vbyte command() const { return CMD_ALU_DEC; }
                virtual std::string name() const { return "ALU_DEC"; }
                CCALUDecrement(vbyte in_register) : CCALUSingleOperation(in_register) {}
            };

            struct CCALUMoveOperation : public CompilerCommand {
                vbyte _in_register;
                vbyte _out_register;
                CCALUMoveOperation(vbyte in_register, vbyte out_register)
                        : _in_register(in_register), _out_register(out_register) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    result[pos + 1] = _in_register;
                    result[pos + 2] = _out_register;
                }
                virtual size_t size() const { return 3; }
                virtual std::string to_string() const {
                    return CompilerCommand::to_string()
                           + " InRegister=" + std::to_string(_in_register)
                           + ", OutRegister=" + std::to_string(_out_register);
                }
            };

            struct CCALUMoveInversion : public CCALUMoveOperation {
                virtual vbyte command() const { return CMD_ALU_INV_MV; }
                virtual std::string name() const { return "ALU_INV_MV"; }
                CCALUMoveInversion(vbyte in_register, vbyte out_register) : CCALUMoveOperation(in_register, out_register) {}
            };

            struct CCALUMoveIncrement : public CCALUMoveOperation {
                virtual vbyte command() const { return CMD_ALU_INC_MV; }
                virtual std::string name() const { return "ALU_INC_MV"; }
                CCALUMoveIncrement(vbyte in_register, vbyte out_register) : CCALUMoveOperation(in_register, out_register) {}
            };

            struct CCALUMoveDecrement : public CCALUMoveOperation {
                virtual vbyte command() const { return CMD_ALU_DEC_MV; }
                virtual std::string name() const { return "ALU_DEC_MV"; }
                CCALUMoveDecrement(vbyte in_register, vbyte out_register) : CCALUMoveOperation(in_register, out_register) {}
            };

            class LabelMap {
            protected:
                typedef struct { vbyte* pos; size_t offset; BitWidth width; } CacheStruct;
                typedef std::unordered_map<std::string, size_t> InternLabelMap;
                typedef std::unordered_multimap<std::string, CacheStruct> InternLabelMultimap;
                InternLabelMap _map;
                InternLabelMap _uniques;
                InternLabelMultimap _cache;

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
                    std::pair<InternLabelMultimap::iterator, InternLabelMultimap::iterator> range = _cache.equal_range(label);
                    InternLabelMultimap::iterator it = range.first;
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

            struct CCLabel : public CompilerCommand {
                LabelMap &_map;
                std::string _label;
                CCLabel(LabelMap &map, const std::string &label) : _map(map), _label(label) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    _map.insert(_label, pos);
                }
                virtual size_t size() const { return 0; }
                virtual vbyte command() const { return CMD_NOP; }
                virtual std::string name() const { return "LABEL"; }
                virtual std::string to_string() const { return "[LABEL] " + _label; }
            };

            struct CCJumpOperation : public CompilerCommand {
                LabelMap &_map;
                std::string _label;
                virtual size_t addressOffset() const = 0;
                CCJumpOperation(LabelMap &map, const std::string &label) : _map(map), _label(label) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    result[pos + 0] = command();
                    _map.setIndex(_label, result, pos + addressOffset(), BIT_64);
                }
                virtual size_t size() const { return (size_t)(addressOffset()+BIT_64); }
                virtual std::string to_string() const {
                    return CompilerCommand::to_string()
                           + " Label=" + _label;
                }
            };

            struct CCJump : public CCJumpOperation {
                virtual size_t addressOffset() const { return 1; }
                virtual vbyte command() const { return (vbyte) (CMD_JMP | widthFlag(BIT_64)); }
                virtual std::string name() const { return "JMP"; }
                CCJump(LabelMap &map, const std::string &label)
                        : CCJumpOperation(map, label) {}
            };

            struct CCJumpEquals : public CCJumpOperation {
                vbyte _register_a;
                vbyte _register_b;
                virtual size_t addressOffset() const { return 3; }
                virtual vbyte command() const { return (vbyte) (CMD_JMP_EQL | widthFlag(BIT_64)); }
                virtual std::string name() const { return "JMP_EQL"; }
                CCJumpEquals(LabelMap &map, const std::string &label, vbyte register_a, vbyte register_b)
                        : CCJumpOperation(map, label), _register_a(register_a), _register_b(register_b) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    CCJumpOperation::compile(result, pos);
                    result[pos + 1] = _register_a;
                    result[pos + 2] = _register_b;
                }
                virtual std::string to_string() const {
                    return CompilerCommand::to_string()
                           + ", RegisterA=" + std::to_string(_register_a)
                           + ", RegisterB=" + std::to_string(_register_b);
                }
            };

            struct CCJumpNotEquals : public CCJumpOperation {
                vbyte _register_a;
                vbyte _register_b;
                virtual size_t addressOffset() const { return 3; }
                virtual vbyte command() const { return (vbyte) (CMD_JMP_NEQL | widthFlag(BIT_64)); }
                virtual std::string name() const { return "JMP_NEQL"; }
                CCJumpNotEquals(LabelMap &map, const std::string &label, vbyte register_a, vbyte register_b)
                        : CCJumpOperation(map, label), _register_a(register_a), _register_b(register_b) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    CCJumpOperation::compile(result, pos);
                    result[pos + 1] = _register_a;
                    result[pos + 2] = _register_b;
                }
                virtual std::string to_string() const {
                    return CompilerCommand::to_string()
                           + ", RegisterA=" + std::to_string(_register_a)
                           + ", RegisterB=" + std::to_string(_register_b);
                }
            };

            struct CCJumpLessThan : public CCJumpOperation {
                vbyte _register_a;
                vbyte _register_b;
                virtual size_t addressOffset() const { return 3; }
                virtual vbyte command() const { return (vbyte) (CMD_JMP_LESS | widthFlag(BIT_64)); }
                virtual std::string name() const { return "JMP_LESS"; }
                CCJumpLessThan(LabelMap &map, const std::string &label, vbyte register_a, vbyte register_b)
                        : CCJumpOperation(map, label), _register_a(register_a), _register_b(register_b) {}
                virtual void compile(vbyte* result, size_t pos) const {
                    CCJumpOperation::compile(result, pos);
                    result[pos + 1] = _register_a;
                    result[pos + 2] = _register_b;
                }
                virtual std::string to_string() const {
                    return CompilerCommand::to_string()
                           + ", RegisterA=" + std::to_string(_register_a)
                           + ", RegisterB=" + std::to_string(_register_b);
                }
            };

        }
    }
}