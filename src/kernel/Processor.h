#pragma once

#include "asm/CurrentELRegister.h"
#include "asm/MainIdRegister.h"
#include "io/UART.h"

namespace Kernel {

class Processor {
public:
    struct StackFrame {
        struct StackFrame* previous_frame;
        size_t last_register;
    };

    struct Info {
        const char* name;

        PartNumber part_number;
        ExceptionLevel exception_level;
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

    static Info get_info()
    {
        MainIdRegister id_register;
        CurrentELRegister el_register;

        return {
            .name = id_register.part_number_as_string(),
            .part_number = id_register.part_number(),
            .exception_level = el_register.exception_level(),
        };
    }
};

}
