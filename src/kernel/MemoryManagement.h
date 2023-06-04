#pragma once

#include "../fluorescent/Optional.h"
#include "../types/integer.h"

namespace Kernel {

struct Region {
    void* start;
    size_t size;
    bool is_free;
};

class MemoryManagement {
public:
    static MemoryManagement& instance();

    void* allocate(size_t size);
    void free(void* pointer);

private:
    MemoryManagement()
    {
    }

    void* align(void* pointer)
    {
        auto address = (uintptr_t)pointer;
        auto alignment = 8;

        return (void*)((address + alignment - 1) & ~(alignment - 1));
    }

    Region find_next_free_region(size_t size);

    bool is_region_invalid(Region region);

    Optional<Region> m_last_allocated_region {};
};

}

void* operator new(size_t size);
