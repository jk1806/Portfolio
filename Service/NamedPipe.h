//
// SentinelHook Service - Named Pipe Server Header
//

#pragma once

#include <windows.h>
#include <vector>
#include "..\Common\telemetry.h"

class NamedPipe {
public:
    NamedPipe();
    ~NamedPipe();

    BOOL Initialize();
    VOID Shutdown();
    BOOL SendTelemetry(const std::vector<TELEMETRY_ENTRY>& events);

private:
    HANDLE m_PipeHandle;
    BOOL m_IsInitialized;
    static const WCHAR* PIPE_NAME;

    BOOL CreatePipeInstance();
    VOID ClosePipe();
};

