// telemetry helper functions
#include "sentinelhook.h"

// get process name from PID
NTSTATUS GetProcessName(
    _In_ HANDLE ProcessId,
    _Out_ PWCHAR ProcessName,
    _In_ ULONG NameBufferSize
)
{
    PEPROCESS process = NULL;
    PUNICODE_STRING imageName = NULL;
    NTSTATUS status;

    if (!ProcessName || NameBufferSize == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(ProcessName, NameBufferSize * sizeof(WCHAR));

    status = PsLookupProcessByProcessId(ProcessId, &process);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    imageName = PsGetProcessImageFileName(process);
    if (imageName && imageName->Buffer) {
        ULONG nameLength = min(imageName->Length / sizeof(WCHAR), NameBufferSize - 1);
        RtlCopyMemory(ProcessName, imageName->Buffer, nameLength * sizeof(WCHAR));
        ProcessName[nameLength] = L'\0';
        status = STATUS_SUCCESS;
    } else {
        status = STATUS_UNSUCCESSFUL;
    }

    ObDereferenceObject(process);
    return status;
}

// basic injection detection heuristic
// TODO: improve with more sophisticated checks
BOOLEAN IsProcessInjection(
    _In_ HANDLE ProcessId,
    _In_ PUNICODE_STRING ImageName
)
{
    UNREFERENCED_PARAMETER(ProcessId);

    if (!ImageName || !ImageName->Buffer) {
        return FALSE;
    }

    // check for temp paths (common injection location)
    UNICODE_STRING suspiciousPaths[] = {
        RTL_CONSTANT_STRING(L"\\Temp\\"),
        RTL_CONSTANT_STRING(L"\\AppData\\Local\\Temp\\"),
        RTL_CONSTANT_STRING(L"\\AppData\\Roaming\\"),
    };

    for (ULONG i = 0; i < ARRAYSIZE(suspiciousPaths); i++) {
        if (RtlFindUnicodeSubString(ImageName, &suspiciousPaths[i], TRUE, FALSE) != NULL) {
            return TRUE;
        }
    }

    return FALSE;
}

// check if driver is unsigned
// NOTE: this is a placeholder - real impl would verify cert chain
BOOLEAN IsUnsignedDriver(
    _In_ PUNICODE_STRING DriverPath
)
{
    if (!DriverPath || !DriverPath->Buffer) {
        return FALSE;
    }

    UNICODE_STRING system32 = RTL_CONSTANT_STRING(L"\\System32\\drivers\\");
    
    // heuristic: drivers outside system32 might be unsigned
    // TODO: implement proper signature verification using Wintrust APIs
    if (RtlFindUnicodeSubString(DriverPath, &system32, TRUE, FALSE) == NULL) {
        return TRUE;
    }

    return FALSE;
}

