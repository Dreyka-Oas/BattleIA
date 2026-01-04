#include "Rendering/UIRenderer.h"
#include "raylib.h"
void UIRenderer::DrawHUD(int aliveCount, float avgElo) { DrawText(TextFormat("ALIVE: %i", aliveCount), 20, 20, 20, GREEN); }
