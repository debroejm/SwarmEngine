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

#include "Optimizer.h"

#include "api/Logging.h"

using namespace Swarm::Logging;

namespace Swarm {
    namespace VHE {

        using namespace Compiler;
        namespace Optimizer {

            CCList compileOptimizeList(const std::list<AbstractStatement*> &stmts, Settings &settings, IDMap &ids, size_t* req_mem_size) {

                CCList output;
                MemoryMap mem;
                Scope scope(nullptr);

                log_vhe(INFO) << "Optimizing VHE Program with " << stmts.size() << " top-level statements" << Flush();

                for(AbstractStatement* stmt : stmts) {
                    log_vhe(DEBUG) << stmt->to_string(1);
                    stmt->compile(output, scope, settings, mem, ids);
                }
                log_vhe().flush();

                std::unordered_set<vbyte> empty_reg_set;
                scope.calculateRegisters(output, settings, mem, empty_reg_set);

                if(req_mem_size != nullptr) *req_mem_size = mem.size();

                return output;
            }

            void Scope::pushLoopCache() { _loop_cache.push(std::set<size_t>()); }
            void Scope::popLoopCache(size_t index_hint, CCIter iter) {
                if(!_loop_cache.empty()) {
                    std::set<size_t> &ids = _loop_cache.top();
                    for (size_t id : ids)
                        addRegisterEntry(nullptr, index_hint, id, iter);
                    _loop_cache.pop();
                }
            }

            void Scope::addRegisterEntry(vbyte* reg_ptr, size_t index_hint, size_t varID, CCIter iter, bool defined) {
                RegisterAllocation* npos = nullptr;
                RegisterAllocation* cpos = _last_pos_cache[varID];

                //DEBUG_PRINT("Adding Register Entry for ID " << std::to_string(varID) << " and Index Hint " << std::to_string(index_hint))

                // Previous Entry Exists
                if(cpos != nullptr) {

                    // Iterate Backwards until a spot is found
                    if(index_hint < cpos->index_hint) {
                        RegisterAllocation* last;
                        while(true) {
                            last = cpos;
                            cpos = cpos->prev;
                            if(cpos == nullptr) break;
                            if(index_hint >= cpos->index_hint) break;
                        }
                        npos = new RegisterAllocation( cpos, last, index_hint, varID, defined, reg_ptr, iter );
                        if(cpos != nullptr) cpos->next = npos;
                        last->prev = npos;
                    }
                        // Iterate Forwards until a spot is found
                    else {
                        RegisterAllocation* last;
                        while(true) {
                            last = cpos;
                            cpos = cpos->next;
                            if(cpos == nullptr) break;
                            if(index_hint <= cpos->index_hint) break;
                        }
                        npos = new RegisterAllocation( last, cpos, index_hint, varID, defined, reg_ptr, iter );
                        if(cpos != nullptr) cpos->prev = npos;
                        last->next = npos;
                    }

                    // Check to see if the begin_cache needs to be updated
                    if(npos->prev == nullptr) {
                        _begin_cache.erase(npos->next);
                        _begin_cache.insert( npos );
                    }

                }
                    // No Previous Entry, create one
                else {
                    npos = new RegisterAllocation( nullptr, nullptr, index_hint, varID, defined, reg_ptr, iter );
                    _begin_cache.insert( npos );
                }
                // Update last_pos_cache
                _last_pos_cache[varID] = npos;

                // Add to Loop Cache if necessary
                if(!_loop_cache.empty())
                    _loop_cache.top().insert(varID);
            }

            struct UsedRegSpan {
                UsedRegSpan* prev;
                UsedRegSpan* next;
                size_t begin;
                size_t end;
                UsedRegSpan(UsedRegSpan* prev, UsedRegSpan* next, size_t begin, size_t end)
                        : prev(prev), next(next), begin(begin), end(end) {}
            };

            void Scope::calculateRegisters(CCList &cmds, Settings &settings, MemoryMap &mem, const std::unordered_set<vbyte> &reserved_registers) {

                UsedRegSpan* used_regs[settings.max_register_count] = { nullptr };

                // Iterate through all
                for(RegisterAllocation* ra : _begin_cache) {

                    RegisterAllocation* current = ra;
                    RegisterAllocation* last = ra;
                    while(current != nullptr) {
                        last = current;
                        current = current->next;
                    }

                    size_t begin = ra->index_hint;
                    size_t end = last->index_hint;

                    // Find register with unused space matching the required usage space
                    vbyte reg = settings.max_register_count;
                    for(vbyte i = 0; i < settings.max_register_count; i++) {
                        if(reserved_registers.count(i)) continue;

                        UsedRegSpan* c = used_regs[i];
                        if(c == nullptr) {
                            // This register isn't used at all, so there is nothing but empty space
                            used_regs[i] = new UsedRegSpan( nullptr, nullptr, begin, end );
                            reg = i;
                            break;
                        } else {
                            size_t last_index = 0;
                            UsedRegSpan* l = c;
                            while(c != nullptr) {
                                // Found empty space either in the beginning or in the middle
                                if( ((begin > last_index) || (c->prev == nullptr)) && end < c->begin ) {
                                    UsedRegSpan* n = new UsedRegSpan( c->prev, c, begin, end );
                                    c->prev = n;
                                    if(c->prev == nullptr) used_regs[i] = n;
                                    else c->prev->next = n;
                                    reg = i;
                                    break;
                                }
                                last_index = c->end;
                                l = c;
                                c = c->next;
                            }
                            // Found empty space at the end
                            if(begin > last_index) {
                                UsedRegSpan* n = new UsedRegSpan( l, nullptr, begin, end );
                                l->next = n;
                                reg = i;
                                break;
                            }
                        }
                    }

                    // For now, throw exception if out of registers
                    // TODO: Determine and store a used register temporarily in order to enable more register usage
                    if(reg == settings.max_register_count)
                        throw Exception::OptimizeException::OutOfRegisters();

                    // Add a Load Cmd if needed
                    if(mem.exists(ra->varID) && !ra->defined) {
                        const MemoryMap::MemoryAllocation spot = mem.get(ra->varID);
                        CompilerCommand* cc = new CCMoveToRegisterConstant(reg, spot.index, settings.program_width, spot.width);
                        cmds.insert(ra->iter, cc);
                    }

                    // Set all the register values
                    current = ra;
                    last = ra;
                    while(current != nullptr) {
                        if(current->reg_ptr != nullptr) *(current->reg_ptr) = reg;
                        last = current;
                        current = current->next;
                    }

                    // Add a Store Cmd if needed
                    if(mem.exists(ra->varID)) {
                        const MemoryMap::MemoryAllocation spot = mem.get(ra->varID);
                        CompilerCommand* cc = new CCMoveToMemoryConstant(reg, spot.index, settings.program_width, spot.width);
                        CCIter i = last->iter;
                        i++;
                        cmds.insert(i, cc);
                    }

                }

                // Delete Heap Created Data
                for(vbyte i = 0; i < settings.max_register_count; i++) {
                    UsedRegSpan* c = used_regs[i];
                    while(c != nullptr) {
                        UsedRegSpan* n = c->next;
                        delete c;
                        c = n;
                    }
                }
            }

            size_t AbstractExpression::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const {
                size_t ret_id = ids.getID();
                return compile(output, scope_parent, settings, mem, ids, ret_id);
            }

            std::string AbstractExpression::to_string(size_t indent) const {
                std::string pre("");
                for(size_t i = 0; i < indent; i++)
                    pre += "  ";
                return pre + to_string();
            }

            size_t AEVariable::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids, size_t resID) const { return _varID; }
            size_t AEVariable::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const { return _varID; }
            std::string AEVariable::to_string() const { return "{" + std::to_string(_varID) + "}"; }

            size_t AEConstant::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids, size_t resID) const {
                CCLoadConstant* cmd = new CCLoadConstant(0, _value, settings.program_width);
                CCIter it = output.insert(output.end(), &*cmd);
                scope_parent.addRegisterEntry(&cmd->_target_register, output.size()-1, resID, it);
                return resID;
            }
            std::string AEConstant::to_string() const { return std::to_string(_value); }

            size_t AEArithmeticDouble::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids, size_t resID) const {

                // Order matters; right to left
                size_t ret_rhs = _rhs->compile(output, scope_parent, settings, mem, ids);
                size_t ret_lhs = _lhs->compile(output, scope_parent, settings, mem, ids);

                CCALUDoubleOperation* cmd = nullptr;
                switch(_op) {
                    case ADDITION:          cmd = new CCALUAddition(0, 0, 0); break;
                    case SUBTRACTION:       cmd = new CCALUSubtraction(0, 0, 0); break;
                    case MULTIPLICATION:    cmd = new CCALUMultiplication(0, 0, 0); break;
                    case DIVISION:          cmd = new CCALUDivision(0, 0, 0); break;
                    case MODULUS:           cmd = new CCALUModulus(0, 0, 0); break;
                    default: throw Exception::OptimizeException::UnknownCommand();
                }

                CCIter it = output.insert(output.end(), &*cmd);

                scope_parent.addRegisterEntry(&cmd->_in_register_a, output.size()-1, ret_lhs, it);
                scope_parent.addRegisterEntry(&cmd->_in_register_b, output.size()-1, ret_rhs, it);
                scope_parent.addRegisterEntry(&cmd->_out_register,  output.size()-1, resID,  it);

                return resID;
            }
            std::string AEArithmeticDouble::to_string() const
                { return "(" + _lhs->to_string() + " " + std::to_string(_op) + " " + _rhs->to_string() + ")"; }

            size_t AEArithmeticSingle::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids, size_t resID) const {
                size_t ret_expr = _expr->compile(output, scope_parent, settings, mem, ids);

                switch(_op) {
                    case POSITIVE: {
                        if(_post) throw Exception::OptimizeException::InvalidSingleOperation("Positive", _post);
                        else return ret_expr;
                    }
                    case NEGATIVE: {
                        if(_post) throw Exception::OptimizeException::InvalidSingleOperation("Negative", _post);
                        CCALUMoveInversion* cmd = new CCALUMoveInversion(0, 0);
                        CCIter it = output.insert(output.end(), &*cmd);
                        scope_parent.addRegisterEntry(&cmd->_in_register,  output.size()-1, ret_expr, it);
                        scope_parent.addRegisterEntry(&cmd->_out_register, output.size()-1, resID,   it);
                    } break;
                    case INCREMENT: {
                        if(_post) {
                            CCCopyRegister* cmd1 = new CCCopyRegister(0, 0);
                            CCIter it = output.insert(output.end(), &*cmd1);
                            scope_parent.addRegisterEntry(&cmd1->_from_register, output.size()-1, ret_expr, it);
                            scope_parent.addRegisterEntry(&cmd1->_to_register,   output.size()-1, resID,   it);
                            CCALUIncrement* cmd2 = new CCALUIncrement(0);
                            it = output.insert(output.end(), &*cmd2);
                            scope_parent.addRegisterEntry(&cmd2->_register, output.size()-1, ret_expr, it);
                        } else {
                            CCALUIncrement* cmd1 = new CCALUIncrement(0);
                            CCIter it = output.insert(output.end(), &*cmd1);
                            scope_parent.addRegisterEntry(&cmd1->_register, output.size()-1, ret_expr, it);
                            CCCopyRegister* cmd2 = new CCCopyRegister(0, 0);
                            it = output.insert(output.end(), &*cmd2);
                            scope_parent.addRegisterEntry(&cmd2->_from_register, output.size()-1, ret_expr, it);
                            scope_parent.addRegisterEntry(&cmd2->_to_register,   output.size()-1, resID,   it);
                        }
                    } break;
                    case DECREMENT: {
                        if(_post) {
                            CCCopyRegister* cmd1 = new CCCopyRegister(0, 0);
                            CCIter it = output.insert(output.end(), &*cmd1);
                            scope_parent.addRegisterEntry(&cmd1->_from_register, output.size()-1, ret_expr, it);
                            scope_parent.addRegisterEntry(&cmd1->_to_register,   output.size()-1, resID,   it);
                            CCALUDecrement* cmd2 = new CCALUDecrement(0);
                            it = output.insert(output.end(), &*cmd2);
                            scope_parent.addRegisterEntry(&cmd2->_register, output.size()-1, ret_expr, it);
                        } else {
                            CCALUDecrement* cmd1 = new CCALUDecrement(0);
                            CCIter it = output.insert(output.end(), &*cmd1);
                            scope_parent.addRegisterEntry(&cmd1->_register, output.size()-1, ret_expr, it);
                            CCCopyRegister* cmd2 = new CCCopyRegister(0, 0);
                            it = output.insert(output.end(), &*cmd2);
                            scope_parent.addRegisterEntry(&cmd2->_from_register, output.size()-1, ret_expr, it);
                            scope_parent.addRegisterEntry(&cmd2->_to_register,   output.size()-1, resID,   it);
                        }
                    } break;
                    default: throw Exception::OptimizeException::UnknownCommand();
                }

                return resID;
            }
            std::string AEArithmeticSingle::to_string() const
                { return _post ? ( _expr->to_string() + std::to_string(_op) ) : ( std::to_string(_op) + _expr->to_string() ); }

            std::string AbstractStatement::to_string(size_t indent) const {
                std::string pre("");
                for(size_t i = 0; i < indent; i++)
                    pre += "  ";
                return pre + to_string();
            }

            void ASAssignment::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const {
                if(_define && !mem.exists(_varID)) mem.create(_varID, settings.program_width);
                _expr->compile(output, scope_parent, settings, mem, ids, _varID);
            }
            std::string ASAssignment::to_string() const
                { return std::string(_define ? "init " : "") + "{" + std::to_string(_varID) + "} = " + _expr->to_string(); }

            void ASExpression::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const
                { _expr->compile(output, scope_parent, settings, mem, ids); }
            std::string ASExpression::to_string() const { return _expr->to_string(); }

            void ASFlowControl::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const {
                switch(_control) {
                    case BREAK: {
                        if(scope_parent._label_break.size() < 1)
                            throw Exception::OptimizeException::ScopeControl(_control);
                        else
                            output.insert(output.end(), new CCJump(settings.labels, scope_parent._label_break));
                    } break;
                    case CONTINUE: {
                        if(scope_parent._label_continue.size() < 1)
                            throw Exception::OptimizeException::ScopeControl(_control);
                        else
                            output.insert(output.end(), new CCJump(settings.labels, scope_parent._label_continue));
                    } break;
                    case RETURN: {
                        // TODO: Fill out ASFlowControl Return operation
                        throw Exception::OptimizeException::ScopeControl(_control);
                    } break;
                    default: break;
                }
            }
            std::string ASFlowControl::to_string() const {
                return std::to_string(_control) +
                       (( _control == RETURN && _expr_ret != nullptr) ? (" " + _expr_ret->to_string()) : "");
            }

            void ASLoop::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const {

                scope_parent.pushLoopCache();

                // Loop initialize statement
                if(_stmt_init != nullptr) _stmt_init->compile(output, scope_parent, settings, mem, ids);

                // Load a '0' constant for comparisons
                size_t zeroConstID = ids.getID();
                CCLoadConstant* cmd_lc = new CCLoadConstant(0, 0, BIT_8);
                CCIter it = output.insert(output.end(), &*cmd_lc);
                scope_parent.addRegisterEntry(&cmd_lc->_target_register, output.size()-1, zeroConstID, it);

                // Create the loop's labels
                std::string label_begin = settings.labels.uniqueLabel(SWM_OPT_LABEL_LOOP_BEGIN);
                std::string label_check = settings.labels.uniqueLabel(SWM_OPT_LABEL_LOOP_CHECK);
                std::string label_end = settings.labels.uniqueLabel(SWM_OPT_LABEL_LOOP_END);

                // Cache the previous Flow Control labels
                std::string label_old_fc_break = scope_parent._label_break;
                std::string label_old_fc_continue = scope_parent._label_continue;

                // Set the Flow Control labels
                scope_parent._label_break = label_end;
                scope_parent._label_continue = label_begin;

                // Jump to the Loop Condition Check
                output.insert(output.end(), new CCJump(settings.labels, label_check));

                // Create the start label
                output.insert(output.end(), new CCLabel(settings.labels, label_begin));

                // Evaluate loop contents
                for(AbstractStatement* stmt : _stmts) {
                    stmt->compile(output, scope_parent, settings, mem, ids);
                }

                // Loop Increment Statement
                if(_stmt_inc != nullptr) _stmt_inc->compile(output, scope_parent, settings, mem, ids);

                // Create the check label
                output.insert(output.end(), new CCLabel(settings.labels, label_check));

                // Jump back to the start based on Loop Check Expression
                if(_expr_cond != nullptr) {
                    size_t retID_cond = _expr_cond->compile(output, scope_parent, settings, mem, ids);
                    CCJumpLessThan *cmd_jmp = new CCJumpLessThan(settings.labels, label_begin, 0, 0);
                    it = output.insert(output.end(), &*cmd_jmp);
                    scope_parent.addRegisterEntry(&cmd_jmp->_register_a, output.size() - 1, zeroConstID, it);
                    scope_parent.addRegisterEntry(&cmd_jmp->_register_b, output.size() - 1, retID_cond, it);
                } else {
                    // No check, always jump (infinite loop if no Flow Control exists)
                    output.insert(output.end(), new CCJump(settings.labels, label_begin));
                }

                // Create the end label
                output.insert(output.end(), new CCLabel(settings.labels, label_end));

                // Reset the Flow Control labels
                scope_parent._label_break = label_old_fc_break;
                scope_parent._label_continue = label_old_fc_continue;

                scope_parent.popLoopCache(output.size() - 1, --output.end());
            }
            std::string ASLoop::to_string(size_t indent) const {
                std::string ind("");
                for(size_t i = 0; i < indent; i++)
                    ind += "  ";
                std::string result = ind + "loop ( " +
                                     ( _stmt_init == nullptr ? "" : _stmt_init->to_string()) + " ; " +
                                     ( _expr_cond == nullptr ? "" : _expr_cond->to_string()) + " ; " +
                                     ( _stmt_inc == nullptr ? "" : _stmt_inc->to_string()) + " )";
                for(AbstractStatement* stmt : _stmts)
                    result += "\n" + stmt->to_string(indent+1);
                return result;
            }

            void ASConditional::compile(CCList &output, Scope &scope_parent, Settings &settings, MemoryMap &mem, IDMap &ids) const {

                // Load a '0' constant for comparisons
                size_t zeroConstID = ids.getID();
                CCLoadConstant* cmd_lc = new CCLoadConstant(0, 0, BIT_8);
                CCIter it = output.insert(output.end(), &*cmd_lc);
                scope_parent.addRegisterEntry(&cmd_lc->_target_register, output.size()-1, zeroConstID, it);

                // Create the conditional's labels
                std::string label_if = settings.labels.uniqueLabel(SWM_OPT_LABEL_CONDITIONAL_IF);
                std::string label_else = settings.labels.uniqueLabel(SWM_OPT_LABEL_CONDITIONAL_ELSE);
                std::string label_end = settings.labels.uniqueLabel(SWM_OPT_LABEL_CONDITIONAL_END);

                // Check to make sure there is at least one conditional block
                if(_if_blocks.empty())
                    throw Exception::OptimizeException::MissingExpression("Conditional");

                // Go through If blocks in order and check conditionals
                size_t bi = 0;
                for(Block* block : _if_blocks) {
                    size_t retID_cond = block->expr->compile(output, scope_parent, settings, mem, ids);
                    CCJumpLessThan *cmd_jmp = new CCJumpLessThan(settings.labels, label_if + std::to_string(bi), 0, 0);
                    it = output.insert(output.end(), &*cmd_jmp);
                    scope_parent.addRegisterEntry(&cmd_jmp->_register_a, output.size() - 1, zeroConstID, it);
                    scope_parent.addRegisterEntry(&cmd_jmp->_register_b, output.size() - 1, retID_cond, it);
                    bi++;
                }

                // Add Else jump
                output.insert(output.end(), new CCJump(settings.labels, label_else));

                // Go through If blocks in order and compile statements
                bi = 0;
                for(Block* block : _if_blocks) {
                    output.insert(output.end(), new CCLabel(settings.labels, label_if + std::to_string(bi)));
                    for(AbstractStatement* stmt : block->stmts) {
                        stmt->compile(output, scope_parent, settings, mem, ids);
                    }
                    output.insert(output.end(), new CCJump(settings.labels, label_end));
                    bi++;
                }

                // Create the Else label
                output.insert(output.end(), new CCLabel(settings.labels, label_else));

                // Add Else statements
                for(AbstractStatement* stmt : _else_stmts) {
                    stmt->compile(output, scope_parent, settings, mem, ids);
                }

                // Create the end label
                output.insert(output.end(), new CCLabel(settings.labels, label_end));
            }
            std::string ASConditional::to_string(size_t indent) const {
                std::string ind("");
                for(size_t i = 0; i < indent; i++)
                    ind += "  ";
                std::string result("");
                size_t bi = 0;
                for(Block* block : _if_blocks) {
                    result += ind + "conditional " + std::to_string(bi) + "( " + block->expr->to_string() + ")\n";
                    for (AbstractStatement *stmt : block->stmts)
                        result += stmt->to_string(indent + 1) + "\n";
                    bi++;
                }
                result += ind + "conditional else";
                for (AbstractStatement *stmt : _else_stmts)
                    result += "\n" + stmt->to_string(indent + 1);
                return result;
            }
        }
    }
}