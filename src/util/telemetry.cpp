#include "util/telemetry.h"

#include <concurrentqueue.h>
#include <thread>
#include <iostream>
#include <iomanip>

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
typedef float Time_t;
typedef std::chrono::duration<Time_t> Time;


unsigned int Telemetry::internal::enabled_telemetry_flags = 0;
typedef  moodycamel::ConcurrentQueue<Telemetry::internal::TelemetryEvent> EventQueue;

namespace {
    EventQueue event_queue;
    std::thread telemetry_thread;
    Time_t startup;
    std::map<std::string, unsigned> counters;
    std::map<std::string, float> gauges;
}

template <typename T>
inline void replaceSpecial (std::string& input, const std::string& token, T getter)
{
    std::size_t pos = 0;
    std::size_t end = 0;
    std::string name;
    while ((pos = input.find(token, pos)) != std::string::npos) {
        end = input.find(";", pos + 3);
        name = std::move(input.substr(pos + 3, end - (pos + 3)));
        input.replace(pos, (end - pos) + 1, std::to_string(getter(name)));
    }
}

void telemetry_thread_fn (EventQueue& events)
{
    Telemetry::internal::TelemetryEvent event;
    const std::map<Telemetry::TelemetryType, std::string> log_levels{
        {Telemetry::Debug, "DEBUG"},
        {Telemetry::Info,  "INFO"},
        {Telemetry::Warn,  "WARN"},
        {Telemetry::Error, "ERROR"}
    };
    std::string message;
    std::size_t pos = 0;
    while (true) {
        if (events.try_dequeue(event)) {
            Time_t now = Time(Clock::now().time_since_epoch()).count() - startup;
            switch (event.type) {
            case Telemetry::TerminateTelemetrySubsystem:
                return;
            case Telemetry::Debug:
            case Telemetry::Info:
            case Telemetry::Warn:
            case Telemetry::Error:
                message = std::move(event.name_or_message);
                replaceSpecial(message, "%c:", Telemetry::peekCounter);
                replaceSpecial(message, "%g:", Telemetry::peekGauge);
                if (event.type == Telemetry::Error) {
                    std::cerr << "[ERROR] "
                              << std::fixed << std::setprecision(4) << std::setfill(' ') << now
                              << "  " << message << "\n";
                    // Now send a termination message to the global message bus.
                } else {
                    std::clog << "[" << log_levels.at(event.type) << "]  "
                              << std::fixed << std::setprecision(4) << std::setfill(' ') << now
                              << " " << message << "\n";
                }
                break;
            case Telemetry::Frame:
                std::clog << "[FRAME] "
                          << std::fixed << std::setprecision(4) << std::setfill(' ') << now
                          << " frame: " << counters["frames"] << " time: " << event.frame.time << "\n";
                break;
            case Telemetry::Counter:
            {
                counters[event.name_or_message] += event.counter.increment;
                break;
            }
            case Telemetry::Gauge:
            {
                auto val = gauges[event.name_or_message] += event.gauge.change;
                break;
            }
            default:
                std::cout << "Got event " << event.type << ": " << event.name_or_message << "\n";
                break;
            }
        } else {
            std::this_thread::yield();
        }
    }
}

void Telemetry::init ()
{
    startup = Time(Clock::now().time_since_epoch()).count();
    telemetry_thread = std::thread(telemetry_thread_fn, std::ref(event_queue));
    Telemetry::reset();
}

void Telemetry::reset ()
{
    counters.clear();
}

void Telemetry::term ()
{
    Telemetry::internal::TelemetryEvent event;
    event.type = Telemetry::TerminateTelemetrySubsystem;
    event_queue.enqueue(std::move(event));
    telemetry_thread.join();
}

void Telemetry::internal::dispatch (const TelemetryEvent&& event)
{
    event_queue.enqueue(std::move(event));
}

unsigned Telemetry::peekCounter (const std::string& name)
{
    auto it = counters.find(name);
    if (it != counters.end()) {
        return it->second;
    } else {
        Telemetry::record<Telemetry::Warn>("Counter `", name, "` used before creation.");
        return 0;
    }
}

float Telemetry::peekGauge (const std::string& name)
{
    auto it = gauges.find(name);
    if (it != gauges.end()) {
        return it->second;
    } else {
        Telemetry::record<Telemetry::Warn>("Guage `", name, "` used before creation.");
        return 0;
    }
}

void Telemetry::enable (TelemetryType type, bool enabled)
{
    if (enabled) {
        internal::enabled_telemetry_flags |= type;
    } else {
        internal::enabled_telemetry_flags &= ~type;
    }
}
