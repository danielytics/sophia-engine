
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <concurrentqueue.h>
#include <lua.hpp>

#include "util/telemetry.h"
#include "util/config.h"

#include <vector>
#include <thread>

#include <string>
#include <iostream>
#include <atomic>
#include <mutex>

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
typedef double Time_t;
typedef std::chrono::duration<Time_t> Time;



int main(int argc, char *argv[])
{
    Telemetry::Counter foo("foo");
    foo.inc();
    std::cout << foo.get() << "\n";
    foo += 2;
    std::cout << foo.get() << "\n";
    ++foo;
    std::cout << foo.get() << "\n";
    Telemetry::Counter bar("foo");
    bar.inc(3);
    std::cout << foo.get() << "\n";
    std::cout << "\n";

    Telemetry::Gauge a("a");
    a += 0.4;
    std::cout << a.get() << "\n";
    a = 5.2;
    std::cout << a.get() << "\n";
    Telemetry::Gauge b("a");
    b.inc(3.1);
    std::cout << a.get() << "\n";

    // Initialise timekeeping
    float frame_time;
    auto start_time = Clock::now();
    auto previous_time = start_time;
    auto current_time = start_time;
    Telemetry::Counter frames("frames");

    // Run the main processing loop
    while (true)
    {
        if (frames.get() == 100) break;

        // Update timekeeping
        previous_time = current_time;
        current_time = Clock::now();
        frame_time = std::chrono::duration_cast<Time>(current_time - previous_time).count();
        frames.inc();
        std::cout << frames.get() << " " << frame_time << "\n";
    }

    return 0;
}

