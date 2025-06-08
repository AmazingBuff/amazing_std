#pragma once

#include "macro.h"
#include <type_traits>

AMAZING_NAMESPACE_BEGIN

enum class AResult
{


    e_succeed = 0,
};




template <typename Tp>
struct Equal
{
    bool operator()(const Tp& lhs, const Tp& rhs) const noexcept
    {
        return lhs == rhs;
    }
};

template <typename Tp>
struct Less
{
    NODISCARD bool operator()(const Tp& lhs, const Tp& rhs) const noexcept
    {
        return lhs < rhs;
    }
};

template <typename Tp, typename Up>
class Pair
{
public:
    Pair() : first(Tp()), second(Up()) {}

    template<typename OtherT, typename OtherU>
        requires(std::is_convertible_v<OtherT, Tp>&& std::is_convertible_v<OtherU, Up>)
    Pair(OtherT&& f, OtherU&& s) : first(std::forward<OtherT>(f)), second(std::forward<OtherU>(s)) {}

    template<typename OtherT, typename OtherU>
        requires(std::is_convertible_v<OtherT, Tp>&& std::is_convertible_v<OtherU, Up>)
    explicit Pair(Pair<OtherT, OtherU>&& other) : first(std::forward<OtherT>(other.first)), second(std::forward<OtherU>(other.second)) {}

    template<typename OtherT, typename OtherU>
        requires(std::is_convertible_v<OtherT, Tp>&& std::is_convertible_v<OtherU, Up>)
    Pair& operator=(const Pair<OtherT, OtherU>& other)
    {
        first = other.first;
        second = other.second;
        return *this;
    }

    template<typename OtherT, typename OtherU>
        requires(std::is_convertible_v<OtherT, Tp>&& std::is_convertible_v<OtherU, Up>)
    Pair& operator=(Pair<OtherT, OtherU>&& other)
    {
        first = std::forward<OtherT>(other.first);
        second = std::forward<OtherU>(other.second);
        return *this;
    }
public:
    Tp first;
    Up second;
};


AMAZING_NAMESPACE_END