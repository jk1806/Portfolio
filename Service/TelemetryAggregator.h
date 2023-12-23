//
// SentinelHook Service - Telemetry Aggregator Header
//

#pragma once

#include <windows.h>
#include <vector>
#include <mutex>
#include "..\Common\telemetry.h"

class TelemetryAggregator {
public:
    TelemetryAggregator();
    ~TelemetryAggregator();

    VOID AddEvent(const TELEMETRY_ENTRY& entry);
    VOID ProcessEvents();
    std::vector<TELEMETRY_ENTRY> GetEvents();
    VOID ClearEvents();

private:
    std::vector<TELEMETRY_ENTRY> m_Events;
    std::mutex m_EventsMutex;
    static const size_t MAX_EVENTS = 10000;
};

