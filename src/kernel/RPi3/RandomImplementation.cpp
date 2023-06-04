#include "RandomImplementation.h"
#include "../io/MMIO.h"
#include "../io/UART.h"

// Most of the magic numbers you see here are from:
// https://elinux.org/BCM2835_registers#RNG

namespace Kernel::RPi3 {

struct Register {
    static const u32 Base = 0x00104000;

    static const u32 Control = Base + 0x00;
    static const u32 Status = Base + 0x04;
    static const u32 Data = Base + 0x08;
    static const u32 IntegerMask = Base + 0x10;
};

struct Control {
    static const u32 Enable = 1;
};

RandomImplementation& RandomImplementation::instance()
{
    static RandomImplementation instance;
    return instance;
}

void RandomImplementation::initialize()
{
    auto control_register = MMIO::instance().read(Register::Control);
    if (control_register & Control::Enable) {
        UART::instance().println("[RPi3::Random] Already enabled!");
        return;
    }

    // Unsure why exactly this is needed...
    MMIO::instance().write(Register::Status, 0x40000);

    // Mask the interrupt bit
    auto mask = MMIO::instance().read(Register::IntegerMask);
    mask |= 1;
    MMIO::instance().write(Register::IntegerMask, mask);

    // Enable the random number generator
    control_register |= Control::Enable;
    MMIO::instance().write(Register::Control, control_register);
}

u32 RandomImplementation::get()
{
    return MMIO::instance().read(Register::Data);
}

void RandomImplementation::wait_until_ready_for_reading()
{
    // TODO
}

}
