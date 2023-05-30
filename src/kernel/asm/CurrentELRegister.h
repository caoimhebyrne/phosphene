#pragma once

#include "../../types/integer.h"

namespace Kernel {

// https://developer.arm.com/documentation/ddi0488/c/programmers-model/armv8-architecture-concepts/exception-levels
enum class ExceptionLevel : u32 {
    // Applications: This is the lowest in terms of privledge
    EL0 = 0,

    // Kernel
    EL1 = 1,

    // Hypervisor: Provides support for processor virtualization
    EL2 = 2,

    // Secure monitor: Provides support for two securtity states (https://developer.arm.com/documentation/ddi0488/c/programmers-model/armv8-architecture-concepts/security-state?lang=en)
    EL3 = 3
};

// https://developer.arm.com/documentation/ddi0601/2023-03/AArch64-Registers/CurrentEL--Current-Exception-Level?lang=en
class CurrentELRegister {
public:
    CurrentELRegister()
    {
        asm volatile("mrs %x0, CurrentEL"
                     : "=r"(m_data));
    }

    ExceptionLevel exception_level()
    {
        return static_cast<ExceptionLevel>(m_data >> 2);
    }

private:
    u32 m_data;
};

}
