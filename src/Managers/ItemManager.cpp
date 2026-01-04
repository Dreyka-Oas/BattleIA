#include "ItemManager.h"
#include "raymath.h"
#include <cmath>

void ItemManager::Init() {
    items.clear();
    spawnTimer = LOOT_SPAWN_INTERVAL;
}

Vector2 ItemManager::FindValidSpawn(Map& map, Vector2 center, float radius) {
    for (int i = 0; i < 100; i++) {
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        float r = radius * sqrtf((float)GetRandomValue(0, 100) / 100.0f);
        float x = center.x + r * cosf(angle);
        float y = center.y + r * sinf(angle);
        if (map.IsWalkable(x, y)) return { x, y };
    }
    return { 0, 0 };
}

void ItemManager::DropItem(Vector2 pos, ItemType type) {
    if (type == ITEM_NONE) return;
    for (auto& item : items) {
        if (!item.active) {
            item.position = pos; item.type = type; item.active = true; return;
        }
    }
    items.push_back({ pos, type, true });
}

void ItemManager::Update(Map& map, Vector2 zoneCenter, float zoneRadius) {
    float dt = GetFrameTime();
    spawnTimer -= dt;

    for (auto& item : items) {
        if (item.active) {
            if (Vector2Distance(item.position, zoneCenter) > zoneRadius) item.active = false;
        }
    }

    if (spawnTimer <= 0) {
        int totalWeight = 0;
        for (int i = 1; i < ITEM_COUNT_ENUM; i++) totalWeight += ITEM_STATS[i].rarityWeight;

        for (int i = 0; i < LOOT_SPAWN_LIMIT; i++) {
            Vector2 pos = FindValidSpawn(map, zoneCenter, zoneRadius);
            if (pos.x == 0 && pos.y == 0) continue;

            int rnd = GetRandomValue(0, totalWeight);
            int currentWeight = 0;
            ItemType selectedType = ITEM_PISTOL;
            for (int j = 1; j < ITEM_COUNT_ENUM; j++) {
                currentWeight += ITEM_STATS[j].rarityWeight;
                if (rnd <= currentWeight) { selectedType = (ItemType)j; break; }
            }

            bool spawned = false;
            for (auto& item : items) {
                if (!item.active) {
                    item.position = pos; item.type = selectedType; item.active = true; spawned = true; break;
                }
            }
            if (!spawned) items.push_back({ pos, selectedType, true });
        }
        spawnTimer = LOOT_SPAWN_INTERVAL;
    }
}

void ItemManager::Draw() {
    for (const auto& item : items) {
        if (!item.active) continue;
        DrawCircleV(item.position, 12.0f, ITEM_STATS[item.type].color);
        DrawCircleLines((int)item.position.x, (int)item.position.y, 13.0f, WHITE);
    }
}

int ItemManager::GetNearestItemIndex(Vector2 pos) {
    int best = -1; float minDist = 800.0f;
    for (int i = 0; i < (int)items.size(); i++) {
        if (!items[i].active) continue;
        float d = Vector2Distance(pos, items[i].position);
        if (d < minDist) { minDist = d; best = i; }
    }
    return best;
}

Vector2 ItemManager::GetItemPosition(int index) {
    if (index >= 0 && index < (int)items.size()) return items[index].position;
    return { 0, 0 };
}

ItemType ItemManager::PickUpItem(int index) {
    if (index >= 0 && index < (int)items.size() && items[index].active) {
        items[index].active = false;
        return items[index].type;
    }
    return ITEM_NONE;
}