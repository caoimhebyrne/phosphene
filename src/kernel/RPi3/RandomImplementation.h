#pragma once

#include "../../types/integer.h"
#include "../Random.h"

// https://elinux.org/BCM2835_registers#RNG
namespace Kernel::RPi3 {

class RandomImplementation : public Random {
public:
    static RandomImplementation& instance();

    virtual void initialize() override;
    virtual u32 get() override;

private:
    void wait_until_ready_for_reading();
};

}
