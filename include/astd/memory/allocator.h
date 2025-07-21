#pragma once

#include "astd/base/define.h"

AMAZING_NAMESPACE_BEGIN

constexpr static size_t k_global_memory_size = 256 * 1024 * 1024;   // 256 MB
constexpr static size_t k_local_memory_size = 64 * 1024 * 1024;    // 64 MB
constexpr static size_t k_cache_alignment = 64;


void* allocate(size_t size, size_t alignment = k_cache_alignment, void* data = nullptr);
void* reallocate(void* p, size_t size, size_t alignment = k_cache_alignment, void* data = nullptr);
void deallocate(void* p);


template <typename Tp>
class Allocator
{
public:
    // only allocate memory, but not initialize
    static Tp* allocate(size_t count, size_t alignment = k_cache_alignment, void* data = nullptr)
    {
        return static_cast<Tp*>(Amazing::allocate(sizeof(Tp) * count, alignment, data));
    }

    // allocate memory near p with count, only allocate memory, but not initialize
    static Tp* reallocate(void* p, size_t count, size_t alignment = k_cache_alignment, void* data = nullptr)
    {
        return static_cast<Tp*>(Amazing::reallocate(p, sizeof(Tp) * count, alignment, data));
    }

    static void deallocate(Tp* p)
    {
        if (p == nullptr)
            return;

        if constexpr (std::is_destructible_v<Tp>)
            p->~Tp();

        Amazing::deallocate(p);
    }
};


#define PLACEMENT_NEW(type, size, ...) (new (Amazing::allocate(size)) type(__VA_ARGS__))
#define PLACEMENT_DELETE(type, p) {if constexpr (std::is_destructible_v<type>) if (p) p->~type(); Amazing::deallocate(p); p = nullptr;}
#define STACK_NEW(type, count) static_cast<type*>(alloca((count) * sizeof(type)))

AMAZING_NAMESPACE_END