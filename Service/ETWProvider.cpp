//
// SentinelHook Service - ETW Provider Implementation
//

#include "ETWProvider.h"
#include <iostream>

//
// Constructor
//
ETWProvider::ETWProvider()
    : m_ProviderHandle(0)
    , m_IsInitialized(FALSE)
{
}

//
// Destructor
//
ETWProvider::~ETWProvider()
{
    Shutdown();
}

//
// Initialize
//
BOOL ETWProvider::Initialize()
{
    ULONG status = EventRegister(
        &SentinelHookProviderGuid,
        NULL,
        NULL,
        &m_ProviderHandle
    );

    if (status != ERROR_SUCCESS) {
        // std::wcerr << L"ETW registration failed: " << status << std::endl;
        return FALSE;
    }

    m_IsInitialized = TRUE;
    return TRUE;
}

//
// Shutdown
//
VOID ETWProvider::Shutdown()
{
    if (m_IsInitialized && m_ProviderHandle != 0) {
        EventUnregister(m_ProviderHandle);
        m_ProviderHandle = 0;
        m_IsInitialized = FALSE;
    }
}

//
// Write Events
//
BOOL ETWProvider::WriteEvents(const std::vector<TELEMETRY_ENTRY>& events)
{
    if (!m_IsInitialized) {
        return FALSE;
    }

    for (const auto& entry : events) {
        switch (entry.EventType) {
        case EventFileCreate:
        case EventFileRead:
        case EventFileWrite:
        case EventFileDelete:
            WriteFileOperationEvent(entry.Data.FileEvent);
            break;

        case EventProcessCreate:
        case EventProcessTerminate:
        case EventProcessInjection:
            WriteProcessEvent(entry.Data.ProcessEvent);
            break;

        case EventImageLoad:
        case EventImageUnload:
        case EventUnsignedDriverLoad:
            WriteImageLoadEvent(entry.Data.ImageEvent);
            break;

        default:
            break;
        }
    }

    return TRUE;
}

//
// Write File Operation Event
//
VOID ETWProvider::WriteFileOperationEvent(const FILE_TELEMETRY& fileEvent)
{
    EVENT_DESCRIPTOR eventDescriptor = { 0 };
    eventDescriptor.Id = EVENT_FILE_OPERATION;
    eventDescriptor.Version = 1;
    eventDescriptor.Level = EVENT_LEVEL_INFO;
    eventDescriptor.Opcode = 0;
    eventDescriptor.Task = 0;
    eventDescriptor.Keyword = 0;

    EventWrite(
        m_ProviderHandle,
        &eventDescriptor,
        sizeof(fileEvent),
        &fileEvent
    );
}

//
// Write Process Event
//
VOID ETWProvider::WriteProcessEvent(const PROCESS_TELEMETRY& processEvent)
{
    EVENT_DESCRIPTOR eventDescriptor = { 0 };

    if (processEvent.EventType == EventProcessCreate) {
        eventDescriptor.Id = EVENT_PROCESS_CREATE;
    } else if (processEvent.EventType == EventProcessTerminate) {
        eventDescriptor.Id = EVENT_PROCESS_TERMINATE;
    } else if (processEvent.EventType == EventProcessInjection) {
        eventDescriptor.Id = EVENT_INJECTION_DETECTED;
        eventDescriptor.Level = EVENT_LEVEL_WARNING;
    } else {
        return;
    }

    eventDescriptor.Version = 1;
    eventDescriptor.Opcode = 0;
    eventDescriptor.Task = 0;
    eventDescriptor.Keyword = 0;

    EventWrite(
        m_ProviderHandle,
        &eventDescriptor,
        sizeof(processEvent),
        &processEvent
    );
}

//
// Write Image Load Event
//
VOID ETWProvider::WriteImageLoadEvent(const IMAGE_TELEMETRY& imageEvent)
{
    EVENT_DESCRIPTOR eventDescriptor = { 0 };

    if (imageEvent.EventType == EventUnsignedDriverLoad) {
        eventDescriptor.Id = EVENT_UNSIGNED_DRIVER;
        eventDescriptor.Level = EVENT_LEVEL_WARNING;
    } else {
        eventDescriptor.Id = EVENT_IMAGE_LOAD;
    }

    eventDescriptor.Version = 1;
    eventDescriptor.Opcode = 0;
    eventDescriptor.Task = 0;
    eventDescriptor.Keyword = 0;

    EventWrite(
        m_ProviderHandle,
        &eventDescriptor,
        sizeof(imageEvent),
        &imageEvent
    );
}

