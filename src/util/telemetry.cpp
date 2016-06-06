#include "util/telemetry.h"

Telemetry::Counter_c::Counter_c (const std::string& name) : id(make_or_get(name))
{
}

unsigned Telemetry::Counter_c::make_or_get (const std::string& name)
{
    std::lock_guard<std::mutex> guard(mutex);
    auto it = counterIds.find(name);
    if (it != counterIds.end()) {
        return it->second;
    } else {
        auto id = counters.size();
        counterIds[name] = id;
        counters.emplace_back(new std::atomic_uint);
        return id;
    }
    mutex.unlock();

}

std::mutex Telemetry::Counter_c::mutex;
std::map<std::string, unsigned> Telemetry::Counter_c::counterIds;
std::vector<std::atomic_uint*> Telemetry::Counter_c::counters;

Telemetry::Gauge_c::Gauge_c(const std::string& name) : id(make_or_get(name))
{
}

unsigned Telemetry::Gauge_c::make_or_get (const std::string& name)
{
    std::lock_guard<std::mutex> guard(mutex);
    auto it = gaugeIds.find(name);
    if (it != gaugeIds.end()) {
        return it->second;
    } else {
        auto id = gauges.size();
        gaugeIds[name] = id;
        gauges.emplace_back(new std::atomic<float>);
        return id;
    }
    mutex.unlock();

}

std::mutex Telemetry::Gauge_c::mutex;
std::map<std::string, unsigned> Telemetry::Gauge_c::gaugeIds;
std::vector<std::atomic<float>*> Telemetry::Gauge_c::gauges;
