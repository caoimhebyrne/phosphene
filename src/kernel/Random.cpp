#include "Random.h"
#include "MemoryManagement.h"
#include "RPi3/RandomImplementation.h"
#include "RPi4/RandomImplementation.h"
#include "asm/MainIdRegister.h"
#include "io/UART.h"

namespace Kernel {

RPi3::RandomImplementation* rpi3_implementation;
RPi4::RandomImplementation* rpi4_implementation;

Random* Random::instance()
{
    MainIdRegister id_register;
    switch (id_register.part_number()) {
    case PartNumber::Pi4: {
        if (rpi4_implementation == nullptr) {
            rpi4_implementation = new RPi4::RandomImplementation();
            rpi4_implementation->initialize();
        }

        return rpi4_implementation;
    }

    default:
        if (id_register.part_number() != PartNumber::Pi3) {
            UART::instance().println("[Random] Using the default Pi3 random number generator... this may not work!");
        }

        if (rpi3_implementation == nullptr) {
            rpi3_implementation = new RPi3::RandomImplementation();
            rpi3_implementation->initialize();
        }

        return rpi3_implementation;
    }
}

}