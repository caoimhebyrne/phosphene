#include "asm/CurrentELRegister.h"
#include "asm/MainIdRegister.h"
#include "io/UART.h"

namespace Kernel {

void main()
{
    auto uart = UART::instance();
    uart.println("[main] Starting...");

    // Print out which Raspberry PI board we are on.
    MainIdRegister id_register;
    uart.println("[main] Board detected: {s}", id_register.part_number_as_string());

    // Our OS only supports the Pi3 and Pi4 at the moment.
    if (id_register.part_number() != PartNumber::Pi3 && id_register.part_number() != PartNumber::Pi4) {
        uart.println("ERROR: Unexpected Raspberry PI board revision!");

        return;
    }

    CurrentELRegister el_register;

    // Our exception level should be 2 by default, this is known as the "Hypervisor" level.
    // TODO: In the future, we should drop from EL2 to EL1.
    auto exception_level = el_register.exception_level();
    if (exception_level != ExceptionLevel::EL2) {
        uart.println("ERROR: Expected exception level to be EL2, but it was EL{i}!", exception_level);

        // Our boot.S will just halt the CPU if we return from main()
        return;
    }

    uart.println("[main] Running on exception level {i}", exception_level);
    uart.println("[main] Halting!");

    while (true) {
    }
}

}
