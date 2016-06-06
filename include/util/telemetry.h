#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <mutex>
#include <map>
#include <string>
#include <vector>
#include <atomic>

namespace Telemetry {
    class Counter_c {
    public:
        Counter_c (const std::string& name);

        inline void inc (const unsigned by) const {
            (*counters[id]) += by;
        }

        inline void operator += (const unsigned by) const {
            (*counters[id]) += by;
        }

        inline void inc () const {
            ++(*counters[id]);
        }

        inline void operator++ () const {
            ++(*counters[id]);
        }

        inline void reset () const {
            (*counters[id]) = 0;
        }

        inline unsigned get () const {
            return counters[id]->load();
        }

    private:
        const unsigned id;
        static unsigned make_or_get (const std::string& name);
        static std::mutex mutex;
        static std::map<std::string, unsigned> counterIds;
        static std::vector<std::atomic_uint*> counters;
    };
    typedef const Counter_c Counter;

    class Gauge_c {
    public:
        Gauge_c (const std::string& name);

        inline void inc (const float by) const {
            auto atomic = gauges[id];
            auto current = atomic->load();
            while (! atomic->compare_exchange_weak(current, current + by)) {}
        }

        inline void operator += (const float by) const {
            inc(by);
        }

        inline void inc () const {
            inc(1.0);
        }

        inline void set (const float to) const {
            (*gauges[id]) = to;
        }

        inline void operator= (const float to) const {
            (*gauges[id]) = to;
        }

        inline void reset () const {
            (*gauges[id]) = 0;
        }

        inline float get () const {
            return gauges[id]->load();
        }

    private:
        const unsigned id;
        static unsigned make_or_get (const std::string& name);
        static std::mutex mutex;
        static std::map<std::string, unsigned> gaugeIds;
        static std::vector<std::atomic<float>*> gauges;
    };
    typedef const Gauge_c Gauge;

}

#endif // TELEMETRY_H
