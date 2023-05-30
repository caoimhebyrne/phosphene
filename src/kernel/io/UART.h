#pragma once

#include "../../types/integer.h"
#include <stdarg.h>

namespace Kernel {

class UART {
public:
    static UART& instance();

    void print(const char* string, ...);
    void println(const char* string, ...);

    u32 read();
    void write(u32 value);

private:
    UART();

    void print(const char* string, va_list arguments);
    void print_raw(const char* string);

    void wait_until_ready_for_reading();
    void wait_until_ready_for_writing();
};

}