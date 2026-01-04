#ifndef MAP_H
#define MAP_H

#include "../Config/GameConfig.h"
#include "Tile.h" // Inclusion cruciale
#include <vector>

class Map {
public:
    void Generate();
    void Draw(Camera2D camera, int sw, int sh);
    bool IsWalkable(float x, float y);
    TileType GetTileTypeAt(float x, float y);
    void Unload();
    
    Texture2D GetTexture() const { return worldTexture; }

private:
    std::vector<Tile> tiles;
    Texture2D worldTexture;
};

#endif