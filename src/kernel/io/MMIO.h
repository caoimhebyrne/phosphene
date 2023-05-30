#pragma once

#include "../../types/integer.h"

namespace Kernel {

// Memory Mapped I/O
class MMIO {
public:
    // We used a shared-instance approach when handling MMIO
    static MMIO& instance();

    void write(u32 reg, u32 value);
    u32 read(u32 reg);

private:
    MMIO();

    u32 m_base_address = -1;
};

}