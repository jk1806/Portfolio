// process and image load callbacks
#include "sentinelhook.h"

// process notify callback - called on process create/terminate
VOID ProcessNotifyRoutine(
    _Inout_ PEPROCESS Process,
    _In_ HANDLE ProcessId,
    _In_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
    KIRQL oldIrql;

    UNREFERENCED_PARAMETER(Process);

    if (!g_DriverContext.MonitoringEnabled) {
        return;
    }

    if (CreateInfo) {
        // new process
        PROCESS_TELEMETRY processTelemetry = { 0 };

        processTelemetry.EventType = EventProcessCreate;
        processTelemetry.Timestamp = KeQueryInterruptTime();
        processTelemetry.ProcessId = HandleToUlong(ProcessId);
        processTelemetry.ParentProcessId = HandleToUlong(CreateInfo->ParentProcessId);
        processTelemetry.ThreadId = HandleToUlong(CreateInfo->CreatingThreadId.UniqueProcess);
        processTelemetry.CreateTime = CreateInfo->CreationTime.QuadPart;

        // Get process name
        if (CreateInfo->ImageFileName) {
            ULONG nameLength = min(CreateInfo->ImageFileName->Length / sizeof(WCHAR), MAX_PROCESS_NAME_LENGTH - 1);
            RtlCopyMemory(
                processTelemetry.ProcessName,
                CreateInfo->ImageFileName->Buffer,
                nameLength * sizeof(WCHAR)
            );
            processTelemetry.ProcessName[nameLength] = L'\0';
        }

        // Get image path
        if (CreateInfo->ImageFileName) {
            ULONG pathLength = min(CreateInfo->ImageFileName->Length / sizeof(WCHAR), MAX_PATH_LENGTH - 1);
            RtlCopyMemory(
                processTelemetry.ImagePath,
                CreateInfo->ImageFileName->Buffer,
                pathLength * sizeof(WCHAR)
            );
            processTelemetry.ImagePath[pathLength] = L'\0';
        }

        // check for injection patterns
        if (IsProcessInjection(ProcessId, CreateInfo->ImageFileName)) {
            processTelemetry.EventType = EventProcessInjection;
            
            KeAcquireSpinLock(&g_DriverContext.StatsLock, &oldIrql);
            g_DriverContext.Stats.InjectionDetections++;
            KeReleaseSpinLock(&g_DriverContext.StatsLock, oldIrql);
            
            DebugPrint("WARNING: Potential injection detected PID=%d", HandleToUlong(ProcessId));
        }

        // Update statistics
        KeAcquireSpinLock(&g_DriverContext.StatsLock, &oldIrql);
        g_DriverContext.Stats.TotalEvents++;
        g_DriverContext.Stats.ProcessEvents++;
        KeReleaseSpinLock(&g_DriverContext.StatsLock, oldIrql);

        // DebugPrint("Proc create: PID=%d, Name=%ws", processTelemetry.ProcessId, processTelemetry.ProcessName);
    } else {
        // process exit
        PROCESS_TELEMETRY processTelemetry = { 0 };

        processTelemetry.EventType = EventProcessTerminate;
        processTelemetry.Timestamp = KeQueryInterruptTime();
        processTelemetry.ProcessId = HandleToUlong(ProcessId);

        GetProcessName(ProcessId, processTelemetry.ProcessName, MAX_PROCESS_NAME_LENGTH);

        KeAcquireSpinLock(&g_DriverContext.StatsLock, &oldIrql);
        g_DriverContext.Stats.TotalEvents++;
        g_DriverContext.Stats.ProcessEvents++;
        KeReleaseSpinLock(&g_DriverContext.StatsLock, oldIrql);

        // DebugPrint("Proc exit: PID=%d", processTelemetry.ProcessId);
    }
}

// image load callback - DLL or driver loading
VOID ImageNotifyRoutine(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_ HANDLE ProcessId,
    _In_ PIMAGE_INFO ImageInfo
)
{
    IMAGE_TELEMETRY imageTelemetry = { 0 };
    KIRQL oldIrql;

    if (!g_DriverContext.MonitoringEnabled) {
        return;
    }

    // driver if ProcessId is NULL
    BOOLEAN isDriver = (ProcessId == NULL);

    imageTelemetry.EventType = EventImageLoad;
    imageTelemetry.Timestamp = KeQueryInterruptTime();
    imageTelemetry.ProcessId = ProcessId ? HandleToUlong(ProcessId) : 0;
    imageTelemetry.ImageBase = (ULONG64)ImageInfo->ImageBase;
    imageTelemetry.ImageSize = ImageInfo->ImageSize;
    imageTelemetry.IsDriver = isDriver;

    // Get image path
    if (FullImageName && FullImageName->Buffer) {
        ULONG pathLength = min(FullImageName->Length / sizeof(WCHAR), MAX_PATH_LENGTH - 1);
        RtlCopyMemory(
            imageTelemetry.ImagePath,
            FullImageName->Buffer,
            pathLength * sizeof(WCHAR)
        );
        imageTelemetry.ImagePath[pathLength] = L'\0';
    }

    // Get process name if not a driver
    if (ProcessId) {
        GetProcessName(ProcessId, imageTelemetry.ProcessName, MAX_PROCESS_NAME_LENGTH);
    }

    // check driver signature
    if (isDriver && FullImageName) {
        if (IsUnsignedDriver(FullImageName)) {
            imageTelemetry.EventType = EventUnsignedDriverLoad;
            
            KeAcquireSpinLock(&g_DriverContext.StatsLock, &oldIrql);
            g_DriverContext.Stats.UnsignedDriverDetections++;
            KeReleaseSpinLock(&g_DriverContext.StatsLock, oldIrql);

            DebugPrint("ALERT: Unsigned driver load: %ws", FullImageName->Buffer);
        }
    }

    // Update statistics
    KeAcquireSpinLock(&g_DriverContext.StatsLock, &oldIrql);
    g_DriverContext.Stats.TotalEvents++;
    g_DriverContext.Stats.ImageEvents++;
    KeReleaseSpinLock(&g_DriverContext.StatsLock, oldIrql);

    // DebugPrint("Image load: %s, PID=%d", isDriver ? "Driver" : "DLL", imageTelemetry.ProcessId);
}

