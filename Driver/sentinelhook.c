// SentinelHook kernel driver
// WDK implementation for Windows 10/11
// started dec 2023

#include "sentinelhook.h"

// global context
DRIVER_CONTEXT g_DriverContext = { 0 };

// Filter registration structure
CONST FLT_OPERATION_REGISTRATION Callbacks[] = {
    { IRP_MJ_CREATE, 0, FilterPreOperation, FilterPostOperation },
    { IRP_MJ_WRITE, 0, FilterPreOperation, FilterPostOperation },
    { IRP_MJ_READ, 0, FilterPreOperation, FilterPostOperation },
    { IRP_MJ_SET_INFORMATION, 0, FilterPreOperation, FilterPostOperation },
    { IRP_MJ_OPERATION_END }
};

CONST FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),
    FLT_REGISTRATION_VERSION,
    0,
    NULL,
    Callbacks,
    FilterUnload,
    FilterInstanceSetup,
    FilterInstanceQueryTeardown,
    FilterInstanceTeardownStart,
    FilterInstanceTeardownComplete,
    NULL,
    NULL
};

//
// Driver Entry Point
//
NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    NTSTATUS status;
    UNICODE_STRING deviceName;
    UNICODE_STRING symbolicLinkName;
    PDEVICE_OBJECT deviceObject = NULL;

    UNREFERENCED_PARAMETER(RegistryPath);

    DebugPrint("SentinelHook loading v%d.%d.%d",
        SENTINELHOOK_VERSION_MAJOR,
        SENTINELHOOK_VERSION_MINOR,
        SENTINELHOOK_VERSION_BUILD);

    // init context
    RtlZeroMemory(&g_DriverContext, sizeof(DRIVER_CONTEXT));
    KeInitializeSpinLock(&g_DriverContext.StatsLock);
    KeInitializeSpinLock(&g_DriverContext.TelemetryLock);
    g_DriverContext.MonitoringEnabled = TRUE;

    // register filter
    status = FltRegisterFilter(DriverObject, &FilterRegistration, &g_DriverContext.FilterHandle);
    if (!NT_SUCCESS(status)) {
        DebugPrint("FltRegisterFilter failed: 0x%08X", status);
        return status;
    }

    // create device for IOCTL
    RtlInitUnicodeString(&deviceName, SENTINELHOOK_DEVICE_NAME);
    RtlInitUnicodeString(&symbolicLinkName, SENTINELHOOK_SYMBOLIC_LINK);

    status = IoCreateDevice(
        DriverObject,
        0,
        &deviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &deviceObject
    );

    if (!NT_SUCCESS(status)) {
        DebugPrint("Failed to create device object: 0x%08X", status);
        FltUnregisterFilter(g_DriverContext.FilterHandle);
        return status;
    }

    g_DriverContext.DeviceObject = deviceObject;
    g_DriverContext.DeviceName = deviceName;
    g_DriverContext.SymbolicLinkName = symbolicLinkName;

    // symlink for user-mode access
    status = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);
    if (!NT_SUCCESS(status)) {
        DebugPrint("symlink create failed: 0x%08X", status);
        IoDeleteDevice(deviceObject);
        FltUnregisterFilter(g_DriverContext.FilterHandle);
        return status;
    }

    // setup IOCTL handlers
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DeviceIoControl;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DeviceIoControl;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoControl;
    DriverObject->DriverUnload = DriverUnload;

    // hook process creation/termination
    status = PsSetCreateProcessNotifyRoutineEx(ProcessNotifyRoutine, FALSE);
    if (!NT_SUCCESS(status)) {
        DebugPrint("PsSetCreateProcessNotifyRoutineEx failed: 0x%08X", status);
        // continue anyway, file monitoring still works
    }

    // hook image loads (DLL/driver)
    status = PsSetLoadImageNotifyRoutine(ImageNotifyRoutine);
    if (!NT_SUCCESS(status)) {
        DebugPrint("PsSetLoadImageNotifyRoutine failed: 0x%08X", status);
    }

    // start the filter
    status = FltStartFiltering(g_DriverContext.FilterHandle);
    if (!NT_SUCCESS(status)) {
        DebugPrint("FltStartFiltering failed: 0x%08X", status);
        IoDeleteSymbolicLink(&symbolicLinkName);
        IoDeleteDevice(deviceObject);
        FltUnregisterFilter(g_DriverContext.FilterHandle);
        return status;
    }

    DebugPrint("SentinelHook loaded OK");
    return STATUS_SUCCESS;
}

// driver unload
VOID DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);

    DebugPrint("SentinelHook unloading...");

    // cleanup callbacks
    PsSetCreateProcessNotifyRoutineEx(ProcessNotifyRoutine, TRUE);
    PsRemoveLoadImageNotifyRoutine(ImageNotifyRoutine);

    if (g_DriverContext.FilterHandle) {
        FltStopFiltering(g_DriverContext.FilterHandle);
    }

    if (g_DriverContext.SymbolicLinkName.Buffer) {
        IoDeleteSymbolicLink(&g_DriverContext.SymbolicLinkName);
    }

    if (g_DriverContext.DeviceObject) {
        IoDeleteDevice(g_DriverContext.DeviceObject);
    }

    if (g_DriverContext.FilterHandle) {
        FltUnregisterFilter(g_DriverContext.FilterHandle);
    }

    DebugPrint("SentinelHook unloaded");
}

// filter instance setup callback
NTSTATUS FilterInstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(VolumeDeviceType);
    UNREFERENCED_PARAMETER(VolumeFilesystemType);

    // accept all volumes for now
    return STATUS_SUCCESS;
}

NTSTATUS FilterInstanceQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
    return STATUS_SUCCESS;
}

VOID FilterInstanceTeardownStart(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
}

VOID FilterInstanceTeardownComplete(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_TEARDOWN_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
}

NTSTATUS FilterUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(Flags);
    return STATUS_SUCCESS;
}

