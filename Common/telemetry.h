//
// SentinelHook - Telemetry Data Structures
// Shared between kernel driver and user-mode service
//

#pragma once

#include <windows.h>

// Event types
typedef enum _TELEMETRY_EVENT_TYPE {
    EventFileCreate = 0,
    EventFileRead,
    EventFileWrite,
    EventFileDelete,
    EventProcessCreate,
    EventProcessTerminate,
    EventImageLoad,
    EventImageUnload,
    EventProcessInjection,
    EventUnsignedDriverLoad,
    EventMax
} TELEMETRY_EVENT_TYPE;

// File operation telemetry
typedef struct _FILE_TELEMETRY {
    TELEMETRY_EVENT_TYPE EventType;
    ULONG64 Timestamp;
    ULONG ProcessId;
    ULONG ThreadId;
    WCHAR FilePath[MAX_PATH_LENGTH];
    WCHAR ProcessName[MAX_PROCESS_NAME_LENGTH];
    ULONG OperationFlags;
    ULONG Result;
} FILE_TELEMETRY, *PFILE_TELEMETRY;

// Process telemetry
typedef struct _PROCESS_TELEMETRY {
    TELEMETRY_EVENT_TYPE EventType;
    ULONG64 Timestamp;
    ULONG ProcessId;
    ULONG ParentProcessId;
    ULONG ThreadId;
    WCHAR ProcessName[MAX_PROCESS_NAME_LENGTH];
    WCHAR ImagePath[MAX_PATH_LENGTH];
    ULONG64 CreateTime;
    BOOLEAN IsSigned;
} PROCESS_TELEMETRY, *PPROCESS_TELEMETRY;

// Image load telemetry
typedef struct _IMAGE_TELEMETRY {
    TELEMETRY_EVENT_TYPE EventType;
    ULONG64 Timestamp;
    ULONG ProcessId;
    ULONG ThreadId;
    WCHAR ImagePath[MAX_PATH_LENGTH];
    WCHAR ProcessName[MAX_PROCESS_NAME_LENGTH];
    ULONG64 ImageBase;
    ULONG ImageSize;
    BOOLEAN IsSigned;
    BOOLEAN IsDriver;
} IMAGE_TELEMETRY, *PIMAGE_TELEMETRY;

// Unified telemetry structure
typedef struct _TELEMETRY_ENTRY {
    TELEMETRY_EVENT_TYPE EventType;
    ULONG64 Timestamp;
    ULONG Size;
    union {
        FILE_TELEMETRY FileEvent;
        PROCESS_TELEMETRY ProcessEvent;
        IMAGE_TELEMETRY ImageEvent;
        UCHAR RawData[512];
    } Data;
} TELEMETRY_ENTRY, *PTELEMETRY_ENTRY;

// Statistics structure
typedef struct _TELEMETRY_STATS {
    ULONG64 TotalEvents;
    ULONG64 FileEvents;
    ULONG64 ProcessEvents;
    ULONG64 ImageEvents;
    ULONG64 InjectionDetections;
    ULONG64 UnsignedDriverDetections;
    ULONG64 DroppedEvents;
    ULONG64 BufferOverflows;
} TELEMETRY_STATS, *PTELEMETRY_STATS;

// Filter configuration
typedef struct _FILTER_CONFIG {
    BOOLEAN MonitorFileOperations;
    BOOLEAN MonitorProcessCreation;
    BOOLEAN MonitorImageLoads;
    BOOLEAN DetectInjections;
    BOOLEAN DetectUnsignedDrivers;
    WCHAR ExcludedPaths[10][MAX_PATH_LENGTH];
    ULONG ExcludedPathCount;
} FILTER_CONFIG, *PFILTER_CONFIG;

