#include "MemoryManagement.h"
#include "io/UART.h"

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

    UART::instance().println("[MemoryManagement] Allocated {i} bytes.", region.size);

    return region.end;
}

Region MemoryManagement::find_next_free_region(size_t size)
{
    // If the last region was invalid...
    if (m_last_allocated_region.end == nullptr) {
        UART::instance().println("[MemoryManagement] Last region was invalid! Starting at the beginning...");

        // TODO: Find a better starting address
        auto region = Region {
            .start = (void*)0x2000,
            .end = (void*)(0x2000 + size),
            .size = size,
            .is_free = false,
        };

        m_last_allocated_region = region;
        return region;
    }

    auto last_end = ((int*)m_last_allocated_region.end);
    auto region = Region {
        .start = last_end,
        .end = last_end + size,
        .size = size,
        .is_free = false
    };

    m_last_allocated_region = region;
    return region;
}

}
