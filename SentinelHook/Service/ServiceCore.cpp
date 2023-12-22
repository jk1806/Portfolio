// SentinelHook service implementation
// handles telemetry collection and aggregation

#include "ServiceCore.h"
#include "DriverComm.h"
#include "NamedPipe.h"
#include "ETWProvider.h"
#include "TelemetryAggregator.h"

#define SERVICE_NAME L"SentinelHookService"
#define SERVICE_DISPLAY_NAME L"SentinelHook Telemetry Service"

// Static instance pointer
SentinelHookService* SentinelHookService::s_pServiceInstance = nullptr;

SentinelHookService::SentinelHookService()
    : m_StopEvent(NULL)
    , m_IsRunning(FALSE)
{
    ZeroMemory(&m_ServiceStatus, sizeof(SERVICE_STATUS));
    m_ServiceStatusHandle = NULL;
}

SentinelHookService::~SentinelHookService()
{
    if (m_StopEvent) {
        CloseHandle(m_StopEvent);
    }
}

BOOL SentinelHookService::Initialize()
{
    m_StopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!m_StopEvent) {
        return FALSE;
    }

    m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    m_ServiceStatus.dwWin32ExitCode = NO_ERROR;
    m_ServiceStatus.dwServiceSpecificExitCode = 0;
    m_ServiceStatus.dwCheckPoint = 0;
    m_ServiceStatus.dwWaitHint = 0;

    return TRUE;
}

VOID WINAPI SentinelHookService::ServiceMain(DWORD argc, LPTSTR* argv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    if (!s_pServiceInstance) {
        return;
    }

    s_pServiceInstance->m_ServiceStatusHandle = RegisterServiceCtrlHandler(
        SERVICE_NAME,
        ServiceCtrlHandler
    );

    if (!s_pServiceInstance->m_ServiceStatusHandle) {
        return;
    }

    s_pServiceInstance->ReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
    s_pServiceInstance->ReportStatus(SERVICE_RUNNING, NO_ERROR, 0);

    s_pServiceInstance->ServiceWorkerThread();
}

VOID WINAPI SentinelHookService::ServiceCtrlHandler(DWORD ctrlCode)
{
    if (!s_pServiceInstance) {
        return;
    }

    switch (ctrlCode) {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        s_pServiceInstance->ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
        SetEvent(s_pServiceInstance->m_StopEvent);
        s_pServiceInstance->m_IsRunning = FALSE;
        break;
    default:
        break;
    }
}

VOID SentinelHookService::ReportStatus(DWORD currentState, DWORD win32ExitCode, DWORD waitHint)
{
    m_ServiceStatus.dwCurrentState = currentState;
    m_ServiceStatus.dwWin32ExitCode = win32ExitCode;
    m_ServiceStatus.dwWaitHint = waitHint;

    if (currentState == SERVICE_START_PENDING) {
        m_ServiceStatus.dwControlsAccepted = 0;
    } else {
        m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    }

    if ((currentState == SERVICE_RUNNING) || (currentState == SERVICE_STOPPED)) {
        m_ServiceStatus.dwCheckPoint = 0;
    } else {
        m_ServiceStatus.dwCheckPoint++;
    }

    SetServiceStatus(m_ServiceStatusHandle, &m_ServiceStatus);
}

VOID SentinelHookService::ServiceWorkerThread()
{
    m_IsRunning = TRUE;

    DriverComm driverComm;
    NamedPipe namedPipe;
    ETWProvider etwProvider;
    TelemetryAggregator aggregator;

    if (!driverComm.Initialize()) {
        ReportStatus(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR, 0);
        return;
    }

    if (!namedPipe.Initialize()) {
        ReportStatus(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR, 0);
        return;
    }

    if (!etwProvider.Initialize()) {
        ReportStatus(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR, 0);
        return;
    }

    // main loop
    while (m_IsRunning) {
        DWORD waitResult = WaitForSingleObject(m_StopEvent, 1000);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        driverComm.PollTelemetry();
        aggregator.ProcessEvents();
        etwProvider.WriteEvents(aggregator.GetEvents());
        namedPipe.SendTelemetry(aggregator.GetEvents());
    }

    etwProvider.Shutdown();
    namedPipe.Shutdown();
    driverComm.Shutdown();

    ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

VOID SentinelHookService::Run()
{
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { (LPWSTR)SERVICE_NAME, ServiceMain },
        { NULL, NULL }
    };

    s_pServiceInstance = this;
    StartServiceCtrlDispatcher(serviceTable);
}

VOID SentinelHookService::Stop()
{
    if (m_StopEvent) {
        SetEvent(m_StopEvent);
    }
    m_IsRunning = FALSE;
}

BOOL SentinelHookService::Install()
{
    SC_HANDLE scManager = NULL;
    SC_HANDLE serviceHandle = NULL;
    TCHAR servicePath[MAX_PATH];
    BOOL result = FALSE;

    if (GetModuleFileName(NULL, servicePath, MAX_PATH) == 0) {
        return FALSE;
    }

    scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (scManager == NULL) {
        return FALSE;
    }

    serviceHandle = CreateService(
        scManager,
        SERVICE_NAME,
        SERVICE_DISPLAY_NAME,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        servicePath,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );

    if (serviceHandle != NULL) {
        result = TRUE;
        CloseServiceHandle(serviceHandle);
    }

    CloseServiceHandle(scManager);
    return result;
}

BOOL SentinelHookService::Uninstall()
{
    SC_HANDLE scManager = NULL;
    SC_HANDLE serviceHandle = NULL;
    BOOL result = FALSE;

    scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (scManager == NULL) {
        return FALSE;
    }

    serviceHandle = OpenService(scManager, SERVICE_NAME, DELETE);
    if (serviceHandle != NULL) {
        if (DeleteService(serviceHandle)) {
            result = TRUE;
        }
        CloseServiceHandle(serviceHandle);
    }

    CloseServiceHandle(scManager);
    return result;
}

