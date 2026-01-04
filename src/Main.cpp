#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "external/raylib/lib/raylib.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")

#include "Core/Application.h"

int main() {
    Application app;
    app.Run();
    return 0;
}