#pragma once

#include "Compiler.h"

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