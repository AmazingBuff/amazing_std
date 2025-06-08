//
// Created by AmazingBuff on 2025/4/2.
//

#pragma once

#include <exception>
#include "define.h"

AMAZING_NAMESPACE_BEGIN

enum class AStdError
{
    NO_ENOUGH_MEMORY,
    NO_APPLICABLE_POSITION,
    NO_VALID_PARAMETER,
};

class AStdException final : public std::exception
{
public:
    explicit AStdException(AStdError&& error) : m_error(error) {}
    ~AStdException() override = default;

    NODISCARD const char* what() const noexcept override
    {
        switch (m_error)
        {
        case AStdError::NO_ENOUGH_MEMORY: return "Not enough memory";
        case AStdError::NO_VALID_PARAMETER: return "Invalid argument";
        case AStdError::NO_APPLICABLE_POSITION: return "Not applicable position";
        }

        return "Unknown error";
    }
private:
    AStdError m_error;
};


AMAZING_NAMESPACE_END
