// ============================================================================
// FLECS ENTITY MANAGER - Gestionnaire d'entités ECS haute performance
// ============================================================================

#ifndef FLECS_ENTITY_MANAGER_H
#define FLECS_ENTITY_MANAGER_H

#include <flecs.h>
#include "FlecsComponents.h"
#include "../World/Map.h"
#include "../Entities/Entity.h"  // Pour conversion temporaire
#include <vector>

class ItemManager;
class ProjectileManager;

class FlecsEntityManager {
public:
    // ========================================================================
    // CONSTRUCTEUR & INITIALISATION
    // ========================================================================
    
    FlecsEntityManager();
    ~FlecsEntityManager();
    
    void Init(Map& map, int entityCount);
    void RespawnAll(Map& map);
    
    // ========================================================================
    // MISE À JOUR (SYSTEMS PIPELINE)
    // ========================================================================
    
    void Update(Map& map, Vector2 zoneCenter, float zoneRadius, 
                ItemManager& itemMgr, ProjectileManager& projMgr, float deltaTime);
    
    // ========================================================================
    // GESTION DES ENTITÉS
    // ========================================================================
    
    flecs::entity CreateEntity(Vector2 position, std::string name, float elo);
    void KillEntity(flecs::entity entity, ItemManager& itemMgr);
    void RespawnEntity(flecs::entity entity, Map& map);
    
    // ========================================================================
    // SYSTÈMES PRINCIPAUX (appelés automatiquement dans Update)
    // ========================================================================
    
    void RegisterSystems();
    void SetupObservers();  // Pour événements (mort, pickup, etc.)
    
    // ========================================================================
    // COMBAT & DOMMAGES
    // ========================================================================
    
    void ApplyDamage(flecs::entity target, float amount, flecs::entity attacker, ItemManager& itemMgr);
    void ApplyPoison(flecs::entity target, float duration, flecs::entity source);
    void HealEntity(flecs::entity target, float amount);
    void ApplyEloPenalty(flecs::entity entity, float amount);
    
    // ========================================================================
    // QUERIES & RECHERCHE
    // ========================================================================
    
    flecs::entity FindNearestEnemy(flecs::entity myEntity, float maxRange);
    flecs::entity FindNearestEnemyExcluding(Vector2 pos, float maxRange, 
                                            const std::vector<flecs::entity>& excluded);
    flecs::entity FindNearestArmedEnemy(Vector2 pos, float maxRange);
    
    // Recherche spatiale optimisée (avec spatial grid)
    std::vector<flecs::entity> GetEntitiesInRadius(Vector2 center, float radius);
    flecs::entity GetEntityAtPosition(Vector2 pos, float radius = 0.0f);
    
    // ========================================================================
    // STATISTIQUES & QUERIES
    // ========================================================================
    
    int GetAliveCount() const;
    float GetAverageElo() const;
    std::vector<flecs::entity> GetAllRankedByElo();
    std::vector<flecs::entity> GetAllRankedByKills();
    
    // ========================================================================
    // PERSISTANCE (SAVE/LOAD)
    // ========================================================================
    
    void SaveData(const char* filename = "save.dat");
    void LoadData(const char* filename = "save.dat");
    
    // ========================================================================
    // CONVERSION TEMPORAIRE (pour transition depuis ancien système)
    // ========================================================================
    
    void SyncToOldEntities(std::vector<Entity>& oldEntities);  // Copie Flecs → Old
    void SyncFromOldEntities(const std::vector<Entity>& oldEntities);  // Copie Old → Flecs
    
    // ========================================================================
    // ACCESSEURS
    // ========================================================================
    
    flecs::world& GetWorld() { return ecs; }
    const flecs::world& GetWorld() const { return ecs; }
    
    // Récupérer entité par ID (pour compatibilité)
    flecs::entity GetEntityByIndex(int index);
    
    // ========================================================================
    // ÉDITEUR WEB FLECS (OPTIONNEL)
    // ========================================================================
    
    void EnableWebEditor(int port = 27750);  // Active dashboard http://localhost:27750
    
private:
    // ========================================================================
    // DONNÉES MEMBRES
    // ========================================================================
    
    flecs::world ecs;  // Le "registry" Flecs
    
    // Systèmes (stockés pour contrôle manuel si besoin)
    flecs::system<> movementSystem;
    flecs::system<> aiSystem;
    flecs::system<> combatSystem;
    flecs::system<> poisonSystem;
    flecs::system<> zoneSystem;
    
    // Observers (réactions automatiques aux événements)
    flecs::observer onEntityDeath;
    flecs::observer onEntityHit;
    flecs::observer onItemPickup;
    
    // Queries pré-compilées (ultra-rapides!)
    flecs::query<Position, Health> aliveQuery;
    flecs::query<Position, Weapon> armedQuery;
    flecs::query<Stats> statsQuery;
    
    // Grille spatiale pour optimisation
    static constexpr int GRID_CELL_SIZE = 512;
    static constexpr int GRID_SIZE = 128;  // 64*800 / 512
    std::vector<flecs::entity> spatialGrid[GRID_SIZE][GRID_SIZE];
    
    // Compteur pour IDs séquentiels
    int nextEntityId = 0;
    
    // Stockage des entités pour accès rapide par index
    std::vector<flecs::entity> entityList;
    
    // ========================================================================
    // MÉTHODES PRIVÉES
    // ========================================================================
    
    void UpdateSpatialGrid();
    Vector2 FindValidPosition(Map& map);
    void EvolveBestPerformers();
};

#endif
