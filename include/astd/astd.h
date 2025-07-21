//
// Created by AmazingBuff on 2025/4/3.
//

#pragma once

#include "trait/functional.h"

#include "container/vector.h"
#include "container/dynamic_vector.h"
#include "container/string.h"
#include "container/list.h"
#include "container/queue.h"
#include "container/stack.h"
#include "container/map.h"
#include "container/set.h"
#include "container/ring.h"

#include "memory/pointer.h"

#include "algorithm/sort.h"
#include "algorithm/iter.h"

#include "sync/parallel.h"

AMAZING_NAMESPACE_BEGIN

static constexpr size_t Amazing_Hash = hash_str("Amazing", 0);

AMAZING_NAMESPACE_END

template <>
struct std::hash<Amazing::String>
{
    NODISCARD size_t operator()(const Amazing::String& str) const noexcept
    {
        return Amazing::hash_str(str.data(), str.size(), Amazing::Amazing_Hash);
    }
};