// SentinelHook driver header
#pragma once

#include <ntddk.h>
#include <fltKernel.h>
#include <ntstrsafe.h>
#include "..\Common\ioctl.h"
#include "..\Common\telemetry.h"

// Driver version
#define SENTINELHOOK_VERSION_MAJOR   1
#define SENTINELHOOK_VERSION_MINOR   0
#define SENTINELHOOK_VERSION_BUILD   0

// Debug print macros
#if DBG
#define DebugPrint(format, ...) \
    DbgPrint("[SentinelHook] " format "\n", ##__VA_ARGS__)
#else
#define DebugPrint(format, ...)
#endif

// Global driver context
typedef struct _DRIVER_CONTEXT {
    PFLT_FILTER FilterHandle;
    PDEVICE_OBJECT DeviceObject;
    UNICODE_STRING DeviceName;
    UNICODE_STRING SymbolicLinkName;
    BOOLEAN MonitoringEnabled;
    TELEMETRY_STATS Stats;
    KSPIN_LOCK StatsLock;
    KSPIN_LOCK TelemetryLock;
} DRIVER_CONTEXT, *PDRIVER_CONTEXT;

// Function declarations
NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
);

VOID DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
);

// Filter callbacks
NTSTATUS FilterInstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
);

NTSTATUS FilterInstanceQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

VOID FilterInstanceTeardownStart(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

VOID FilterInstanceTeardownComplete(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
);

NTSTATUS FilterUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
);

NTSTATUS FilterInstanceQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
);

// File operation callbacks
FLT_PREOP_CALLBACK_STATUS FilterPreOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS FilterPostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
);

// Process and image notify callbacks
VOID ProcessNotifyRoutine(
    _Inout_ PEPROCESS Process,
    _In_ HANDLE ProcessId,
    _In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
);

VOID ImageNotifyRoutine(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_ HANDLE ProcessId,
    _In_ PIMAGE_INFO ImageInfo
);

// IOCTL handler
NTSTATUS DeviceIoControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
);

// Telemetry functions
VOID LogFileOperation(
    _In_ TELEMETRY_EVENT_TYPE EventType,
    _In_ PFLT_CALLBACK_DATA Data
);

VOID LogProcessEvent(
    _In_ TELEMETRY_EVENT_TYPE EventType,
    _In_ HANDLE ProcessId,
    _In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
);

VOID LogImageLoad(
    _In_ PUNICODE_STRING FullImageName,
    _In_ HANDLE ProcessId,
    _In_ PIMAGE_INFO ImageInfo
);

// Utility functions
NTSTATUS GetProcessName(
    _In_ HANDLE ProcessId,
    _Out_ PWCHAR ProcessName,
    _In_ ULONG NameBufferSize
);

BOOLEAN IsProcessInjection(
    _In_ HANDLE ProcessId,
    _In_ PUNICODE_STRING ImageName
);

BOOLEAN IsUnsignedDriver(
    _In_ PUNICODE_STRING DriverPath
);

// Global context
extern DRIVER_CONTEXT g_DriverContext;

