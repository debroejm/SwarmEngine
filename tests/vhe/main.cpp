#include "api/Core.h"
#include "api/Logging.h"

// Shouldn't directly include an internal header, but its a stopgap measure for now
#include "vhe/Optimizer.h"

using namespace Swarm;
using namespace Swarm::Logging;
using namespace Swarm::VHE;

int main() {

    // Initialization
    if(!Core::init(SWM_INIT_VHE)) {
        return -1;
    }

    try {
        ASList stmts;
        Optimizer::IDMap ids;
        Optimizer::Settings settings{
                BIT_64,
                32,
                Compiler::LabelMap()
        };

        size_t var_fib_a = ids.getID();
        size_t var_fib_b = ids.getID();
        size_t var_count = ids.getID();

        stmts.push_back(new Optimizer::ASAssignment(
                new Optimizer::AEConstant(3),
                var_fib_a, true));
        stmts.push_back(new Optimizer::ASAssignment(
                new Optimizer::AEConstant(2),
                var_fib_b, true));

        ASList loop_stmts;
        std::list<Optimizer::ASConditional::Block*> if_blocks(
                {
                        new Optimizer::ASConditional::Block(new Optimizer::AEArithmeticDouble(
                                new Optimizer::AEVariable(var_count),
                                new Optimizer::AEConstant(2),
                                Optimizer::MODULUS), ASList(
                                {
                                        new Optimizer::ASAssignment( new Optimizer::AEArithmeticDouble(
                                                new Optimizer::AEVariable(var_fib_a),
                                                new Optimizer::AEVariable(var_fib_b),
                                                Optimizer::ADDITION), var_fib_a)
                                }
                        ))
                }
        );
        loop_stmts.push_back( new Optimizer::ASConditional(if_blocks, ASList(
                {
                        new Optimizer::ASAssignment( new Optimizer::AEArithmeticDouble(
                                new Optimizer::AEVariable(var_fib_a),
                                new Optimizer::AEVariable(var_fib_b),
                                Optimizer::ADDITION), var_fib_b)
                }
        )));

        stmts.push_back( new Optimizer::ASLoop(
                loop_stmts,
                new Optimizer::ASAssignment(
                        new Optimizer::AEConstant(1000),
                        var_count, true
                ),
                new Optimizer::AEVariable(var_count),
                new Optimizer::ASExpression( new Optimizer::AEArithmeticSingle(
                        new Optimizer::AEVariable(var_count),
                        Optimizer::DECREMENT
                ))
        ));

        size_t req_mem_size;
        CCList cmds = Optimizer::compileOptimizeList(stmts, settings, ids, &req_mem_size);

        Environment::Program program = Compiler::compileCommandList(cmds, req_mem_size);

        Environment::VirtualEnvironment ve(BIT_64, 32, 1, MEM_KB, 128, MEM_BYTE);
        retcode result = program.run(ve);

        Log::log_vhe(INFO) << "RetCode=" << result;
        Log::log_vhe(INFO) << "Registers:\n" << ve.printRegisters() << "\n";
        Log::log_vhe(INFO) << "Memory:\n" << ve.printMemory() << "\n";

        // Delete Heap-Allocated Lists
        for (Optimizer::AbstractStatement *stmt : stmts)
            delete stmt;
        for (Compiler::CompilerCommand *cmd : cmds)
            delete cmd;
    } catch(std::exception &e) {
        Log::log_vhe(ERR) << e.what();
        return -1;
    }

    // Cleanup Everything Up When Done
    Core::cleanup();

}