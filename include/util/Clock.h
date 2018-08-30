#ifndef CLOCK_H
#define CLOCK_H

#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
typedef float Time_t;
typedef std::chrono::duration<Time_t> Time;

#endif // CLOCK_H
