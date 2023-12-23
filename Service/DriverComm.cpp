//
// SentinelHook Service - Driver Communication Implementation
//

#include "DriverComm.h"
#include <iostream>

//
// Constructor
//
DriverComm::DriverComm()
    : m_DeviceHandle(INVALID_HANDLE_VALUE)
    , m_IsInitialized(FALSE)
{
}

//
// Destructor
//
DriverComm::~DriverComm()
{
    Shutdown();
}

//
// Initialize
//
BOOL DriverComm::Initialize()
{
    if (m_IsInitialized) {
        return TRUE;
    }

    if (!OpenDevice()) {
        return FALSE;
    }

    m_IsInitialized = TRUE;
    return TRUE;
}

//
// Shutdown
//
VOID DriverComm::Shutdown()
{
    if (m_IsInitialized) {
        CloseDevice();
        m_IsInitialized = FALSE;
    }
}

//
// Open Device
//
BOOL DriverComm::OpenDevice()
{
    m_DeviceHandle = CreateFile(
        SENTINELHOOK_USER_DEVICE,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (m_DeviceHandle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        // std::wcerr << L"Failed to open device: " << error << std::endl;
        return FALSE;
    }

    return TRUE;
}

//
// Close Device
//
VOID DriverComm::CloseDevice()
{
    if (m_DeviceHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_DeviceHandle);
        m_DeviceHandle = INVALID_HANDLE_VALUE;
    }
}

//
// Poll Telemetry
//
BOOL DriverComm::PollTelemetry()
{
    if (!m_IsInitialized || m_DeviceHandle == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD bytesReturned = 0;
    TELEMETRY_ENTRY entry = { 0 };

    BOOL result = DeviceIoControl(
        m_DeviceHandle,
        IOCTL_SENTINELHOOK_GET_TELEMETRY,
        NULL,
        0,
        &entry,
        sizeof(entry),
        &bytesReturned,
        NULL
    );

    if (result && bytesReturned > 0) {
        // Process telemetry entry
        // In real implementation, would queue for aggregation
        return TRUE;
    }

    return FALSE;
}

//
// Get Statistics
//
BOOL DriverComm::GetStatistics(PTELEMETRY_STATS stats)
{
    if (!m_IsInitialized || m_DeviceHandle == INVALID_HANDLE_VALUE || !stats) {
        return FALSE;
    }

    DWORD bytesReturned = 0;

    BOOL result = DeviceIoControl(
        m_DeviceHandle,
        IOCTL_SENTINELHOOK_GET_STATS,
        NULL,
        0,
        stats,
        sizeof(TELEMETRY_STATS),
        &bytesReturned,
        NULL
    );

    return result && (bytesReturned == sizeof(TELEMETRY_STATS));
}

//
// Enable Monitoring
//
BOOL DriverComm::EnableMonitoring()
{
    if (!m_IsInitialized || m_DeviceHandle == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD bytesReturned = 0;

    return DeviceIoControl(
        m_DeviceHandle,
        IOCTL_SENTINELHOOK_ENABLE_MONITORING,
        NULL,
        0,
        NULL,
        0,
        &bytesReturned,
        NULL
    );
}

//
// Disable Monitoring
//
BOOL DriverComm::DisableMonitoring()
{
    if (!m_IsInitialized || m_DeviceHandle == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DWORD bytesReturned = 0;

    return DeviceIoControl(
        m_DeviceHandle,
        IOCTL_SENTINELHOOK_DISABLE_MONITORING,
        NULL,
        0,
        NULL,
        0,
        &bytesReturned,
        NULL
    );
}

//
// Set Filter Config
//
BOOL DriverComm::SetFilterConfig(PFILTER_CONFIG config)
{
    if (!m_IsInitialized || m_DeviceHandle == INVALID_HANDLE_VALUE || !config) {
        return FALSE;
    }

    DWORD bytesReturned = 0;

    return DeviceIoControl(
        m_DeviceHandle,
        IOCTL_SENTINELHOOK_SET_FILTER,
        config,
        sizeof(FILTER_CONFIG),
        NULL,
        0,
        &bytesReturned,
        NULL
    );
}

