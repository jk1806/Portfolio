//
// SentinelHook Service - Core Service Header
//

#pragma once

#include <windows.h>
#include <winsvc.h>
#include <string>

class SentinelHookService {
public:
    SentinelHookService();
    ~SentinelHookService();

    BOOL Initialize();
    VOID Run();
    VOID Stop();
    BOOL Install();
    BOOL Uninstall();

private:
    static VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
    static VOID WINAPI ServiceCtrlHandler(DWORD ctrlCode);
    static SentinelHookService* s_pServiceInstance;

    SERVICE_STATUS m_ServiceStatus;
    SERVICE_STATUS_HANDLE m_ServiceStatusHandle;
    HANDLE m_StopEvent;
    BOOL m_IsRunning;

    VOID ReportStatus(DWORD currentState, DWORD win32ExitCode, DWORD waitHint);
    VOID ServiceWorkerThread();
};

