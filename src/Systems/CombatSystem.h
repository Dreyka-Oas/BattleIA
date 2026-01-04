#ifndef COMBAT_SYSTEM_H
#define COMBAT_SYSTEM_H
#include "raylib.h"
class CombatSystem { public: static void HandleExplosion(Vector2 pos, float radius, int ownerID, class EntityManager& em, class ItemManager& im); };
#endif
