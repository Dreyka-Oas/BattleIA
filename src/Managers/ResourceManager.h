#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "raylib.h"

class ResourceManager {
public:
    static Font GetSystemFont();
    static void UnloadResources();
private:
    static Font systemFont;
    static bool fontLoaded;
};

#endif