#pragma once

#include "io/UART.h"

namespace Kernel {

class Processor {
public:
    struct StackFrame {
        struct StackFrame* previous_frame;
        size_t last_register;
    };

    static inline void halt()
    {
        while (true) {
            asm volatile("wfi");
        }
    }

    static void panic(const char* message = "")
    {
        UART::instance().println("PANIC: {s}", message);

        struct StackFrame* frame;
        asm volatile("mov x0, sp"
                     : "=r"(frame));

        for (auto i = 0; frame && i < 16; ++i) {
            UART::instance().println("       {i}: {#}", i, frame->last_register);
            frame = frame->previous_frame;
        }

        halt();
    }
};

}
