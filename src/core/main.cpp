
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <concurrentqueue.h>
#include <lua.hpp>

#include "util/telemetry.h"

int main(int argc, char *argv[])
{
    Telemetry::record<Telemetry::Frame>(1);
    Telemetry::enable(Telemetry::Frame, true);
    Telemetry::enable(Telemetry::Counter, true);
    Telemetry::record<Telemetry::Frame>(1, 2);
    Telemetry::enable(Telemetry::Frame, false);
    Telemetry::record<Telemetry::Frame>(1, 2, 3);
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Counter>("frames", 5);
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Counter>("frames", 2);

    moodycamel::ConcurrentQueue<int> queue;
    queue.enqueue(8);

    int item;
    if (queue.try_dequeue(item)) {
        std::cout << item << "\n";
    } else {
        std::cout << "No items in queue\n";
    }
    return 0;
}

