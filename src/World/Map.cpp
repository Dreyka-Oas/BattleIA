#include "Map.h"
#include "raymath.h"
#include <cmath>

float GetSmoothNoise(float x, float y) {
    return (sinf(x * 0.03f) + cosf(y * 0.03f) + sinf(x * 0.01f + y * 0.01f)) * 0.5f;
}

void Map::Generate() {
    int texSize = 2048; float lS = (float)MAP_SIZE * 2.0f;
    Image img = GenImageColor(texSize, texSize, CLR_DEEP_BLUE);
    Color* px = (Color*)img.data;
    
    // Initialisation
    tiles.resize(MAP_SIZE * MAP_SIZE);
    for(auto& t : tiles) { t.type = TILE_DEEP_WATER; t.variant = 255; }

    Vector2 l1 = { lS * 0.35f, lS * 0.35f }; Vector2 l2 = { lS * 0.65f, lS * 0.65f }; float lR = lS * 0.06f;

    for (int y = 0; y < texSize; y++) {
        for (int x = 0; x < texSize; x++) {
            float fx = (x / (float)texSize) * lS; float fy = (y / (float)texSize) * lS;
            float d = Vector2Distance({ fx, fy }, { lS / 2, lS / 2 });
            float fd = d + (GetSmoothNoise(fx, fy) * (lS * 0.04f)) + (powf(d / (lS / 2), 8.0f) * 1000.0f);

            Color c = CLR_DEEP_BLUE; TileType tT = TILE_DEEP_WATER; bool n = false;
            if (fd < 900.0f) {
                n = true;
                float dL1 = Vector2Distance({ fx, fy }, l1) + GetSmoothNoise(fx * 0.1f, fy * 0.1f) * 60.0f;
                float dL2 = Vector2Distance({ fx, fy }, l2) + GetSmoothNoise(fx * 0.1f, fy * 0.1f) * 60.0f;
                if (dL1 < lR || dL2 < lR) { c = CLR_WATER; tT = TILE_WATER; }
                else if (fd > 840.0f) { c = CLR_WATER; tT = TILE_WATER; }
                else if (fd > 800.0f) { c = CLR_SAND; tT = TILE_SAND; }
                else {
                    float bi = GetSmoothNoise(fx * 0.5f, fy * 0.5f);
                    if (bi > 0.6f) { c = CLR_MOUNTAIN; tT = TILE_MOUNTAIN; }
                    else if (bi > 0.1f) { c = CLR_FOREST; tT = TILE_FOREST; }
                    else { c = CLR_GRASS; tT = TILE_GRASS; }

                    if (abs((fx - lS / 2) + sinf(fy * 0.02f) * (lS * 0.06f)) < lS * 0.006f) { c = CLR_PATH; tT = TILE_PATH; }
                }
            }
            int tx = (int)((fx / lS) * MAP_SIZE); int ty = (int)((fy / lS) * MAP_SIZE);
            if (tx >= 0 && tx < MAP_SIZE && ty >= 0 && ty < MAP_SIZE) tiles[ty * MAP_SIZE + tx].type = tT;
            if (n) { unsigned char v = (unsigned char)(235 + (GetSmoothNoise(fx * 2, fy * 2) * 15.0f)); c.r = (c.r * v) / 255; c.g = (c.g * v) / 255; c.b = (c.b * v) / 255; }
            px[y * texSize + x] = c;
        }
    }
    worldTexture = LoadTextureFromImage(img);
    GenTextureMipmaps(&worldTexture); SetTextureFilter(worldTexture, TEXTURE_FILTER_TRILINEAR); SetTextureWrap(worldTexture, TEXTURE_WRAP_CLAMP); UnloadImage(img);
}

void Map::Draw(Camera2D camera, int sw, int sh) {
    ClearBackground(CLR_DEEP_BLUE);
    DrawTextureEx(worldTexture, { 0,0 }, 0.0f, (MAP_SIZE * TILE_SIZE) / 2048.0f, WHITE);
}

bool Map::IsWalkable(float x, float y) {
    if (x < 0 || x >= MAP_SIZE * TILE_SIZE || y < 0 || y >= MAP_SIZE * TILE_SIZE) return false;
    int tx = (int)(x / TILE_SIZE);
    int ty = (int)(y / TILE_SIZE);
    if (tx < 0 || tx >= MAP_SIZE || ty < 0 || ty >= MAP_SIZE) return false;
    TileType t = tiles[ty * MAP_SIZE + tx].type;
    return (t != TILE_DEEP_WATER);
}

TileType Map::GetTileTypeAt(float x, float y) {
    if (x < 0 || x >= MAP_SIZE * TILE_SIZE || y < 0 || y >= MAP_SIZE * TILE_SIZE) return TILE_DEEP_WATER;
    int tx = (int)(x / TILE_SIZE);
    int ty = (int)(y / TILE_SIZE);
    if (tx < 0 || tx >= MAP_SIZE || ty < 0 || ty >= MAP_SIZE) return TILE_DEEP_WATER;
    return tiles[ty * MAP_SIZE + tx].type;
}

void Map::Unload() { UnloadTexture(worldTexture); }