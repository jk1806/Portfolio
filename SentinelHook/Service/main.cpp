// service main entry point
#include <windows.h>
#include <iostream>
#include "ServiceCore.h"

int wmain(int argc, wchar_t* argv[])
{
    SentinelHookService service;

    if (!service.Initialize()) {
        std::wcerr << L"Failed to initialize service" << std::endl;
        return 1;
    }

    if (argc > 1) {
        if (_wcsicmp(argv[1], L"install") == 0) {
            if (service.Install()) {
                std::wcout << L"Service installed successfully" << std::endl;
                return 0;
            } else {
                std::wcerr << L"Failed to install service" << std::endl;
                return 1;
            }
        } else if (_wcsicmp(argv[1], L"uninstall") == 0) {
            if (service.Uninstall()) {
                std::wcout << L"Service uninstalled successfully" << std::endl;
                return 0;
            } else {
                std::wcerr << L"Failed to uninstall service" << std::endl;
                return 1;
            }
        } else if (_wcsicmp(argv[1], L"run") == 0) {
            // Run as console application for testing
            std::wcout << L"Running service in console mode..." << std::endl;
            service.Run();
            return 0;
        }
    }

    // Run as Windows service
    service.Run();
    return 0;
}

