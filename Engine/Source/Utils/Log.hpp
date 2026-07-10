#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace aio
{
    class Log
    {
    public:
        static void Init();
        static std::shared_ptr<spdlog::logger> sLogger;
    };
}

#if defined (AIO_DEBUG)

#define AIO_LOG_TRACE(...)       aio::Log::sLogger->trace(__VA_ARGS__)
#define AIO_LOG_INFO(...)        aio::Log::sLogger->info(__VA_ARGS__)
#define AIO_LOG_WARN(...)        aio::Log::sLogger->warn(__VA_ARGS__)
#define AIO_LOG_ERROR(...)       aio::Log::sLogger->error(__VA_ARGS__)
#define AIO_LOG_CRITICAL(...)    aio::Log::sLogger->critical(__VA_ARGS__)

#define AIO_ASSERT(x, ...)  if(!(x)) { aio::Log::sLogger->error("Assertion Failed: {0}", __VA_ARGS__); AIO_DEBUG_BREAK(); } 

#else

#define AIO_LOG_TRACE(...)
#define AIO_LOG_INFO(...)
#define AIO_LOG_WARN(...)
#define AIO_LOG_ERROR(...)
#define AIO_LOG_CRITICAL(...)

#define AIO_ASSERT(x, ...) x

#endif