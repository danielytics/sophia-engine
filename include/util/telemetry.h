#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <string>
#include <tuple>
#include <map>
#include <iostream>

template <typename T> constexpr T bitfield (T bit) { return 1 << bit; }

namespace Telemetry {
    enum TelemetryType {
        Frame   = bitfield(0),
        Guage   = bitfield(1),
        Counter = bitfield(2),
        Debug   = bitfield(3),
        Info    = bitfield(4),
        Warn    = bitfield(5),
    };

    struct FrameData {

    };
    struct GuageData {

    };
    struct CounterData {

    };

    namespace internal {
        extern unsigned int enabled_telemetry_flags;
        extern std::map<std::string, unsigned> counters;

        template <TelemetryType T, typename... Args>
        struct Recorder {
            static inline void record () {
                std::cout << "Generic recorder called for type " << T << " with " << sizeof...(Args) << " arguments.\n";
            }
        };

        template <typename... Args>
        struct Recorder<Frame, Args...> {
            static inline void record (Args... args) {
                std::cout << "Frame recorder called with " << sizeof...(Args) << " arguments.\n";
            }
        };

        template <typename... Args>
        struct Recorder<Counter, Args...> {
            static inline void record (const std::string& name, unsigned increment) {
                unsigned value = counters[name] += increment;
                std::cout << "Counter " << name << " incremented by " << increment << ". New value: " << value << "\n";
            }
        };

    }

    void enable (TelemetryType type, bool enabled);

    template <TelemetryType T, typename... Args>
    inline void record (Args... args) {
        if (internal::enabled_telemetry_flags & T) {
            internal::Recorder<T, Args...>::record(args...);
        }
    }
}


#endif // TELEMETRY_H
