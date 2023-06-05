#include "../fluorescent/Fluorescent.h"
#include "Kernel.h"
#include "MemoryManagement.h"
#include "Processor.h"
#include "asm/CurrentELRegister.h"
#include "asm/MainIdRegister.h"
#include "io/UART.h"

namespace Kernel {

void main()
{
    auto uart = UART::instance();

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
    test_random_number_generation();

    Processor::panic("Reached end of init!");
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

    uart.println("[test_memory_management] Checking if `free` works...");

    uart.println("[test_memory_management] Freeing address_a ({#})...", address_a);
    MemoryManagement::instance().free(address_a);

    uart.println("[test_memory_management] Values: {#} = {i}, {#} = {i}", address_a, *(int*)address_a, address_b, *(int*)address_b);

    auto free_valid = *(int*)address_a != expected_a_value && *(int*)address_b == expected_b_value;
    if (!valid) {
        uart.println("[test_memory_management] ERROR: Expected values were not in {#} or {#}!", address_a, address_b);
        uart.println("                         * {#} should not have been {i}", address_a, expected_a_value);
        uart.println("                         * {#} should have been {i}, and was {i}", address_b, expected_b_value, *(int*)address_b);

        return;
    }

    uart.println("[test_memory_management] It appears that `free` works as expected!");

    uart.println("[test_memory_management] Cleaning up...");
    MemoryManagement::instance().free(address_b);

    uart.println("[test_memory_management] Checking if `free`ing an address and then `malloc`ing of the same size causes the old region to be re-used...");

    auto verification_address = MemoryManagement::instance().allocate(sizeof(int));
    uart.println("[test_memory_management] Allocated {i} bytes for verification_address at {#}.", sizeof(int), verification_address);

    // Make sure that this doesn't get affected.
    *(int*)verification_address = 0x69;
    uart.println("[test_memory_management]    -> Setting {#} = {i}", verification_address, 0x69);

    auto temporary_address = MemoryManagement::instance().allocate(sizeof(int));
    uart.println("[test_memory_management] Allocated {i} bytes for temporary_address at {#}.", sizeof(int), temporary_address);

    // This is just to make sure that the free works correctly
    *(int*)temporary_address = 0x420;
    uart.println("[test_memory_management]    -> Setting {#} = {i}", temporary_address, 0x420);

    uart.println("[test_memory_management] Freeing temporary_address ({#})...", temporary_address);

    auto old_address = temporary_address;
    MemoryManagement::instance().free(temporary_address);

    // Try allocating another integer region
    uart.println("[test_memory_management] Allocating {i} bytes for final_address...", sizeof(int));
    auto final_address = MemoryManagement::instance().allocate(sizeof(int));
    if (final_address != old_address) {
        Processor::panic("Reallocation of same-sized-address didn't fill free slot!");
    }

    uart.println("[test_memory_management] Freeing {i} bytes caused {#} to be re-used after a free! Woohoo!", sizeof(int), final_address);

    uart.println("[test_memory_management] Cleaning up...");
    MemoryManagement::instance().free(verification_address);
    MemoryManagement::instance().free(final_address);

    uart.println("[test_memory_management] Checking if freeing a large region will cause it to be resized...");

    auto big_address = MemoryManagement::instance().allocate(512);
    *(int*)big_address = 0x69;
    uart.println("[test_memory_management] Allocated 512 bytes for big_address at {#}", big_address);

    uart.println("[test_memory_management] Freeing big_address... ({#})", big_address);
    MemoryManagement::instance().free(big_address);

    uart.println("[test_memory_management] Allocating 16 bytes for small_address...");
    auto small_address = MemoryManagement::instance().allocate(16);
    if (*(int*)small_address == 0x69) {
        Processor::panic("big_address was not free'd correctly!");
    }

    uart.println("[test_memory_management] big_address = {#}, small_address = {#}", big_address, small_address);
    if (big_address != small_address) {
        Processor::panic("Large free region was not re-sized correctly!");
    }

    uart.println("[test_memory_management] It looks like the large region was resized accordingly!");

    uart.println("[test_memory_management] Cleaning up...");

    MemoryManagement::instance().free(small_address);
    MemoryManagement::instance().print_stats();
}

void test_random_number_generation()
{
    auto uart = UART::instance();
    uart.println("[test_random_number_generation] Checking if the random number generator works...");

    auto random_number_a = random(0, 1000);
    uart.println("[test_random_number_generation] random_number_a = {i}", random_number_a);

    auto random_number_b = random(0, 10);
    uart.println("[test_random_number_generation] random_number_b = {i}", random_number_b);

    if (random_number_a == random_number_b) {
        uart.println("ERROR: Random number generator test failed. {i} = {i}!", random_number_a, random_number_b);

        return;
    }

    uart.println("[test_random_number_generation] It appears that the random number generator is working as expected!");
}

}
