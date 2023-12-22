// mini-filter for file system monitoring
#include "sentinelhook.h"

// pre-op callback - intercept file operations
FLT_PREOP_CALLBACK_STATUS FilterPreOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    if (!g_DriverContext.MonitoringEnabled) {
        return FLT_PREOP_SUCCESS_WITH_CALLBACK;
    }

    switch (Data->Iopb->MajorFunction) {
    case IRP_MJ_CREATE:
        if (FlagOn(Data->Iopb->Parameters.Create.Options, FILE_DELETE_ON_CLOSE)) {
            LogFileOperation(EventFileDelete, Data);
        } else {
            LogFileOperation(EventFileCreate, Data);
        }
        break;

    case IRP_MJ_WRITE:
        LogFileOperation(EventFileWrite, Data);
        break;

    case IRP_MJ_READ:
        LogFileOperation(EventFileRead, Data);
        break;

    case IRP_MJ_SET_INFORMATION:
        if (Data->Iopb->Parameters.SetFileInformation.FileInformationClass == FileDispositionInformation) {
            PFILE_DISPOSITION_INFORMATION dispositionInfo = 
                (PFILE_DISPOSITION_INFORMATION)Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
            if (dispositionInfo && dispositionInfo->DeleteFile) {
                LogFileOperation(EventFileDelete, Data);
            }
        }
        break;
    }

    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

// post-op callback
FLT_POSTOP_CALLBACK_STATUS FilterPostOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_opt_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    // Post-operation processing if needed
    // Currently, we log in pre-operation for simplicity

    return FLT_POSTOP_FINISHED_PROCESSING;
}

// log file operation to telemetry
VOID LogFileOperation(
    _In_ TELEMETRY_EVENT_TYPE EventType,
    _In_ PFLT_CALLBACK_DATA Data
)
{
    FILE_TELEMETRY fileTelemetry = { 0 };
    PEPROCESS process = NULL;
    HANDLE processId = NULL;
    KIRQL oldIrql;

    // get process info
    process = IoThreadToProcess(Data->Thread);
    if (process) {
        processId = PsGetProcessId(process);
        fileTelemetry.ProcessId = HandleToUlong(processId);
        fileTelemetry.ThreadId = HandleToUlong(PsGetThreadId(Data->Thread));
    }

    // extract file path
    if (Data->Iopb->TargetFileObject) {
        PFLT_FILE_NAME_INFORMATION nameInfo = NULL;
        NTSTATUS status = FltGetFileNameInformation(
            Data,
            FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT,
            &nameInfo
        );

        if (NT_SUCCESS(status) && nameInfo) {
            status = FltParseFileNameInformation(nameInfo);
            if (NT_SUCCESS(status)) {
                ULONG nameLength = min(nameInfo->Name.Length / sizeof(WCHAR), MAX_PATH_LENGTH - 1);
                RtlCopyMemory(
                    fileTelemetry.FilePath,
                    nameInfo->Name.Buffer,
                    nameLength * sizeof(WCHAR)
                );
                fileTelemetry.FilePath[nameLength] = L'\0';
            }
            FltReleaseFileNameInformation(nameInfo);
        }
    }

    if (processId) {
        GetProcessName(processId, fileTelemetry.ProcessName, MAX_PROCESS_NAME_LENGTH);
    }

    fileTelemetry.EventType = EventType;
    fileTelemetry.Timestamp = KeQueryInterruptTime();
    fileTelemetry.OperationFlags = Data->Iopb->OperationFlags;
    fileTelemetry.Result = NT_SUCCESS(Data->IoStatus.Status) ? 0 : Data->IoStatus.Status;

    // update stats
    KeAcquireSpinLock(&g_DriverContext.StatsLock, &oldIrql);
    g_DriverContext.Stats.TotalEvents++;
    g_DriverContext.Stats.FileEvents++;
    KeReleaseSpinLock(&g_DriverContext.StatsLock, oldIrql);

    // TODO: queue telemetry for user-mode retrieval
    // currently just logging, need to implement ring buffer
    DebugPrint("File op: Type=%d, PID=%d, Path=%ws",
        EventType, fileTelemetry.ProcessId, fileTelemetry.FilePath);
}

