#ifndef LOGGING_H
#define LOGGING_H

#include <spdlog/spdlog.h>

namespace Logging {
    void init ();
    void term ();

    extern std::shared_ptr<spdlog::logger> logger;
}

#define LOG_FILELINE_FMT "({}:{}:{}) "

#ifdef SPDLOG_TRACE_ON
#define trace(fmt,...) Logging::logger->trace(LOG_FILELINE_FMT fmt, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
#define trace(...)
#endif

#ifdef SPDLOG_DEBUG_ON
#define debug(fmt,...) Logging::logger->debug(LOG_FILELINE_FMT fmt, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
#define debug(...)
#endif

#define info(fmt,...) Logging::logger->info(LOG_FILELINE_FMT fmt, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define warn(fmt,...) Logging::logger->warn(LOG_FILELINE_FMT fmt, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define error(fmt,...) Logging::logger->err(LOG_FILELINE_FMT fmt, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#endif // LOGGING_H
