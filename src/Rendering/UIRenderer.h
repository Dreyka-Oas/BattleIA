#ifndef UI_RENDERER_H
#define UI_RENDERER_H
#include "raylib.h"
class UIRenderer {
public:
    static void DrawHUD(int aliveCount, float avgElo);
};
#endif
