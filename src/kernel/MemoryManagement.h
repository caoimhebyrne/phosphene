#pragma once

#include "../types/integer.h"

namespace Kernel {

struct Region {
    void* start;
    void* end;

    size_t size;

    bool is_free;
};

class MemoryManagement {
public:
    static MemoryManagement& instance();

    void* allocate(size_t size);

private:
    MemoryManagement()
        : m_last_allocated_region({ .start = nullptr, .end = nullptr, .size = 0, .is_free = false })
    {
    }

    Region find_next_free_region(size_t size);

    bool is_region_invalid(Region region);

    Region m_last_allocated_region;
};

}
