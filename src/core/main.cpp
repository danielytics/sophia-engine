
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


int main(int argc, char *argv[])
{
    Telemetry::init();
    Telemetry::enable(Telemetry::Frame, true);
    Telemetry::enable(Telemetry::Counter, true);
    Telemetry::enable(Telemetry::Gauge, true);
    Telemetry::enable(Telemetry::Info, true);
    Telemetry::enable(Telemetry::Warn, true);
    Telemetry::enable(Telemetry::Error, true);

    Telemetry::record<Telemetry::Info>("water level: ", Telemetry::gauge("water"));
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Info>("frames: ", Telemetry::counter("frames"));
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Gauge>("water", 1);
    Telemetry::record<Telemetry::Frame>(12.4);
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Gauge>("water", -0.5);
    Telemetry::record<Telemetry::Info>("frames: ", Telemetry::counter("frames"));
    Telemetry::record<Telemetry::Frame>(16.333);
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Frame>(123);
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Counter>("frames", 1);
    Telemetry::record<Telemetry::Info>("frames: ", Telemetry::counter("frames"));
    Telemetry::record<Telemetry::Info>("water level: ", Telemetry::gauge("water"));

    Telemetry::term();
    return 0;
}

