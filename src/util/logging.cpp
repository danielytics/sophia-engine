
#include "util/logging.h"

std::shared_ptr<spdlog::logger> Logging::logger;

void Logging::init () {
    size_t q_size = 8192; //queue size must be power of 2
    spdlog::set_async_mode(q_size);
    spdlog::set_level(spdlog::level::trace);
    Logging::logger = spdlog::stdout_logger_mt("console", true /*use color*/);
    spdlog::set_pattern("%l [%D %H/%M/%S:%f] %v");
}

void Logging::term () {
    spdlog::drop_all();
}
