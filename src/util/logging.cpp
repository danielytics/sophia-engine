
#include "util/logging.h"

std::shared_ptr<spdlog::logger> Logging::logger;

void Logging::init (const YAML::Node& config_node) {
    // Async logging in release mode, sync logging in debug mode
    // In a debug build, we want to make sure everything gets logged before a crash
    // In a release build, we would like things to be logged, but performance is more important
#ifdef NDEBUG
    size_t q_size = 8192; //queue size must be power of 2
    spdlog::set_async_mode(q_size);
#endif
    std::string log_level;
     using namespace Config;
     auto parser = make_parser(
        map("telemetry",
            scalar("logging", log_level)
        )
    );
    parser(config_node);
    std::map<std::string,spdlog::level::level_enum> log_levels{
        {"trace", spdlog::level::trace},
        {"debug", spdlog::level::debug},
        {"info", spdlog::level::info},
        {"warn", spdlog::level::warn},
        {"error", spdlog::level::err},
        {"No", spdlog::level::off},
    };
    // TODO: Error checking for invalid values of log_level
    spdlog::set_level(log_levels[log_level]);
    Logging::logger = spdlog::stdout_logger_mt("console", true /*use color*/);
    spdlog::set_pattern("%l [%D %H/%M/%S:%f] %v");
}

void Logging::term () {
    spdlog::drop_all();
}
