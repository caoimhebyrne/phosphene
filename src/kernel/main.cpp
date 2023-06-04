#include "Kernel.h"
#include "MemoryManagement.h"
#include "asm/CurrentELRegister.h"
#include "asm/MainIdRegister.h"
#include "io/UART.h"

namespace Kernel {

void main()
{
    auto uart = UART::instance();
    uart.println("[main] \"No heaven without hell beneath, in misery without you and me\"");

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

    test_memory_management();

    uart.println("[main] Halting!");

    while (true) {
    }
}

void test_memory_management()
{
    auto uart = UART::instance();

    auto expected_a_value = 4;
    auto expected_b_value = 69;

    uart.println("[test_memory_management] Checking if `allocate` works...");

    auto address_a = MemoryManagement::instance().allocate(sizeof(int));
    uart.println("[test_memory_management] Allocated for `address_a` at {#}", address_a);

    uart.println("[test_memory_management]   -> Setting {#} = {i}", address_a, expected_a_value);
    *(int*)address_a = expected_a_value;

    auto address_b = MemoryManagement::instance().allocate(sizeof(int));
    uart.println("[test_memory_management] Allocated for `address_b` at {#}", address_b);

    uart.println("[test_memory_management]   -> Setting {#} = {i}", address_b, expected_b_value);
    *(int*)address_b = expected_b_value;

    uart.println("[test_memory_management] Values: {#} = {i}, {#} = {i}", address_a, *(int*)address_a, address_b, *(int*)address_b);

    auto valid = *(int*)address_a == expected_a_value && *(int*)address_b == expected_b_value;
    if (!valid) {
        uart.println("[test_memory_management] ERROR: Expected values were not in {#} or {#}!", address_a, address_b);
        uart.println("                         * {#} should have been {i}, and was {i}", address_a, expected_a_value, *(int*)address_a);
        uart.println("                         * {#} should have been {i}, and was {i}", address_b, expected_b_value, *(int*)address_b);

        return;
    }

    uart.println("[test_memory_management] It appears that `allocate` is working as expected!");
}

}
