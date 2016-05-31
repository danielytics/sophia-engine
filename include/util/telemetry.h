#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <string>
#include <sstream>
#include <tuple>
#include <map>
#include <iostream>

template <typename T> constexpr T bitfield (T bit) { return 1 << bit; }

namespace Telemetry {
    enum TelemetryType {
        TerminateTelemetrySubsystem = 0,
        Frame   = bitfield(0),
        Gauge   = bitfield(1),
        Counter = bitfield(2),
        Debug   = bitfield(3),
        Info    = bitfield(4),
        Warn    = bitfield(5),
        Error   = bitfield(6),
    };

    namespace internal {

        struct FrameData {
            float time;
        };
        struct GaugeData {
            float change;
        };
        struct CounterData {
            unsigned increment;
        };
        struct TelemetryEvent {
            TelemetryType type;
            std::string name_or_message;
            union {
                FrameData frame;
                GaugeData gauge;
                CounterData counter;
            };
        };
        extern unsigned int enabled_telemetry_flags;

        void dispatch (const TelemetryEvent&& event);

        template <TelemetryType T, typename Arg, typename... Args>
        struct Recorder {
            static inline void record (Arg& arg, Args&... args) {
                std::ostringstream sstr;
                sstr << std::forward<Arg>(arg);
                using expander = int[];
                (void) expander{ (sstr << std::forward<Args>(args), void(), 0)... };
                TelemetryEvent event;
                event.type = T;
                event.name_or_message = std::move(sstr.str());
                dispatch(std::move(event));
            }
        };

        template <typename... Args>
        struct Recorder<Frame, Args...> {
            static inline void record (float frame_time) {
                TelemetryEvent event;
                event.type = Frame;
                event.frame.time = frame_time;
                dispatch(std::move(event));
            }
        };

        template <typename... Args>
        struct Recorder<Counter, Args...> {
            static inline void record (const std::string& name, unsigned increment) {
                TelemetryEvent event;
                event.type = Counter;
                event.name_or_message = name;
                event.counter.increment = increment;
                dispatch(std::move(event));
            }
        };

        template <typename... Args>
        struct Recorder<Gauge, Args...> {
            static inline void record (const std::string& name, float change) {
                TelemetryEvent event;
                event.type = Gauge;
                event.name_or_message = name;
                event.gauge.change = change;
                dispatch(std::move(event));
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

    inline std::string&& counter (const std::string& name)
    {
        std::string temp{"%c:"};
        temp += name;
        temp += ";";
        return std::move(temp);
    }

    inline std::string gauge (const std::string& name)
    {
        std::string temp{"%g:"};
        temp += name;
        temp += ";";
        return std::move(temp);
    }

    unsigned peekCounter (const std::string& name);
    float peekGauge (const std::string& name);

    void init ();
    void reset ();
    void term ();
}


#endif // TELEMETRY_H
