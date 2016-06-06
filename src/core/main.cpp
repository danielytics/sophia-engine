
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <concurrentqueue.h>
#include <lua.hpp>

#include <iostream>

#include "window/window.h"
#include "util/telemetry.h"
#include "util/config.h"
#include "util/logging.h"

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
typedef double Time_t;
typedef std::chrono::duration<Time_t> Time;

int main(int argc, char *argv[])
{
    Logging::init();
    {
        Window window("Bento");
        window.open();

        window.run();
    }
    Logging::term();
    return 0;
}

