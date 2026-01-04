#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include "../Entities/Item.h"
#include "../World/Map.h"
#include <vector>

class ItemManager {
public:
    void Init();
    void Update(Map& map, Vector2 zoneCenter, float zoneRadius);
    void Draw();
    int GetNearestItemIndex(Vector2 pos);
    Vector2 GetItemPosition(int index);
    ItemType PickUpItem(int index);
    void DropItem(Vector2 pos, ItemType type);
    float GetTimeNextSpawn() const { return spawnTimer; }

    // Getter manquant ajouté pour AISystem
    const std::vector<Loot>& GetItems() const { return items; }

private:
    std::vector<Loot> items;
    float spawnTimer;
    Vector2 FindValidSpawn(Map& map, Vector2 center, float radius);
};

#endif