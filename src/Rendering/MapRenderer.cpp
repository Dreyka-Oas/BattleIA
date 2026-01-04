#include "Rendering/MapRenderer.h"
#include "Config/GameConfig.h"

void MapRenderer::Draw(const Map& map) {
    DrawTextureEx(map.GetTexture(), { 0,0 }, 0.0f, (MAP_SIZE * TILE_SIZE) / 2048.0f, WHITE);
}
