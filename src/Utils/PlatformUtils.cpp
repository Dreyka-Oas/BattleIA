#include "PlatformUtils.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <dxgi1_4.h>
#pragma comment(lib, "dxgi.lib")
#endif

void DisableWindowResize(void* windowHandle) {
#ifdef _WIN32
    HWND hwnd = (HWND)windowHandle;
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
    SetWindowLong(hwnd, GWL_STYLE, style);
#endif
}

HardwareStats GetHardwareMetrics() {
    HardwareStats stats = { 0 };
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        stats.ramUsage = pmc.WorkingSetSize;
    }
    stats.cpuUsage = 1.0; 
#endif
    return stats;
}