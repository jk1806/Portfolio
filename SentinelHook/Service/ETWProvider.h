//
// SentinelHook Service - ETW Provider Header
//

#pragma once

#include <windows.h>
#include <evntrace.h>
#include <vector>
#include "..\Common\telemetry.h"
#include "..\Common\events.h"

class ETWProvider {
public:
    ETWProvider();
    ~ETWProvider();

    BOOL Initialize();
    VOID Shutdown();
    BOOL WriteEvents(const std::vector<TELEMETRY_ENTRY>& events);

private:
    REGHANDLE m_ProviderHandle;
    BOOL m_IsInitialized;

    VOID WriteFileOperationEvent(const FILE_TELEMETRY& fileEvent);
    VOID WriteProcessEvent(const PROCESS_TELEMETRY& processEvent);
    VOID WriteImageLoadEvent(const IMAGE_TELEMETRY& imageEvent);
};

