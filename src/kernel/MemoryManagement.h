#pragma once

#include "../fluorescent/Optional.h"
#include "../types/integer.h"

namespace Kernel {

struct Region {
    void* start;

    Region* next;

    size_t size;
    bool is_free;
};

class MemoryManagement {
public:
    static MemoryManagement& instance();

    void* allocate(size_t size);
    void free(void* pointer);

    void print_stats();

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

    Region allocate_new_region(size_t size);
    Optional<Region> find_next_free_region(size_t size);

    bool is_region_invalid(Region region);

    // Linked-list approach, see Region::next
    Region* m_first_region { nullptr };
    Optional<Region> m_last_allocated_region {};
    u64 m_bytes_allocated = 0;
    u64 m_bytes_freed = 0;
    u64 m_bytes_reused = 0;
};

}

void* operator new(size_t size);
void operator delete(void* pointer) noexcept;
