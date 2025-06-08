#pragma once

#include "macro.h"
#include <format>
#include <string>
#include <iostream>

AMAZING_NAMESPACE_BEGIN

class Logger
{
public:
	enum class Level
	{
		e_info,
		e_warning,
		e_error
	};

	template<typename... Args>
	static void log(const Level& level, const char* loc, const std::string& message, Args&&... args);
};

template<typename... Args>
void Logger::log(const Level& level, const char* loc, const std::string& message, Args&&... args)
{
	std::string msg;
	if constexpr (sizeof...(Args) > 0)
		msg = std::vformat(message, std::make_format_args(args...));
	else
		msg = message;
	switch (level)
	{
	case Level::e_info:
		std::cout << "[" << loc << ", Info]: " << msg << std::endl;
		break;
	case Level::e_warning:
		std::cerr << "[" << loc << ", Warning]: " << msg << std::endl;
		break;
	case Level::e_error:
		const std::string format = std::format("[{}, Error]: {}", loc, msg);
		std::cerr << format << std::endl;
		throw std::runtime_error(format);
	}
}

#if defined(_DEBUG) || defined(DEBUG)
#define LOG_INFO(loc, msg, ...)		Logger::log(Logger::Level::e_info, loc, msg __VA_OPT__(,) __VA_ARGS__)
#define LOG_WARNING(loc, msg, ...)	Logger::log(Logger::Level::e_warning, loc, msg __VA_OPT__(,) __VA_ARGS__)
#define LOG_ERROR(loc, msg, ...)		Logger::log(Logger::Level::e_error, loc, msg __VA_OPT__(,) __VA_ARGS__)

#define ASSERT(expr, loc, msg, ...)  if (!(expr)) LOG_ERROR(loc, msg, __VA_ARGS__);
#else
#define LOG_INFO(loc, msg, ...)	reinterpret_cast<void*>(0)
#define LOG_WARNING(loc, msg, ...) reinterpret_cast<void*>(0)
#define LOG_ERROR(loc, msg, ...)	reinterpret_cast<void*>(0)

#define ASSERT(expr, loc, msg, ...) (void)(expr)
#endif

#define CONTAINER_LOG_INFO(msg, ...)		LOG_INFO("Container", msg, __VA_ARGS__)
#define CONTAINER_LOG_WARNING(msg, ...)	LOG_WARNING("Container", msg, __VA_ARGS__)
#define CONTAINER_LOG_ERROR(msg, ...)	LOG_ERROR("Container", msg, __VA_ARGS__)
#define CONTAINER_ASSERT(expr, msg, ...)	ASSERT(expr, "Container", msg, __VA_ARGS__)


#define RENDERING_LOG_INFO(msg, ...)		LOG_INFO("Rendering", msg, __VA_ARGS__)
#define RENDERING_LOG_WARNING(msg, ...)	LOG_WARNING("Rendering", msg, __VA_ARGS__)
#define RENDERING_LOG_ERROR(msg, ...)	LOG_ERROR("Rendering", msg, __VA_ARGS__)
#define RENDERING_ASSERT(expr, msg, ...)	ASSERT(expr, "Rendering", msg, __VA_ARGS__)



AMAZING_NAMESPACE_END