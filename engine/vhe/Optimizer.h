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

#include "Compiler.h"

#include <unordered_set>
#include <stack>

#define SWM_OPT_LABEL_LOOP_BEGIN            "LoopBegin"
#define SWM_OPT_LABEL_LOOP_CHECK            "LoopCondition"
#define SWM_OPT_LABEL_LOOP_END              "LoopEnd"

#define SWM_OPT_LABEL_CONDITIONAL_IF        "ConditionalIf"
#define SWM_OPT_LABEL_CONDITIONAL_ELSE      "ConditionalElse"
#define SWM_OPT_LABEL_CONDITIONAL_END       "ConditionalEnd"

namespace Swarm {
    namespace VHE {
        namespace Optimizer {

            enum ArithmeticOperatorDouble {
                ADDITION,
                SUBTRACTION,
                MULTIPLICATION,
                DIVISION,
                MODULUS
            };
            enum ArithmeticOperatorSingle {
                POSITIVE, // Usually a Positive Unary operation is NOOP
                NEGATIVE,
                INCREMENT,
                DECREMENT
            };
            enum FlowControl {
                BREAK,
                CONTINUE,
                RETURN
            };

        }
    }
}

namespace std {

    using namespace Swarm::VHE::Optimizer;

    inline string to_string(ArithmeticOperatorDouble op) {
        switch(op) {
            case ADDITION: return "+";
            case SUBTRACTION: return "-";
            case MULTIPLICATION: return "*";
            case DIVISION: return "/";
            case MODULUS: return "%";
            default: return "(?)";
        }
    }
    inline string to_string(ArithmeticOperatorSingle op) {
        switch(op) {
            case POSITIVE: return "+";
            case NEGATIVE: return "-";
            case INCREMENT: return "++";
            case DECREMENT: return "--";
            default: return "(?)";
        }
    }
    inline string to_string(FlowControl cntrl) {
        switch(cntrl) {
            case BREAK: return "break";
            case CONTINUE: return "continue";
            case RETURN: return "return";
            default: return "flow:?";
        }
    }
}

namespace Swarm {

    namespace Exception {
        class OptimizeException : public std::runtime_error {
        public:
            enum Type {
                VARIABLE_NOT_FOUND,
                SIZE_INVALID,
                SCOPE_CONTROL,
                OUT_OF_REGISTERS,
                UNKNOWN_COMMAND,
                INVALID_OPERATION,
                MISSING_EXPRESSION
            };

            Type type() { return _type; }

            static OptimizeException VariableNotFound(size_t id) {
                return OptimizeException(VARIABLE_NOT_FOUND,
                                         "The Variable with ID '" + std::to_string(id) + "' has not been created yet");
            }
            static OptimizeException MemorySizeInvalid(BitWidth width, size_t memSize, MemoryPrefix memPrefix) {
                return OptimizeException(SIZE_INVALID,
                                         "A Bit Width of " + std::to_string(width) + " cannot map a Memory of the size " +
                                         std::to_string(memSize) + " " + std::to_string(memPrefix));
            }

            static OptimizeException ScopeControl(const VHE::Optimizer::FlowControl control) {
                return OptimizeException(SCOPE_CONTROL,
                                         "Flow control '" + std::to_string(control) + "' not valid for current scope");
            }

            static OptimizeException OutOfRegisters() {
                return OptimizeException(OUT_OF_REGISTERS,
                                         "Ran out of available registers when filling out register values");
            }

            static OptimizeException UnknownCommand() {
                return OptimizeException(UNKNOWN_COMMAND,
                                         "Could not determine an appropiate compiler command");
            }

            static OptimizeException InvalidSingleOperation(const std::string &operation, bool post) {
                return OptimizeException(INVALID_OPERATION,
                                         "Operation '" + operation + "' not supported as a " + (post ? "POST" : "PRE") + " operation");
            }

            static OptimizeException MissingExpression(const std::string &statement) {
                return OptimizeException(MISSING_EXPRESSION,
                                         "No Expression set for the Statement of type '" + statement + "'");
            }

        protected:
            OptimizeException(Type type, const std::string &msg) : _type(type), runtime_error(msg) {}
            Type _type;
        };
    }

    namespace VHE {
        namespace Optimizer {

            struct MemoryMap {
            public:
                struct MemoryAllocation {
                    size_t index;
                    size_t varID;
                    BitWidth width;
                };
            protected:
                std::unordered_map<size_t, MemoryAllocation> _data;
                size_t _next = 0;
            public:
                MemoryAllocation create(size_t varID, BitWidth width) {
                    MemoryAllocation val{ _next, varID, width };
                    _next += width;
                    _data.insert({{ varID, val }});
                    //DEBUG_PRINT("Created Memory for ID " << std::to_string(varID));
                    return val;
                }
                MemoryAllocation get(size_t varID) const {
                    if(_data.count(varID))
                        return _data.at(varID);
                    else
                        throw Exception::OptimizeException::VariableNotFound(varID);
                }
                bool exists(size_t varID) const {
                    return _data.count(varID) > 0;
                }
                size_t size() { return _next; }
                size_t count() { return _data.size(); }
            };


            struct Settings {
                BitWidth program_width;
                vbyte max_register_count;
                Compiler::LabelMap labels;
            };


            struct IDMap {
            private:
                size_t _nextID = 0;
            public:
                size_t getID() { return _nextID++; }
            };

            CCList compileOptimizeList(const std::list<AbstractStatement*> &stmts, Settings &settings, IDMap &ids, size_t* req_mem_size);

            struct Scope {
            public:
                struct RegisterAllocation {
                    RegisterAllocation* prev;
                    RegisterAllocation* next;
                    size_t index_hint;
                    size_t varID;
                    bool defined;
                    vbyte* reg_ptr;
                    CCIter iter;
                    RegisterAllocation(RegisterAllocation* prev, RegisterAllocation* next, size_t index_hint, size_t varID, bool defined, vbyte* reg_ptr, const CCIter &iter)
                            : prev(prev), next(next), index_hint(index_hint), varID(varID), defined(defined), reg_ptr(reg_ptr), iter(iter) {}
                };

                struct RACompare {
                    bool operator()(RegisterAllocation* const& lhs, RegisterAllocation* const& rhs) const
                    { return lhs->index_hint < rhs->index_hint; }
                };

            protected:
                std::unordered_map<size_t, RegisterAllocation*> _last_pos_cache;
                std::set<RegisterAllocation*, RACompare> _begin_cache;
                std::stack<std::set<size_t>> _loop_cache;

            public:
                Scope* const _parent;

                std::string _label_break = "";
                std::string _label_continue = "";

                Scope(Scope* const parent) : _parent(parent) {}

                virtual ~Scope() {
                    for(RegisterAllocation* entry : _begin_cache) {
                        RegisterAllocation* current = entry;
                        while(current != nullptr) {
                            RegisterAllocation* next = current->next;
                            delete current;
                            current = next;
                        }
                    }
                }

                void pushLoopCache();
                void popLoopCache(size_t index_hint, CCIter iter);

                void addRegisterEntry(vbyte* reg_ptr, size_t index_hint, size_t varID, CCIter iter, bool defined = false);
                void calculateRegisters(CCList &cmds, Settings &settings, MemoryMap &mem, const std::unordered_set<vbyte> &reserved_registers);

            };

            struct AbstractExpression {
                virtual size_t compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids, size_t resID) const = 0;
                virtual size_t compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const;
                virtual std::string to_string() const = 0;
                virtual std::string to_string(size_t indent) const;
                virtual ~AbstractExpression() {};
            };

            struct AEVariable : public AbstractExpression {
                const size_t _varID;
                AEVariable(size_t varID) : _varID(varID) {}
                virtual size_t compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids, size_t resID) const;
                virtual size_t compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const;
                virtual std::string to_string() const;
            };

            struct AEConstant : public AbstractExpression {
                const int64_t _value;
                AEConstant(int64_t value) : _value(value) {}
                virtual size_t compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids, size_t resID) const;
                virtual std::string to_string() const;
            };

            struct AEArithmeticDouble : public AbstractExpression {
                const AbstractExpression* _lhs;
                const AbstractExpression* _rhs;
                const ArithmeticOperatorDouble _op;
                AEArithmeticDouble(const AbstractExpression* lhs, const AbstractExpression* rhs, ArithmeticOperatorDouble op)
                        : _lhs(lhs), _rhs(rhs), _op(op) {}
                virtual ~AEArithmeticDouble() { delete _lhs; delete _rhs; }
                virtual size_t compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids, size_t resID) const;
                virtual std::string to_string() const;
            };

            struct AEArithmeticSingle : public AbstractExpression {
                const AbstractExpression* _expr;
                const ArithmeticOperatorSingle _op;
                const bool _post;
                AEArithmeticSingle(const AbstractExpression* expr, ArithmeticOperatorSingle op, bool post = false)
                        : _expr(expr), _op(op), _post(post) {}
                virtual ~AEArithmeticSingle() { delete _expr; }
                virtual size_t compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids, size_t resID) const;
                virtual std::string to_string() const;
            };



            struct AbstractStatement {
                virtual void compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const = 0;
                virtual std::string to_string() const = 0;
                virtual std::string to_string(size_t indent) const;
                virtual ~AbstractStatement() {}
            };

            struct ASAssignment : public AbstractStatement {
                const AbstractExpression* _expr;
                const size_t _varID;
                const bool _define;
                ASAssignment(const AbstractExpression* expr, size_t varID, bool define = false)
                        : _expr(expr), _varID(varID), _define(define) {}
                virtual ~ASAssignment() { delete _expr; }
                virtual void compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const;
                virtual std::string to_string() const;
            };

            struct ASExpression : public AbstractStatement {
                const AbstractExpression* _expr;
                ASExpression(const AbstractExpression* expr)
                        : _expr(expr) {}
                virtual ~ASExpression() { delete _expr; }
                virtual void compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const;
                virtual std::string to_string() const;
            };

            struct ASFlowControl : public AbstractStatement {
                const FlowControl _control;
                const AbstractExpression* _expr_ret;
                ASFlowControl(const FlowControl control, const AbstractExpression* expr_ret = nullptr)
                        : _control(control), _expr_ret(expr_ret) {}
                virtual ~ASFlowControl() { delete _expr_ret; }
                virtual void compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const;
                virtual std::string to_string() const;
            };

            struct ASLoop : public AbstractStatement {
            protected:
                ASList _stmts;

            public:
                const AbstractStatement* _stmt_init;
                const AbstractExpression* _expr_cond;
                const AbstractStatement* _stmt_inc;

                ASLoop(ASList stmts, const AbstractStatement* expr_init, const AbstractExpression* expr_cond, const AbstractStatement* expr_inc)
                        : _stmts(stmts), _stmt_init(expr_init), _expr_cond(expr_cond), _stmt_inc(expr_inc) {}

                virtual ~ASLoop() {
                    for(AbstractStatement* stmt : _stmts) delete stmt;
                    delete _stmt_init; delete _expr_cond; delete _stmt_inc;
                }

                virtual void compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const;
                virtual std::string to_string(size_t indent) const;
                virtual std::string to_string() const { return to_string(0); }
            };

            struct ASConditional : public AbstractStatement {
                struct Block {
                    const AbstractExpression* expr;
                    ASList stmts;
                    Block(const AbstractExpression* expr, ASList stmts)
                            : expr(expr), stmts(stmts) {}
                    ~Block() {
                        delete expr;
                        for(AbstractStatement* stmt : stmts)
                            delete stmt;
                    }
                };

                ASList _else_stmts;
                std::list<Block*> _if_blocks;

                ASConditional(std::list<Block*> if_blocks, ASList else_stmts)
                        :  _if_blocks(if_blocks), _else_stmts(else_stmts)  {}

                virtual ~ASConditional() {
                    for(AbstractStatement* stmt : _else_stmts) delete stmt;
                    for(Block* block : _if_blocks) delete block;
                }

                virtual void compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const;
                virtual std::string to_string(size_t indent) const;
                virtual std::string to_string() const { return to_string(0); }
            };
        }
    }
}