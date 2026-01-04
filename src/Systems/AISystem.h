#ifndef AI_SYSTEM_H
#define AI_SYSTEM_H

#include "../Entities/Entity.h"
#include "../World/Map.h"
#include "../Managers/ItemManager.h"
#include "../Managers/ProjectileManager.h"
// On ne met pas EntityManager.h ici pour éviter l inclusion circulaire, on utilise une forward declaration
class EntityManager;

class AISystem {
public:
    // On passe EntityManager par référence pour utiliser sa grille spatiale optimisée
    static void Think(Entity& ent, EntityManager& em, ItemManager& im, ProjectileManager& pm, Map& map, Vector2 zoneCenter, float zoneRadius, float dt);
};

#endif