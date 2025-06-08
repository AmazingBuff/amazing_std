//
// Created by AmazingBuff on 2025/4/3.
//

#pragma once

#include "vector.h"

AMAZING_NAMESPACE_BEGIN

template <typename Tp>
    requires(std::is_same_v<char, Tp> || std::is_same_v<wchar_t, Tp>)
size_t str_length(const Tp* str)
{
    if constexpr (std::is_same_v<char, Tp>)
        return std::strlen(str);
    else
        return std::wcslen(str);
}

template <typename Tp>
    requires(std::is_same_v<char, Tp> || std::is_same_v<wchar_t, Tp>)
int str_compare(const Tp* str1, const Tp* str2, size_t length)
{
    if constexpr (std::is_same_v<char, Tp>)
        return std::strncmp(str1, str2, length);
    else
        return std::wcsncmp(str1, str2, length);
}

INTERNAL_NAMESPACE_BEGIN

template <typename Tp, template <typename> typename Alloc = Allocator>
class StringT : public Vector<Tp, Alloc>
{
    using Str = Vector<Tp, Alloc>;
    using allocator = Alloc<Tp>;
    using allocator_next = Alloc<size_t>;
public:
    StringT()
    {
        Str::m_size = 0;
        Str::m_capacity = Str::m_size + 1;
        Str::m_data = allocator::allocate(Str::m_capacity);
        Str::m_data[Str::m_size] = '\0';
    }

    StringT(const Tp* str)
    {
        Str::m_size = str_length(str);
        Str::m_capacity = Str::m_size + 1;
        Str::m_data = allocator::allocate(Str::m_capacity);
        std::memcpy(Str::m_data, str, Str::m_size * sizeof(Tp));
        Str::m_data[Str::m_size] = '\0';
    }

    StringT(const StringT& str)
    {
        Str::m_size = str.m_size;
        Str::m_capacity = str.m_capacity;
        Str::m_data = allocator::allocate(Str::m_capacity);
        std::memcpy(Str::m_data, str.m_data, Str::m_size * sizeof(Tp));
        Str::m_data[Str::m_size] = '\0';
    }

    StringT(StringT&& str) noexcept
    {
        Str::swap(str);
    }

    StringT& operator=(const Tp* str)
    {
        size_t size = str_length(str);
        if (Str::m_size == size && str_compare(Str::m_data, str, str_length(str)) == 0)
            return *this;

        Str::m_size = str_length(str);
        if (Str::m_capacity < Str::m_size + 1)
            Str::reserve(Str::m_size + 1);
        std::memcpy(Str::m_data, str, Str::m_size * sizeof(Tp));
        Str::m_data[Str::m_size] = '\0';

        return *this;
    }

    StringT& operator=(const StringT& str)
    {
        if (*this == str)
            return *this;

        Str::m_size = str.m_size;
        if (Str::m_capacity < Str::m_size + 1)
            Str::reserve(Str::m_size + 1);
        std::memcpy(Str::m_data, str.m_data, Str::m_size * sizeof(Tp));
        Str::m_data[Str::m_size] = '\0';

        return *this;
    }

    StringT& operator=(StringT&& str) noexcept
    {
        Str::swap(str);

        return *this;
    }

    NODISCARD size_t find(const Tp* str) const
    {
        size_t size = str_length(str);
        return find(str, size);
    }

    NODISCARD size_t find(const StringT& str) const
    {
        return find(str.m_data, str.m_size);
    }

    NODISCARD const Tp* c_str() const
    {
        return Str::m_data;
    }

    NODISCARD StringT substr(size_t pos, size_t count) const
    {
        StringT ret;
        if (pos < Str::m_size)
        {
            size_t len = count;
            if (pos + count > Str::m_size)
                len = Str::m_size - pos;
            std::memcpy(ret.m_data, Str::m_data + pos, len * sizeof(Tp));
        }

        return ret;
    }

    NODISCARD StringT operator+(const StringT& other) const
    {
        StringT str;
        str.reserve(Str::m_size + other.m_size + 1);
        str.resize(Str::m_size + other.m_size);
        std::memcpy(str.m_data, Str::m_data, Str::m_size * sizeof(Tp));
        std::memcpy(str.m_data + Str::m_size, other.m_data, other.m_size * sizeof(Tp));
        str.m_data[str.m_size] = '\0';
        return str;
    }

    NODISCARD StringT operator+(const Tp* other) const
    {
        size_t size = str_length(other);

        StringT str;
        str.reserve(Str::m_size + size + 1);
        str.resize(Str::m_size + size);
        std::memcpy(str.m_data, Str::m_data, Str::m_size * sizeof(Tp));
        std::memcpy(str.m_data + Str::m_size, other, size * sizeof(Tp));
        str.m_data[str.m_size] = '\0';
        return str;
    }

    StringT& operator+=(const StringT& other)
    {
        if (Str::m_size + other.m_size + 1 > Str::m_capacity)
            Str::reserve(Str::m_size + other.m_size + 1);
        std::memcpy(Str::m_data + Str::m_size, other.m_data, other.m_size * sizeof(Tp));
        Str::m_size += other.m_size;
        Str::m_data[Str::m_size] = '\0';
        return *this;
    }

    StringT& operator+=(const Tp* other)
    {
        size_t size = str_length(other);
        if (Str::m_size + size + 1 > Str::m_capacity)
            Str::reserve(Str::m_size + size + 1);

        std::memcpy(Str::m_data + Str::m_size, other, size * sizeof(Tp));
        Str::m_size += size;
        Str::m_data[Str::m_size] = '\0';
        return *this;
    }

    NODISCARD bool operator<(const StringT& other) const
    {
        size_t min_size = std::min(Str::m_size, other.m_size);
        int ret = str_compare(Str::m_data, other.m_data, min_size);
        if (ret < 0 || (ret == 0 && Str::m_size < other.m_size))
            return true;

        return false;
    }

    NODISCARD bool operator>(const StringT& other) const
    {
        size_t min_size = std::min(Str::m_size, other.m_size);
        int ret = str_compare(Str::m_data, other.m_data, min_size);
        if (ret > 0 || (ret == 0 && Str::m_size > other.m_size))
            return true;

        return false;
    }

    NODISCARD bool operator==(const StringT& other) const
    {
        if (Str::m_size != other.m_size)
            return false;
        for (size_t i = 0; i < Str::m_size; i++)
        {
            if (Str::m_data[i] != other.m_data[i])
                return false;
        }
        return true;
    }

    NODISCARD bool operator!=(const StringT& other) const
    {
        return !(*this == other);
    }

    friend StringT operator+(const Tp* lhs, const StringT& rhs)
    {
        size_t size = str_length(lhs);
        StringT<Tp, Alloc> str;

        str.reserve(size + rhs.m_size + 1);
        str.resize(size + rhs.m_size);
        std::memcpy(str.m_data, lhs, size * sizeof(Tp));
        std::memcpy(str.m_data + size, rhs.m_data, rhs.m_size * sizeof(Tp));
        str.m_data[str.m_size] = '\0';
        return str;
    }


public:
    static constexpr size_t end_pos = std::numeric_limits<size_t>::max();

private:
    size_t find(const Tp* str, size_t size) const
    {
        if (size > Str::m_size || size <= 0)
            return end_pos;

        // next array
        size_t len = 0;

        Vector<size_t> next(size);
        next[0] = len;

        size_t index = 1;
        while (index < size)
        {
            if (str[len] == str[index])
            {
                ++len;
                next[index] = len;
                index++;
            }
            else
            {
                if (len == 0)
                {
                    next[index] = len;
                    index++;
                }
                else
                    len = next[len - 1];
            }
        }

        // kmp
        size_t i = 0, j = 0;
        while (i < Str::m_size && j < size)
        {
            if (Str::m_data[i] == str[j])
            {
                i++;
                j++;
            }
            else
            {
                if (j == 0)
                    i++;
                else
                    j = next[j - 1];
            }
        }

        return j == size ? i - j : end_pos;
    }
};


template <typename Char, typename Tp>
StringT<Char> to_str(const Tp& value)
{
    Char buf[21];
    Char* end = buf + 20;
    *end = '\0';

    if constexpr (std::is_integral_v<Tp>)
    {
        using Up = std::make_signed_t<Tp>;
        const Up up = static_cast<Up>(value);

        Up val = up;
        if (value < 0)
            val = -up;

        do
        {
            *--end = static_cast<Char>('0' + val % 10);
            val /= 10;
        } while (val != 0);

        if (value < 0)
            *--end = static_cast<Char>('-');

        return StringT<Char>(end);
    }

    return {};
}


INTERNAL_NAMESPACE_END

using String = Internal::StringT<char>;
using WString = Internal::StringT<wchar_t>;

template <typename Tp>
String to_str(const Tp& value)
{
    return Internal::to_str<char>(value);
}

template <typename Tp>
WString to_wstr(const Tp& value)
{
    return Internal::to_str<wchar_t>(value);
}

template <>
struct Less<const char*>
{
    NODISCARD bool operator()(const char* lhs, const char* rhs) const noexcept
    {
        size_t l_len = str_length(lhs);
        size_t r_len = str_length(rhs);
        int val = str_compare(lhs, rhs, std::min(l_len, r_len));
        if (val == 0)
            return l_len < r_len;
        return val < 0;
    }
};

template <>
struct Less<const wchar_t*>
{
    NODISCARD bool operator()(const wchar_t* lhs, const wchar_t* rhs) const noexcept
    {
        size_t l_len = str_length(lhs);
        size_t r_len = str_length(rhs);
        int val = str_compare(lhs, rhs, std::min(l_len, r_len));
        if (val == 0)
            return l_len < r_len;
        return val < 0;
    }
};

AMAZING_NAMESPACE_END