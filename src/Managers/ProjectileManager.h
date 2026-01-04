#ifndef PROJECTILE_MANAGER_H
#define PROJECTILE_MANAGER_H

#include "../Entities/Projectile.h"
#include <vector>

class EntityManager;
class ItemManager;

class ProjectileManager {
public:
    // Signature mise à jour avec ItemType
    void Spawn(Vector2 pos, Vector2 target, int ownerID, ItemType weaponID, ProjType type, float speed);
    
    void Update(EntityManager& entityMgr, ItemManager& itemMgr);
    void Draw();
    void Clear();
    
    const std::vector<Projectile>& GetProjectiles() const { return projectiles; }
    const std::vector<Explosion>& GetExplosions() const { return explosions; }
    const std::vector<LightningBolt>& GetLightnings() const { return lightnings; }

private:
    std::vector<Projectile> projectiles;
    std::vector<Explosion> explosions;
    std::vector<LightningBolt> lightnings;
};

#endif