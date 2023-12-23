// IOCTL handler for user-mode communication
#include "sentinelhook.h"

NTSTATUS DeviceIoControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _Inout_ PIRP Irp
)
{
    PIO_STACK_LOCATION ioStack;
    NTSTATUS status = STATUS_SUCCESS;
    ULONG ioControlCode;
    PVOID inputBuffer = NULL;
    PVOID outputBuffer = NULL;
    ULONG inputBufferLength = 0;
    ULONG outputBufferLength = 0;
    ULONG_PTR information = 0;

    UNREFERENCED_PARAMETER(DeviceObject);

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    switch (ioStack->MajorFunction) {
    case IRP_MJ_CREATE:
    case IRP_MJ_CLOSE:
        status = STATUS_SUCCESS;
        break;

    case IRP_MJ_DEVICE_CONTROL:
        ioControlCode = ioStack->Parameters.DeviceIoControl.IoControlCode;
        inputBuffer = Irp->AssociatedIrp.SystemBuffer;
        outputBuffer = Irp->AssociatedIrp.SystemBuffer;
        inputBufferLength = ioStack->Parameters.DeviceIoControl.InputBufferLength;
        outputBufferLength = ioStack->Parameters.DeviceIoControl.OutputBufferLength;

        switch (ioControlCode) {
        case IOCTL_SENTINELHOOK_GET_TELEMETRY:
            // TODO: implement proper ring buffer for telemetry queue
            if (outputBufferLength >= sizeof(TELEMETRY_ENTRY)) {
                RtlZeroMemory(outputBuffer, outputBufferLength);
                information = sizeof(TELEMETRY_ENTRY);
                status = STATUS_SUCCESS;
            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }
            break;

        case IOCTL_SENTINELHOOK_GET_STATS:
            if (outputBufferLength >= sizeof(TELEMETRY_STATS)) {
                KIRQL oldIrql;
                KeAcquireSpinLock(&g_DriverContext.StatsLock, &oldIrql);
                RtlCopyMemory(outputBuffer, &g_DriverContext.Stats, sizeof(TELEMETRY_STATS));
                KeReleaseSpinLock(&g_DriverContext.StatsLock, oldIrql);
                information = sizeof(TELEMETRY_STATS);
                status = STATUS_SUCCESS;
            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }
            break;

        case IOCTL_SENTINELHOOK_SET_FILTER:
            if (inputBufferLength >= sizeof(FILTER_CONFIG)) {
                PFILTER_CONFIG config = (PFILTER_CONFIG)inputBuffer;
                // TODO: apply filter rules
                UNREFERENCED_PARAMETER(config);
                status = STATUS_SUCCESS;
            } else {
                status = STATUS_INVALID_PARAMETER;
            }
            break;

        case IOCTL_SENTINELHOOK_ENABLE_MONITORING:
            g_DriverContext.MonitoringEnabled = TRUE;
            DebugPrint("Monitoring enabled via IOCTL");
            status = STATUS_SUCCESS;
            break;

        case IOCTL_SENTINELHOOK_DISABLE_MONITORING:
            g_DriverContext.MonitoringEnabled = FALSE;
            DebugPrint("Monitoring disabled via IOCTL");
            status = STATUS_SUCCESS;
            break;

        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }
        break;

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = information;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

