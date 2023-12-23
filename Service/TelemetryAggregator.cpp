//
// SentinelHook Service - Telemetry Aggregator Implementation
//

#include "TelemetryAggregator.h"

//
// Constructor
//
TelemetryAggregator::TelemetryAggregator()
{
    m_Events.reserve(MAX_EVENTS);
}

//
// Destructor
//
TelemetryAggregator::~TelemetryAggregator()
{
    ClearEvents();
}

//
// Add Event
//
VOID TelemetryAggregator::AddEvent(const TELEMETRY_ENTRY& entry)
{
    std::lock_guard<std::mutex> lock(m_EventsMutex);

    if (m_Events.size() >= MAX_EVENTS) {
        // Remove oldest event (FIFO)
        m_Events.erase(m_Events.begin());
    }

    m_Events.push_back(entry);
}

//
// Process Events
//
VOID TelemetryAggregator::ProcessEvents()
{
    std::lock_guard<std::mutex> lock(m_EventsMutex);

    // Process events for correlation, filtering, etc.
    // In real implementation, would perform:
    // - Event correlation
    // - Anomaly detection
    // - Pattern matching
    // - Behavior modeling
}

//
// Get Events
//
std::vector<TELEMETRY_ENTRY> TelemetryAggregator::GetEvents()
{
    std::lock_guard<std::mutex> lock(m_EventsMutex);
    return m_Events;
}

//
// Clear Events
//
VOID TelemetryAggregator::ClearEvents()
{
    std::lock_guard<std::mutex> lock(m_EventsMutex);
    m_Events.clear();
}

