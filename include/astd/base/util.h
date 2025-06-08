#pragma once

#include <type_traits>
#include "macro.h"

AMAZING_NAMESPACE_BEGIN

template<typename Tp>
    requires(std::is_integral_v<Tp>)
constexpr Tp align_to(const Tp value, const Tp alignment)
{
    return (value + alignment - 1) / alignment * alignment;
}

template<typename Tp>
    requires(std::is_integral_v<Tp>)
constexpr Tp division_up(const Tp value, const Tp divisor)
{
    return (value + divisor - 1) / divisor;
}

template<typename Tp, size_t N>
constexpr size_t array_size(const Tp(&)[N])
{
    return N;
}

template<typename Tp, size_t N>
    requires(std::is_integral_v<Tp>)
constexpr size_t hash_str(const Tp(&str)[N], const size_t& seed)
{
    size_t hash = seed;
    for (size_t i = 0; i < N - 1; ++i)
        hash = (hash ^ static_cast<size_t>(str[i])) * 16777619ull;

    return hash;
}

template<typename Tp>
    requires(std::is_integral_v<Tp>)
constexpr size_t hash_str(const Tp* str, const size_t len, const size_t& seed)
{
    size_t hash = seed;
    for (size_t i = 0; i < len; ++i)
        hash = (hash ^ static_cast<size_t>(str[i])) * 16777619ull;

    return hash;
}

template<typename Tp, typename... Rest>
constexpr void hash_combine_mul(size_t& seed, const Tp& val, const Rest&... rest)
{
    if constexpr (std::is_convertible_v<Tp, size_t>)
        seed ^= (static_cast<size_t>(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    else
        seed ^= (std::hash<Tp>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    (hash_combine_mul(seed, rest), ...);
}

template<typename Tp>
constexpr size_t hash_combine(const size_t& seed, const Tp& val)
{
    if constexpr (std::is_convertible_v<Tp, size_t>)
        return seed ^ (static_cast<size_t>(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    else
        return seed ^ (std::hash<Tp>()(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

inline size_t hash_combine(const size_t& seed, const void* mem, const size_t& length)
{
    uint8_t const* bytes = static_cast<uint8_t const*>(mem);
    size_t hash = seed;
    for (size_t i = 0; i < length; ++i)
        hash = (hash ^ static_cast<size_t>(bytes[i])) * 16777619ull;

    return hash;
}


template<typename Tp>
    requires(std::is_integral_v<Tp>)
constexpr size_t count_bits(const Tp value)
{
    static constexpr size_t m1 = 0x5555555555555555; //binary: 0101...
    static constexpr size_t m2 = 0x3333333333333333; //binary: 00110011..
    static constexpr size_t m4 = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
    static constexpr size_t m8 = 0x00ff00ff00ff00ff; //binary:  8 zeros,  8 ones ...
    static constexpr size_t m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...
    static constexpr size_t m32 = 0x00000000ffffffff; //binary: 32 zeros, 32 ones ...

    size_t val = static_cast<size_t>(value);
    val = (val & m1) + ((val >> 1) & m1); //put count of each  2 bits into those  2 bits
    val = (val & m2) + ((val >> 2) & m2); //put count of each  4 bits into those  4 bits
    val = (val & m4) + ((val >> 4) & m4); //put count of each  8 bits into those  8 bits
    val = (val & m8) + ((val >> 8) & m8); //put count of each 16 bits into those 16 bits
    val = (val & m16) + ((val >> 16) & m16); //put count of each 32 bits into those 32 bits
    val = (val & m32) + ((val >> 32) & m32); //put count of each 64 bits into those 64 bits
    return val;
}

template<typename Tp>
void swap(Tp& l, Tp& r) noexcept
{
    Tp tmp = std::move(l);
    l = std::move(r);
    r = std::move(tmp);
}

AMAZING_NAMESPACE_END