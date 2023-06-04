#pragma once

#include "../types/integer.h"

namespace Kernel {

// The RPi3 and RPi4 have slightly different (and undocumented) hardware random number generators
class Random {
public:
    static Random* instance();

    virtual void initialize() = 0;
    virtual u32 get() = 0;
};

}
