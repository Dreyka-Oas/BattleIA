#ifndef LOOT_SYSTEM_H
#define LOOT_SYSTEM_H
#include "Entities/Entity.h"
#include "World/Map.h"
#include <vector>

class LootSystem {
public:
    static void UpdateSpawning(float dt, class ItemManager& im, Map& map, Vector2 zoneCenter, float zoneRadius);
    static bool CheckPickup(Entity& ent, class ItemManager& im);
};
#endif
