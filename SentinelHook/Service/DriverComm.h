//
// SentinelHook Service - Driver Communication Header
//

#pragma once

#include <windows.h>
#include "..\Common\ioctl.h"
#include "..\Common\telemetry.h"

class DriverComm {
public:
    DriverComm();
    ~DriverComm();

    BOOL Initialize();
    VOID Shutdown();
    BOOL PollTelemetry();
    BOOL GetStatistics(PTELEMETRY_STATS stats);
    BOOL EnableMonitoring();
    BOOL DisableMonitoring();
    BOOL SetFilterConfig(PFILTER_CONFIG config);

private:
    HANDLE m_DeviceHandle;
    BOOL m_IsInitialized;

    BOOL OpenDevice();
    VOID CloseDevice();
};

