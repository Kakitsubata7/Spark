#include "Executor.hpp"

#include "Opcode.hpp"

namespace Spark {

    /* ===== Execution ===== */

    bool Executor::execute() {

        Opcode opcode = Opcode::Halt;

        switch (opcode) {

            case Opcode::Halt:
                return true;

            case Opcode::PushNone:

                break;

            case Opcode::PushType:

                break;

            case Opcode::PushInteger:

                break;

            case Opcode::PushFloat:

                break;

            case Opcode::PushBoolean:

                break;

            case Opcode::PushString:

                break;

            case Opcode::PushArray:

                break;

            case Opcode::PushTable:

                break;

            case Opcode::PushObject:

                break;

            default:
                break;
        }

        return false;
    }

} // Spark
