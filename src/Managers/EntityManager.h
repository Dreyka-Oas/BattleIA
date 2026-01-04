#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "../World/Map.h"
#include "../Entities/Entity.h"
#include "../Entities/Projectile.h"
#include <vector>

class ItemManager;
class ProjectileManager;

enum class SortMode { RANK, NAME, HEALTH, ELO, KILLS };

class EntityManager {
public:
    void Init(Map& map);
    void RespawnAll(Map& map);
    void EvolveGeneration();
    void Update(Map& map, Vector2 zoneCenter, float zoneRadius, ItemManager& itemMgr, ProjectileManager& projMgr);
    
    // PERSISTANCE
    void SaveData();
    void LoadData();

    // MÉTHODES DE COMBAT ET ELO
    void ApplyDamage(int targetID, float amount, int attackerID, ItemManager& itemMgr);
    void HealEntity(int targetID, float amount);
    void ApplyEloPenalty(int entityID, float amount); // Était manquante
    void ProcessNearMiss(Vector2 projPos, float radius, std::vector<int>& ignoredIDs);

    // DESSIN (Même si vide, nécessaire car appelée dans Game.cpp)
    void Draw(Camera2D camera, int sw, int sh, int selID); // Était manquante

    // GETTERS ET UTILITAIRES
    int GetAliveCount() const;
    float GetAverageElo() const;
    std::vector<const Entity*> GetAllRanked(SortMode mode);
    const Entity* GetEntityByID(int id) const;
    Vector2 GetEntityPosition(int id);
    bool IsEntityActive(int id);
    int GetEntityIdAtPosition(Vector2 worldPos);
    int GetEntityIdAtPositionOptimized(Vector2 worldPos, float radiusCheck);
    int GetEntityIdAtIndex(int index, SortMode mode);
    bool IsEnemyArmed(int enemyID);
    int FindNearestEnemy(int myID, float range);
    int FindNearestEnemyExcluding(Vector2 pos, float range, const std::vector<int>& excludedIDs);
    void PullEntities(Vector2 center, float radius, float force, int excludeID);

    const std::vector<Entity>& GetEntities() const { return entities; }

private:
    std::vector<Entity> entities;
    static const int GRID_CELL_SIZE = 512;
    static const int GRID_COUNT = (int)((800 * 64) / GRID_CELL_SIZE) + 2;
    std::vector<int> spatialGrid[GRID_COUNT][GRID_COUNT];
    Vector2 FindValidPosition(Map& map);
};

#endif