#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

struct HardwareStats {
    double cpuUsage;
    unsigned long long ramUsage;
    unsigned long long vramUsage;
    int gpuUsage;
};

void DisableWindowResize(void* windowHandle);
HardwareStats GetHardwareMetrics();

#endif