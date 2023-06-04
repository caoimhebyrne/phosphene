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
    if (!m_last_allocated_region) {
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

    auto last_region = m_last_allocated_region.get();
    auto last_end = ((int*)last_region.end);

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
