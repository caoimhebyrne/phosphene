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
    auto existing_region = this->find_next_free_region(size);
    if (existing_region != nullptr) {
        UART::instance().println("[MemoryManagement] Reused {i} bytes. ({#} -> {#})", existing_region->size, existing_region->start, (u8*)existing_region->start + existing_region->size);
        return existing_region->start;
    }

    auto region = this->allocate_new_region(size);
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

Region MemoryManagement::allocate_new_region(size_t size)
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
        .next = nullptr,
        // FIXME: Why wasn't region.end working here?
        // .end = .start + size
        .size = size,
        .is_free = false
    };

    *(Region*)aligned_start = region;
    m_last_allocated_region = region;

    if (m_first_region != nullptr) {
        m_first_region->next = (Region*)aligned_start;
    }

    m_first_region = (Region*)aligned_start;
    return region;
}

Region* MemoryManagement::find_next_free_region(size_t size)
{
    if (m_first_region == nullptr) {
        UART::instance().println("[MemoryManagement] Failed to find existing region to adopt as m_first_region was null!");
        return nullptr;
    }

    for (auto region = m_first_region; region != nullptr; region = region->next) {
        if (!region->is_free) {
            continue;
        }

        UART::instance().println("[MemoryManagement] Checking if the region is suitable: \\{ start = {#}, next = {#}, size = {i}, is_free = {b} \\}", region->start, region->next, region->size, region->is_free);

        // If this region is too small, we can't use it for anything.
        if (region->size < size) {
            continue;
        }

        // If the region is the same size as our requirement, we can just adopt it!
        if (region->size == size) {
            region->is_free = false;
            return region;
        }

        Processor::panic("FIXME: Re-using larger regions by shrinking them has not been implemented yet.");
    }

    return nullptr;
}

}

void* operator new(size_t size)
{
    return Kernel::MemoryManagement::instance().allocate(size);
}

void operator delete(void* pointer) noexcept
{
    return Kernel::MemoryManagement::instance().free(pointer);
}
