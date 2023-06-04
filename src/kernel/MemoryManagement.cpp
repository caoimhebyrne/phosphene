#include "MemoryManagement.h"
#include "io/UART.h"

// Defined in the linker script
extern "C" u8 __bss_end;

namespace Kernel {

MemoryManagement& MemoryManagement::instance()
{
    static MemoryManagement instance;
    return instance;
}

void* MemoryManagement::allocate(size_t size)
{
    auto region = this->find_next_free_region(size);
    region.is_free = false;

    UART::instance().println("[MemoryManagement] Allocated {i} bytes. ({#} -> {#})", region.size, region.start, region.end);

    return region.end;
}

Region MemoryManagement::find_next_free_region(size_t size)
{
    // This has gotten quite messy, and there's a bit of alignment and stuff involved,
    // but hey, it works!

    int* start_position = nullptr;
    if (m_last_allocated_region) {
        auto last_allocated_region = m_last_allocated_region.get();
        start_position = (int*)last_allocated_region.end;
    } else {
        UART::instance().println("[MemoryManagement] Last region was invalid! Starting from the end of the BSS.");
        start_position = (int*)&__bss_end;
    }

    if (start_position == nullptr) {
        UART::instance().println("[MemoryManagement] ERROR: Failed to locate a valid start position!");

        // TODO: We should introduce the concept of a panic...
        while (true) {
            asm volatile("wfi");
        }

        return {};
    }

    // TODO: Check if we are near the peripheral base address.
    //       It's not something we want to overwrite, but we should be fine for now.

    auto aligned_start = align(start_position);
    auto region = Region {
        .start = aligned_start,
        .end = (u8*)aligned_start + size,
        .size = size,
        .is_free = false
    };

    m_last_allocated_region = region;
    return region;
}

}

void* operator new(size_t size)
{
    return Kernel::MemoryManagement::instance().allocate(size);
}
