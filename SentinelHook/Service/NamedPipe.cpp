//
// SentinelHook Service - Named Pipe Server Implementation
//

#include "NamedPipe.h"
// #include <iostream>

const WCHAR* NamedPipe::PIPE_NAME = L"\\\\.\\pipe\\SentinelHookTelemetry";

//
// Constructor
//
NamedPipe::NamedPipe()
    : m_PipeHandle(INVALID_HANDLE_VALUE)
    , m_IsInitialized(FALSE)
{
}

//
// Destructor
//
NamedPipe::~NamedPipe()
{
    Shutdown();
}

//
// Initialize
//
BOOL NamedPipe::Initialize()
{
    m_IsInitialized = TRUE;
    return TRUE;
}

//
// Shutdown
//
VOID NamedPipe::Shutdown()
{
    ClosePipe();
    m_IsInitialized = FALSE;
}

//
// Create Pipe Instance
//
BOOL NamedPipe::CreatePipeInstance()
{
    ClosePipe();

    m_PipeHandle = CreateNamedPipe(
        PIPE_NAME,
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1,
        64 * 1024,
        64 * 1024,
        0,
        NULL
    );

    if (m_PipeHandle == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    // Wait for client connection
    BOOL connected = ConnectNamedPipe(m_PipeHandle, NULL) ?
        TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    return connected;
}

//
// Close Pipe
//
VOID NamedPipe::ClosePipe()
{
    if (m_PipeHandle != INVALID_HANDLE_VALUE) {
        DisconnectNamedPipe(m_PipeHandle);
        CloseHandle(m_PipeHandle);
        m_PipeHandle = INVALID_HANDLE_VALUE;
    }
}

//
// Send Telemetry
//
BOOL NamedPipe::SendTelemetry(const std::vector<TELEMETRY_ENTRY>& events)
{
    if (!m_IsInitialized) {
        return FALSE;
    }

    // Create pipe instance if needed
    if (m_PipeHandle == INVALID_HANDLE_VALUE) {
        if (!CreatePipeInstance()) {
            return FALSE;
        }
    }

    // Send events
    DWORD bytesWritten = 0;
    for (const auto& entry : events) {
        BOOL result = WriteFile(
            m_PipeHandle,
            &entry,
            sizeof(TELEMETRY_ENTRY),
            &bytesWritten,
            NULL
        );

        if (!result) {
            DWORD error = GetLastError();
            if (error == ERROR_NO_DATA || error == ERROR_BROKEN_PIPE) {
                ClosePipe();
            }
            return FALSE;
        }
    }

    FlushFileBuffers(m_PipeHandle);
    return TRUE;
}

