#include "MMIO.h"
#include "../asm/MainIdRegister.h"

namespace Kernel {

MMIO& MMIO::instance()
{
    static MMIO instance;
    return instance;
}

// We decide the base address for MMIO based on the Raspberry Pi part number
// https://wiki.osdev.org/Detecting_Raspberry_Pi_Board
MMIO::MMIO()
{
    MainIdRegister id_register;
    switch (id_register.part_number()) {
    case PartNumber::Pi2:
    case PartNumber::Pi3:
        m_base_address = 0x3F000000;
        break;

    case PartNumber::Pi4:
        m_base_address = 0xFE000000;
        break;

    default:
        m_base_address = 0x20000000;
        break;
    }
}

void MMIO::write(u32 reg, u32 value)
{
    *(volatile u32*)(m_base_address + reg) = value;
}

u32 MMIO::read(u32 reg)
{
    return *(volatile u32*)(m_base_address + reg);
}

}
