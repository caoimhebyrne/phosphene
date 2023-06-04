#include "RandomImplementation.h"
#include "../io/MMIO.h"
#include "../io/UART.h"

// Most of the magic numbers you see here are from:
// https://github.com/raspberrypi/linux/blob/rpi-6.1.y/drivers/char/hw_random/iproc-rng200.c

namespace Kernel::RPi4 {

struct Register {
    static const u32 Base = 0x00104000;
    static const u32 Control = Base + 0x0;
    static const u32 Data = Base + 0x20;
    static const u32 FIFOCount = Base + 0x24;
};

struct Control {
    static const u32 Enable = 1;
};

struct Mask {
    static const u32 EnableRNG = 0x00001FFF;
    static const u32 FIFOCount = 0x000000FF;
};

RandomImplementation& RandomImplementation::instance()
{
    static RandomImplementation instance;
    return instance;
}

// https://github.com/raspberrypi/linux/blob/rpi-6.1.y/drivers/char/hw_random/iproc-rng200.c#L206
void RandomImplementation::initialize()
{
    auto control_register = MMIO::instance().read(Register::Control);
    if (control_register & Mask::EnableRNG) {
        UART::instance().println("[RPi4::Random] Already enabled!");
        return;
    }

    auto control = (0x3 << 13) | Mask::EnableRNG;
    MMIO::instance().write(Register::Control, control);
}

// https://github.com/raspberrypi/linux/blob/rpi-6.1.y/drivers/char/hw_random/iproc-rng200.c#L199
u32 RandomImplementation::get()
{
    this->wait_until_ready_for_reading();
    return MMIO::instance().read(Register::Data);
}

// https://github.com/raspberrypi/linux/blob/rpi-6.1.y/drivers/char/hw_random/iproc-rng200.c#L185
void RandomImplementation::wait_until_ready_for_reading()
{
    while ((MMIO::instance().read(Register::FIFOCount) & Mask::FIFOCount) == 0) {
    }
}

}
