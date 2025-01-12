# SentinelHook - Windows Endpoint Telemetry Agent

Windows endpoint telemetry agent for security research. Combines kernel-mode and user-mode components for process and file system monitoring.

## Overview

Security research project for endpoint monitoring:
- **Kernel-mode mini-filter driver** for low-level file system monitoring
- **Process and DLL injection detection** via kernel callbacks
- **User-mode service** for telemetry aggregation and analysis
- **ETW integration** for event correlation and behavior modeling

## Architecture

### Kernel-Mode Components
- **Mini-Filter Driver** (`sentinelhook.sys`): Intercepts IRPs for file operations (create, read, write, delete)
- **Process Notify Callbacks**: Detects process creation/termination via `PsSetCreateProcessNotifyRoutineEx`
- **Image Load Notify Callbacks**: Monitors DLL and driver loads via `PsSetLoadImageNotifyRoutine`

### User-Mode Components
- **Telemetry Service** (`SentinelHookService.exe`): Aggregates and processes telemetry data
- **IOCTL Communication**: Secure communication channel with kernel driver
- **Named Pipes**: Inter-process communication for data export
- **ETW Provider**: Event Tracing for Windows integration

## Features

-  Real-time file system monitoring (create, read, write, delete)
-  Process creation/termination detection
-  DLL and driver load monitoring
-  Process injection detection
-  Unsigned driver load detection
-  PatchGuard-safe implementation
-  DSE-compliant (Driver Signature Enforcement)
-  ETW event correlation
-  Windows 10/11 x64 support

## Build Requirements

- **Visual Studio 2022** (with C++ Desktop Development workload)
- **Windows Driver Kit (WDK) 10** or later
- **Windows SDK** 10.0.22621.0 or later
- **Windows 10/11 x64** development machine
- **Test Signing** enabled (for development) or valid code signing certificate (for production)

## Building the Project

### Prerequisites
1. Install Visual Studio 2022 with C++ Desktop Development
2. Install Windows Driver Kit (WDK)
3. Enable test signing (for development):
   ```cmd
   bcdedit /set testsigning on
   ```
   (Requires administrator privileges and reboot)

### Build Steps

1. Open `SentinelHook.sln` in Visual Studio 2022
2. Select **x64** platform
3. Build Solution (Ctrl+Shift+B)
4. Output files:
   - `Driver/x64/Debug/sentinelhook.sys` - Kernel driver
   - `Service/x64/Debug/SentinelHookService.exe` - User-mode service
   - `Driver/x64/Debug/sentinelhook.inf` - Driver installation file

### Manual Build (Command Line)

```cmd
# Build driver
cd Driver
msbuild sentinelhook.vcxproj /p:Configuration=Debug /p:Platform=x64

# Build service
cd ..\Service
msbuild SentinelHookService.vcxproj /p:Configuration=Debug /p:Platform=x64
```

## Installation

### Development (Test Signing)

1. Enable test signing (if not already enabled):
   ```cmd
   bcdedit /set testsigning on
   ```
   Reboot required.

2. Install driver using Device Manager or:
   ```cmd
   pnputil /add-driver sentinelhook.inf /install
   ```

3. Start the service:
   ```cmd
   sc start SentinelHookService
   ```

### Production (Code Signed)

1. Sign the driver with a valid code signing certificate
2. Install using standard Windows driver installation methods
3. Deploy service as Windows service

## Usage

### Starting the Service

```cmd
# Install service
sc create SentinelHookService binPath="C:\Path\To\SentinelHookService.exe" start=auto

# Start service
sc start SentinelHookService

# Check status
sc query SentinelHookService
```

### Viewing Telemetry

Telemetry data is available via:
- **Named Pipe**: `\\.\pipe\SentinelHookTelemetry`
- **ETW Events**: Use `logman` or Event Viewer
- **Service Logs**: Windows Event Log (Application)

### ETW Tracing

```cmd
# Start ETW trace
logman create trace SentinelHookTrace -p SentinelHookProvider -o trace.etl
logman start SentinelHookTrace

# Stop trace
logman stop SentinelHookTrace
```

## Project Structure

```
SentinelHook/
├── Driver/                 # Kernel-mode driver
│   ├── sentinelhook.c      # Main driver entry point
│   ├── filter.c            # Mini-filter implementation
│   ├── callbacks.c         # Process/image notify callbacks
│   ├── ioctl.c             # IOCTL handler
│   ├── telemetry.c         # Telemetry data collection
│   ├── sentinelhook.h      # Driver headers
│   └── sentinelhook.inf    # Driver installation file
├── Service/                # User-mode service
│   ├── main.cpp            # Service entry point
│   ├── ServiceCore.cpp     # Service implementation
│   ├── DriverComm.cpp      # IOCTL communication
│   ├── NamedPipe.cpp       # Named pipe server
│   ├── ETWProvider.cpp     # ETW integration
│   └── TelemetryAggregator.cpp  # Data aggregation
├── Common/                 # Shared definitions
│   ├── ioctl.h             # IOCTL definitions
│   ├── telemetry.h         # Telemetry data structures
│   └── events.h            # Event definitions
└── README.md              # This file
```

## Security Considerations

- **PatchGuard Compliance**: All kernel callbacks are PatchGuard-safe
- **DSE Compliance**: Driver must be properly signed for production
- **Privilege Requirements**: Service requires SYSTEM privileges
- **Data Privacy**: Telemetry data may contain sensitive information

## Development Notes

- This is a **security research project** for educational purposes
- Requires kernel-mode debugging capabilities (WinDbg)
- Test on isolated systems or VMs
- Follow responsible disclosure practices

## Tools Used

- **C/C++**: Core implementation languages
- **Windows Driver Kit (WDK)**: Driver development framework
- **WinDbg**: Kernel debugging
- **Sysinternals Suite**: System analysis and testing
- **ETW Tracing**: Event correlation
- **Visual Studio 2022**: Development environment

## License

This project is for security research and educational purposes only.

## Author

Freelance Security Researcher - December 2023


<!-- 2024-05-27 -->

<!-- 2024-08-07 -->

<!-- 2025-01-13 -->
