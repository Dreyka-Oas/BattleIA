#ifndef ITEM_H
#define ITEM_H

#include "raylib.h"
#include "../Config/ItemConfig.h"

struct Loot {
    Vector2 position;
    ItemType type;
    bool active;
};

#endif