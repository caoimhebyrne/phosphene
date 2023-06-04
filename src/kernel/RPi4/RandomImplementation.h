#pragma once

#include "../../types/integer.h"
#include "../Random.h"

// The undocumented hardware randomness of the RPi4 / BCM2711 (and RPi3 / BCM2837)...
// https://github.com/raspberrypi/linux/blob/rpi-6.1.y/arch/arm/boot/dts/bcm2711.dtsi#L125C1-L128
// https://github.com/raspberrypi/documentation/issues/1148
// https://github.com/raspberrypi/linux/blob/rpi-6.1.y/drivers/char/hw_random/iproc-rng200.c

namespace Kernel::RPi4 {

class RandomImplementation : public Random {
public:
    static RandomImplementation& instance();

    virtual void initialize() override;
    virtual u32 get() override;

private:
    void wait_until_ready_for_reading();
};

}
