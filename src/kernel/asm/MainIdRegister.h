#pragma once

#include "../../types/integer.h"

namespace Kernel {

// https://wiki.osdev.org/Detecting_Raspberry_Pi_Board
enum class PartNumber : u32 {
    Pi1 = 0xB76,
    Pi2 = 0xC07,
    Pi3 = 0xD03,
    Pi4 = 0xD08
};

// https://developer.arm.com/documentation/ddi0601/2023-03/AArch64-Registers/MIDR-EL1--Main-ID-Register?lang=en
class MainIdRegister {
public:
    MainIdRegister()
    {
        asm volatile("mrs %x0, midr_el1"
                     : "=r"(m_data));
    }

    // https://wiki.osdev.org/Detecting_Raspberry_Pi_Board
    PartNumber part_number()
    {
        return static_cast<PartNumber>((m_data >> 4) & 0xFFF);
    }

    const char* part_number_as_string()
    {
        switch (part_number()) {
        case PartNumber::Pi1:
            return "Raspberry Pi 1";

        case PartNumber::Pi2:
            return "Raspberry Pi 2";

        case PartNumber::Pi3:
            return "Raspberry Pi 3";

        case PartNumber::Pi4:
            return "Raspberry Pi 4";

        default:
            return "Unknown raspberry PI part number";
        }
    }

private:
    u32 m_data;
};

}
