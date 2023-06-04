#include "MemoryManagement.h"
#include "Processor.h"
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

    UART::instance().println("[MemoryManagement] Allocated {i} bytes. ({#} -> {#})", region.size, region.start, (u8*)region.start + region.size);

    return region.start;
}

// Our memory management approach has a pretty big issue at the moment...
// We don't fill the gaps of memory that has been free()'d, meaning that we will just exponentially grow,
// meaning that free() has no real benefits apart from scrubbing out sensitive data.
void MemoryManagement::free(void* pointer)
{
    if (pointer == nullptr) {
        return;
    }

    auto region_pointer = (u8*)pointer - sizeof(Region);
    auto region = (Region*)region_pointer;

    // We don't do anything with invalid regions
    if (region->start == nullptr || region->size == 0 || region->is_free) {
        return;
    }

    // Mark the region as free
    region->is_free = true;

    // Scrub out the data
    for (auto i = 0; i < region->size; i++) {
        *((u8*)region->start + i) = 0;
    }
}

Region MemoryManagement::find_next_free_region(size_t size)
{
    // This has gotten quite messy, and there's a bit of alignment and stuff involved,
    // but hey, it works!

    int* start_position = nullptr;
    if (m_last_allocated_region) {
        auto last_allocated_region = m_last_allocated_region.get();

        // FIXME: Why wasn't region.end working here?
        start_position = (int*)last_allocated_region.start + sizeof(Region) + last_allocated_region.size;
    } else {
        UART::instance().println("[MemoryManagement] Last region was invalid! Starting from the end of the BSS.");
        start_position = (int*)&__bss_end;
    }

    if (start_position == nullptr) {
        Processor::panic("MemoryManagement::find_next_free_region failed to locate a valid start position!");
        return {};
    }

    // TODO: Check if we are near the peripheral base address.
    //       It's not something we want to overwrite, but we should be fine for now.

    auto aligned_start = align(start_position);
    auto region = Region {
        .start = (u8*)aligned_start + sizeof(Region),
        // FIXME: Why wasn't region.end working here?
        // .end = .start + size
        .size = size,
        .is_free = false
    };

    *(Region*)aligned_start = region;
    m_last_allocated_region = region;
    return region;
}

}

void* operator new(size_t size)
{
    return Kernel::MemoryManagement::instance().allocate(size);
}
