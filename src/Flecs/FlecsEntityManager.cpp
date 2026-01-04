// ============================================================================
// FLECS ENTITY MANAGER - Implémentation
// ============================================================================

#include "FlecsEntityManager.h"
#include "../Managers/ItemManager.h"
#include "../Managers/ProjectileManager.h"
#include "../Config/GameConfig.h"
#include "../Config/EloConfig.h"
#include "../Systems/MovementSystem.h"
#include "raymath.h"
#include <algorithm>
#include <fstream>

// ============================================================================
// HELPERS
// ============================================================================

static float GetRandomFloat(float min, float max) {
    return min + ((float)GetRandomValue(0, 10000) / 10000.0f) * (max - min);
}

// ============================================================================
// CONSTRUCTEUR & DESTRUCTEUR
// ============================================================================

FlecsEntityManager::FlecsEntityManager() {
    // Enregistrer les composants
    ecs.component<Position>();
    ecs.component<Target>();
    ecs.component<Velocity>();
    ecs.component<Health>();
    ecs.component<Poisoned>();
    ecs.component<Active>();
    ecs.component<Dead>();
    ecs.component<Name>();
    ecs.component<Stats>();
    ecs.component<Brain>();
    ecs.component<AITimers>();
    ecs.component<Weapon>();
    ecs.component<Armed>();
    ecs.component<GridCell>();
    
    // Relations
    ecs.component<Attacking>();
    ecs.component<TeamedWith>();
    ecs.component<FleeingFrom>();
    
    // Créer les queries pré-compilées
    aliveQuery = ecs.query_builder<Position, Health>()
        .with<Active>()
        .build();
    
    armedQuery = ecs.query_builder<Position, Weapon>()
        .with<Active>()
        .build();
    
    statsQuery = ecs.query<Stats>();
}

FlecsEntityManager::~FlecsEntityManager() {
    // Flecs gère automatiquement le cleanup
}

// ============================================================================
// INITIALISATION
// ============================================================================

void FlecsEntityManager::Init(Map& map, int entityCount) {
    entityList.clear();
    entityList.reserve(entityCount);
    
    for (int i = 0; i < entityCount; i++) {
        Vector2 spawnPos = FindValidPosition(map);
        std::string entityName = "Bot " + std::to_string(i + 1);
        float initialElo = (float)AI_BASE_ELO;
        
        flecs::entity e = CreateEntity(spawnPos, entityName, initialElo);
        entityList.push_back(e);
    }
    
    // Charger les données sauvegardées si elles existent
    LoadData();
    
    // Enregistrer tous les systèmes
    RegisterSystems();
    SetupObservers();
}

// ============================================================================
// CRÉATION D'ENTITÉ
// ============================================================================

flecs::entity FlecsEntityManager::CreateEntity(Vector2 position, std::string name, float elo) {
    auto entity = ecs.entity()
        .set<Position>({position, position})
        .set<Target>({position})
        .set<Velocity>({AI_SPEED * GetRandomFloat(0.8f, 1.2f), {0, 0}})
        .set<Health>({100.0f, 100.0f, 0.0f})
        .set<Name>({name})
        .set<Stats>({elo, elo, 0})
        .set<Brain>({
            GetRandomFloat(0.2f, 1.0f),   // aggression
            GetRandomFloat(0.0f, 1.0f),   // teamSpirit
            GetRandomFloat(0.2f, 0.8f),   // greed
            GetRandomFloat(0.5f, 1.0f)    // precision
        })
        .set<AITimers>({GetRandomFloat(0.0f, 1.0f), 0.0f, 0.0f})
        .set<Weapon>({ITEM_NONE, 0.0f})
        .add<Active>();
    
    return entity;
}

// ============================================================================
// ENREGISTREMENT DES SYSTÈMES
// ============================================================================

void FlecsEntityManager::RegisterSystems() {
    // SYSTÈME 1: POISON (update poison timers et dommages)
    poisonSystem = ecs.system<Poisoned, Health>("PoisonSystem")
        .with<Active>()
        .each([&](flecs::entity e, Poisoned& poison, Health& health) {
            poison.timer -= ecs.delta_time();
            if (poison.timer <= 0) {
                e.remove<Poisoned>();
            } else {
                health.current -= 5.0f * ecs.delta_time();
                if (health.current <= 0) {
                    e.add<Dead>();
                    e.remove<Active>();
                }
            }
        });
    
    // SYSTÈME 2: ZONE DAMAGE
    // On le fera dans Update() car il a besoin de zoneCenter/zoneRadius
    
    // SYSTÈME 3: MOUVEMENT (applique velocity à position)
    movementSystem = ecs.system<Position, const Velocity, const Target>("MovementSystem")
        .with<Active>()
        .each([](Position& pos, const Velocity& vel, const Target& target) {
            Vector2 dir = Vector2Normalize(Vector2Subtract(target.destination, pos.pos));
            if (Vector2LengthSqr(dir) < 0.01f) {
                dir = {0, 0};
            }
            
            float dt = GetFrameTime();
            pos.lastPos = pos.pos;
            pos.pos = Vector2Add(pos.pos, Vector2Scale(dir, vel.speed * dt * 60.0f));
        });
    
    // SYSTÈME 4: IA (décision et target)
    // Implémenté dans Update() car trop complexe pour lambda simple
    
    // SYSTÈME 5: COMBAT (cooldowns armes)
    combatSystem = ecs.system<Weapon>("CombatSystem")
        .with<Active>()
        .each([](Weapon& weapon) {
            if (weapon.cooldown > 0) {
                weapon.cooldown -= GetFrameTime();
            }
        });
}

// ============================================================================
// OBSERVERS (événements automatiques)
// ============================================================================

void FlecsEntityManager::SetupObservers() {
    // Observer: Quand une entité meurt
    onEntityDeath = ecs.observer<Position, Weapon>()
        .event(flecs::OnAdd)
        .with<Dead>()
        .each([](flecs::entity e, Position& pos, Weapon& weapon) {
            // Logique de mort sera gérée dans ApplyDamage
        });
}

// ============================================================================
// UPDATE PRINCIPAL
// ============================================================================

void FlecsEntityManager::Update(Map& map, Vector2 zoneCenter, float zoneRadius, 
                                  ItemManager& itemMgr, ProjectileManager& projMgr, float deltaTime) {
    
    ecs.set_delta_time(deltaTime);
    
    // Mettre à jour la grille spatiale
    UpdateSpatialGrid();
    
    // 1. Systèmes automatiques (poison, movement, combat cooldowns)
    ecs.progress(deltaTime);
    
    // 2. ZONE DAMAGE (manuel car contexte nécessaire)
    float worldLimit = MAP_SIZE * TILE_SIZE;
    float killZoneRadius = zoneRadius + 2000.0f;
    
    aliveQuery.each([&](flecs::entity e, Position& pos, Health& health) {
        // Clamp position
        pos.pos.x = Clamp(pos.pos.x, 0.0f, worldLimit);
        pos.pos.y = Clamp(pos.pos.y, 0.0f, worldLimit);
        
        // Zone damage
        float distToZone = Vector2Distance(pos.pos, zoneCenter);
        if (distToZone > zoneRadius) {
            if (distToZone > killZoneRadius) {
                health.current = 0;
            } else {
                float severity = 1.0f + (distToZone - zoneRadius) / 200.0f;
                health.current -= (ZONE_DAMAGE_PER_SEC * severity * severity) * deltaTime;
            }
            
            if (health.current <= 0) {
                KillEntity(e, itemMgr);
            }
        }
    });
    
    // 3. IA SYSTEM (logique complexe, fait manuellement)
    ecs.each([&](flecs::entity e, Position& pos, Target& target, Health& health, 
                 AITimers& timers, Brain& brain, Weapon& weapon, Stats& stats) {
        
        if (!e.has<Active>()) return;
        
        // Update timers
        timers.scanTimer -= deltaTime;
        if (timers.lootTimer > 0) timers.lootTimer -= deltaTime;
        
        float distToZone = Vector2Distance(pos.pos, zoneCenter);
        bool isAtTarget = Vector2DistanceSqr(pos.pos, target.destination) < 100.0f;
        
        // Décision AI toutes les ~0.8s
        if (timers.scanTimer <= 0 || isAtTarget) {
            timers.scanTimer = 0.8f + GetRandomFloat(0.0f, 0.4f);
            
            // PRIORITÉ 1: Zone
            if (distToZone > zoneRadius * 0.9f) {
                target.destination = zoneCenter;
            }
            // PRIORITÉ 2: Soin
            else if (health.current < 40.0f) {
                int nearestLoot = itemMgr.GetNearestItemIndex(pos.pos);
                if (nearestLoot != -1) {
                    Vector2 lootPos = itemMgr.GetItemPosition(nearestLoot);
                    if (Vector2Distance(pos.pos, lootPos) < 2000.0f) {
                        target.destination = lootPos;
                    }
                }
            }
            // PRIORITÉ 3: Combat
            else if (weapon.type >= ITEM_PISTOL) {
                flecs::entity enemy = FindNearestEnemy(e, 15000.0f);
                if (enemy.is_alive()) {
                    const Position* enemyPos = enemy.get<Position>();
                    if (enemyPos) {
                        float range = ITEM_STATS[weapon.type].range;
                        if (Vector2Distance(pos.pos, enemyPos->pos) > range * 0.8f) {
                            target.destination = enemyPos->pos;
                        } else {
                            // Tourne autour
                            float angle = GetRandomFloat(0, 360) * DEG2RAD;
                            target.destination = Vector2Add(pos.pos, {cosf(angle) * 500.0f, sinf(angle) * 500.0f});
                        }
                    }
                }
            }
            // PRIORITÉ 4: Loot
            else if (weapon.type == ITEM_NONE) {
                int nearestLoot = itemMgr.GetNearestItemIndex(pos.pos);
                if (nearestLoot != -1) {
                    target.destination = itemMgr.GetItemPosition(nearestLoot);
                }
            }
            // PRIORITÉ 5: Exploration
            else {
                float angle = GetRandomFloat(0, 360) * DEG2RAD;
                float dist = GetRandomFloat(600, 2000);
                target.destination = Vector2Add(pos.pos, {cosf(angle) * dist, sinf(angle) * dist});
            }
        }
        
        // TIR
        if (weapon.type >= ITEM_PISTOL && weapon.cooldown <= 0) {
            const ItemProperties& itemStats = ITEM_STATS[weapon.type];
            flecs::entity enemy = FindNearestEnemy(e, itemStats.range);
            if (enemy.is_alive()) {
                const Position* enemyPos = enemy.get<Position>();
                if (enemyPos) {
                    int projCount = (itemStats.projCount > 0) ? itemStats.projCount : 1;
                    for (int i = 0; i < projCount; i++) {
                        // TODO: ApplySpread
                        Vector2 aimPos = enemyPos->pos;
                        int entityId = (int)e.id(); // Cast temporaire pour ProjectileManager
                        projMgr.Spawn(pos.pos, aimPos, entityId, weapon.type, 
                                     itemStats.projType, itemStats.projSpeed);
                    }
                    weapon.cooldown = itemStats.cooldown;
                }
            }
        }
        
        // RAMASSAGE (simplifié, TODO: LootSystem)
        int nearbyItem = itemMgr.GetNearestItemIndex(pos.pos);
        if (nearbyItem != -1 && timers.lootTimer <= 0) {
            Vector2 itemPos = itemMgr.GetItemPosition(nearbyItem);
            if (Vector2Distance(pos.pos, itemPos) < 100.0f) {
                ItemType pickedItem = itemMgr.GetItemType(nearbyItem);
                if (pickedItem == ITEM_MEDKIT) {
                    health.current = fminf(health.maximum, health.current + 50.0f);
                    timers.lootTimer = 0.5f;
                } else if (pickedItem >= ITEM_PISTOL) {
                    weapon.type = pickedItem;
                    weapon.cooldown = 0.0f;
                    e.add<Armed>();
                    timers.lootTimer = 0.5f;
                }
                itemMgr.RemoveItem(nearbyItem);
            }
        }
    });
}

// ============================================================================
// COMBAT & DOMMAGES
// ============================================================================

void FlecsEntityManager::ApplyDamage(flecs::entity target, float amount, flecs::entity attacker, ItemManager& itemMgr) {
    if (!target.is_alive() || !target.has<Active>()) return;
    
    Health* health = target.get_mut<Health>();
    Stats* victimStats = target.get_mut<Stats>();
    if (!health || !victimStats) return;
    
    health->current -= amount;
    
    if (health->current <= 0) {
        // MORT
        KillEntity(target, itemMgr);
        victimStats->elo *= ELO_DEATH_FACTOR_UNARMED;
        
        if (attacker.is_alive()) {
            Stats* attackerStats = attacker.get_mut<Stats>();
            if (attackerStats) {
                attackerStats->kills++;
                attackerStats->elo += ELO_GAIN_KILL;
            }
        }
    } else {
        // BLESSÉ
        victimStats->elo = fmaxf(0.0f, victimStats->elo - ELO_LOSS_HIT);
        if (attacker.is_alive()) {
            Stats* attackerStats = attacker.get_mut<Stats>();
            if (attackerStats) {
                attackerStats->elo += ELO_GAIN_HIT;
            }
        }
    }
}

void FlecsEntityManager::KillEntity(flecs::entity entity, ItemManager& itemMgr) {
    entity.remove<Active>();
    entity.add<Dead>();
    
    // Drop weapon
    const Weapon* weapon = entity.get<Weapon>();
    const Position* pos = entity.get<Position>();
    if (weapon && pos && weapon->type != ITEM_NONE) {
        itemMgr.DropItem(pos->pos, weapon->type);
    }
}

// ============================================================================
// RECHERCHE D'ENNEMIS
// ============================================================================

flecs::entity FlecsEntityManager::FindNearestEnemy(flecs::entity myEntity, float maxRange) {
    const Position* myPos = myEntity.get<Position>();
    if (!myPos) return flecs::entity::null();
    
    flecs::entity nearest = flecs::entity::null();
    float minDistSq = maxRange * maxRange;
    
    aliveQuery.each([&](flecs::entity e, Position& pos, Health&) {
        if (e == myEntity) return;
        
        float distSq = Vector2DistanceSqr(myPos->pos, pos.pos);
        if (distSq < minDistSq) {
            minDistSq = distSq;
            nearest = e;
        }
    });
    
    return nearest;
}

// ============================================================================
// STATISTIQUES
// ============================================================================

int FlecsEntityManager::GetAliveCount() const {
    int count = 0;
    ecs.each([&](flecs::entity e) {
        if (e.has<Active>()) count++;
    });
    return count;
}

float FlecsEntityManager::GetAverageElo() const {
    float total = 0;
    int count = 0;
    statsQuery.each([&](const Stats& stats) {
        total += stats.elo;
        count++;
    });
    return count > 0 ? total / count : 0.0f;
}

// ============================================================================
// SPATIAL GRID (OPTIMISATION)
// ============================================================================

void FlecsEntityManager::UpdateSpatialGrid() {
    // Clear grid
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            spatialGrid[x][y].clear();
        }
    }
    
    // Repopulate
    aliveQuery.each([&](flecs::entity e, Position& pos, Health&) {
        int gx = (int)(pos.pos.x / GRID_CELL_SIZE);
        int gy = (int)(pos.pos.y / GRID_CELL_SIZE);
        if (gx >= 0 && gx < GRID_SIZE && gy >= 0 && gy < GRID_SIZE) {
            spatialGrid[gx][gy].push_back(e);
        }
    });
}

// ============================================================================
// UTILITAIRES
// ============================================================================

Vector2 FlecsEntityManager::FindValidPosition(Map& map) {
    float limit = MAP_SIZE * TILE_SIZE;
    for (int i = 0; i < 50; i++) {
        float x = GetRandomFloat(0, limit);
        float y = GetRandomFloat(0, limit);
        if (map.IsWalkable(x, y)) {
            return {x, y};
        }
    }
    return {limit / 2, limit / 2};
}

// ============================================================================
// ÉDITEUR WEB
// ============================================================================

void FlecsEntityManager::EnableWebEditor(int port) {
    #ifdef FLECS_REST
    ecs.import<flecs::rest>();
    ecs.set<flecs::rest::Rest>({port});
    TraceLog(LOG_INFO, "Flecs Web Editor: http://localhost:%d", port);
    #else
    TraceLog(LOG_WARNING, "Flecs REST non activé. Recompiler avec #define FLECS_REST");
    #endif
}

// ============================================================================
// SAVE/LOAD (TODO: implémenter sérialisation Flecs)
// ============================================================================

void FlecsEntityManager::SaveData(const char* filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return;
    
    // TODO: Sérialiser stats & brain de chaque entité
    TraceLog(LOG_INFO, "SaveData() non implémenté (TODO)");
    file.close();
}

void FlecsEntityManager::LoadData(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return;
    
    // TODO: Charger stats & brain
    TraceLog(LOG_INFO, "LoadData() non implémenté (TODO)");
    file.close();
}

// ============================================================================
// CONVERSION TEMPORAIRE (pour transition)
// ============================================================================

void FlecsEntityManager::SyncToOldEntities(std::vector<Entity>& oldEntities) {
    // Copie Flecs → ancien format (pour rendu temporaire)
    // TODO
}

void FlecsEntityManager::SyncFromOldEntities(const std::vector<Entity>& oldEntities) {
    // Copie ancien → Flecs (pour migration progressive)
    // TODO
}

void FlecsEntityManager::RespawnAll(Map& map) {
    ecs.each([&](flecs::entity e, Position& pos, Health& health, Weapon& weapon, AITimers& timers, Stats& stats) {
        pos.pos = FindValidPosition(map);
        pos.lastPos = pos.pos;
        health.current = health.maximum;
        health.armor = 0.0f;
        weapon.type = ITEM_NONE;
        weapon.cooldown = 0.0f;
        timers.scanTimer = GetRandomFloat(0.0f, 0.5f);
        timers.lootTimer = 0.0f;
        stats.eloStart = stats.elo;
        stats.kills = 0;
        e.add<Active>();
        e.remove<Dead>();
        e.remove<Poisoned>();
        e.remove<Armed>();
    });
}

flecs::entity FlecsEntityManager::GetEntityByIndex(int index) {
    if (index >= 0 && index < (int)entityList.size()) {
        return entityList[index];
    }
    return flecs::entity::null();
}
