#ifndef TILE_H
#define TILE_H

enum TileType { 
    TILE_DEEP_WATER, 
    TILE_WATER, 
    TILE_SAND, 
    TILE_GRASS, 
    TILE_FOREST, 
    TILE_MOUNTAIN, 
    TILE_PATH 
};

struct Tile {
    TileType type;
    unsigned char variant;
};

#endif