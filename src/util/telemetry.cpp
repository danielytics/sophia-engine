#include "util/telemetry.h"

unsigned int Telemetry::internal::enabled_telemetry_flags = 0;
std::map<std::string, unsigned> Telemetry::internal::counters;

void Telemetry::enable (TelemetryType type, bool enabled)
{
    if (enabled) {
        internal::enabled_telemetry_flags |= type;
    } else {
        internal::enabled_telemetry_flags &= ~type;
    }
}
