//
// SentinelHook - Event Definitions
// ETW event definitions and GUIDs
//

#pragma once

#include <windows.h>
#include <evntrace.h>

// ETW Provider GUID
// {A1B2C3D4-E5F6-4789-A0B1-C2D3E4F5A6B7}
DEFINE_GUID(SentinelHookProviderGuid,
    0xa1b2c3d4, 0xe5f6, 0x4789, 0xa0, 0xb1, 0xc2, 0xd3, 0xe4, 0xf5, 0xa6, 0xb7);

// Event descriptors
#define EVENT_FILE_OPERATION        0x01
#define EVENT_PROCESS_CREATE        0x02
#define EVENT_PROCESS_TERMINATE     0x03
#define EVENT_IMAGE_LOAD            0x04
#define EVENT_INJECTION_DETECTED    0x05
#define EVENT_UNSIGNED_DRIVER       0x06

// Event levels
#define EVENT_LEVEL_CRITICAL        1
#define EVENT_LEVEL_ERROR           2
#define EVENT_LEVEL_WARNING         3
#define EVENT_LEVEL_INFO            4
#define EVENT_LEVEL_VERBOSE         5

