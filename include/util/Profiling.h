#ifndef PROFILING_H
#define PROFILING_H

#include <string>

#ifdef DEBUG_BUILD
#include "util/Logging.h"
#include "util/Clock.h"
class Profile {
public:
    static bool profiling_enabled;
    Profile (const std::string& name)
        : name(name)
        , start_time(Clock::now())
    {}
    ~Profile () {
        if (profiling_enabled) {
            auto duration = std::chrono::duration_cast<Time>(Clock::now() - start_time).count() * 1000;
            info("PROFILING -- {} = {:.6f} ms", name, duration);
        }
    }

private:
    const std::string name;
    const Clock::time_point start_time;
};
#else
class Profile {
public:
    Profile (const std::string& name) {}
};
#endif

#endif // PROFILING_H
