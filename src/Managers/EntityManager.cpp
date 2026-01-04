#include "EntityManager.h"
#include "ItemManager.h"
#include "ProjectileManager.h"
#include "../Config/EloConfig.h"
#include "../Config/GameConfig.h"
#include "../World/Tile.h"
#include "../Systems/AISystem.h"
#include "raymath.h"
#include <algorithm>
#include <fstream>
#include <cmath>

float GetRandomFloat(float min, float max) {
    return min + ((float)GetRandomValue(0, 10000) / 10000.0f) * (max - min);
}

void EntityManager::Init(Map& map) {
    entities.clear();
    entities.resize(AI_COUNT);
    for (int i = 0; i < AI_COUNT; i++) {
        entities[i].id = i;
        entities[i].name = "Bot " + std::to_string(i + 1);
        entities[i].elo = (float)AI_BASE_ELO;
        entities[i].eloStartRound = (float)AI_BASE_ELO;
        entities[i].brain.aggression = GetRandomFloat(0.2f, 1.0f);
        entities[i].brain.teamSpirit = GetRandomFloat(0.0f, 1.0f);
        entities[i].brain.greed = GetRandomFloat(0.2f, 0.8f);
        entities[i].brain.precision = GetRandomFloat(0.5f, 1.0f);
        entities[i].position = FindValidPosition(map);
        entities[i].lastPosition = entities[i].position;
        entities[i].target = entities[i].position;
        entities[i].health = 100.0f;
        entities[i].maxHealth = 100.0f;
        entities[i].speed = AI_SPEED * GetRandomFloat(0.8f, 1.2f);
        entities[i].active = true;
        entities[i].scanTimer = GetRandomFloat(0.0f, 1.0f); 
    }
    LoadData();
}

void EntityManager::SaveData() {
    std::ofstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        for (const auto& e : entities) {
            file.write((char*)&e.elo, sizeof(float));
            file.write((char*)&e.brain, sizeof(Brain));
        }
        file.close();
    }
}

void EntityManager::LoadData() {
    std::ifstream file("save.dat", std::ios::binary);
    if (file.is_open()) {
        for (auto& e : entities) {
            file.read((char*)&e.elo, sizeof(float));
            file.read((char*)&e.brain, sizeof(Brain));
        }
        file.close();
    }
}

void EntityManager::RespawnAll(Map& map) {
    for (auto& ent : entities) {
        ent.active = true;
        ent.health = 100.0f;
        ent.armor = 0.0f;
        ent.itemHeld = ITEM_NONE;
        ent.weaponCooldown = 0.0f;
        ent.position = FindValidPosition(map);
        ent.lastPosition = ent.position;
        ent.target = ent.position;
        ent.poisonTimer = 0.0f;
        ent.kills = 0;
        ent.eloStartRound = ent.elo;
        ent.scanTimer = GetRandomFloat(0.0f, 0.5f);
    }
}

void EntityManager::EvolveGeneration() {
    std::vector<Entity*> ranked;
    for (auto& e : entities) ranked.push_back(&e);
    std::sort(ranked.begin(), ranked.end(), [](Entity* a, Entity* b) { return a->elo > b->elo; });
    if (ranked.size() >= 3) {
        ranked[0]->elo *= 1.1f;
    }
    SaveData();
}

void EntityManager::ApplyDamage(int targetID, float amount, int attackerID, ItemManager& itemMgr) {
    if (targetID < 0 || targetID >= (int)entities.size()) return;
    Entity& vic = entities[targetID];
    if (!vic.active) return;
    vic.health -= amount;
    if (vic.health <= 0) {
        vic.active = false;
        if (vic.itemHeld != ITEM_NONE) itemMgr.DropItem(vic.position, vic.itemHeld);
        vic.elo *= ELO_DEATH_FACTOR_UNARMED;
        if (attackerID >= 0 && attackerID < (int)entities.size()) {
            entities[attackerID].kills++;
            entities[attackerID].elo += ELO_GAIN_KILL;
        }
    } else {
        vic.elo = fmaxf(0.0f, (vic.elo - ELO_LOSS_HIT));
        if (attackerID >= 0 && attackerID < (int)entities.size()) entities[attackerID].elo += ELO_GAIN_HIT;
    }
}

void EntityManager::ApplyEloPenalty(int entityID, float amount) {
    if (entityID >= 0 && entityID < (int)entities.size()) {
        entities[entityID].elo = fmaxf(0.0f, (entities[entityID].elo - amount));
    }
}

void EntityManager::HealEntity(int id, float amt) {
    if(id>=0 && id<(int)entities.size()) entities[id].health = fminf(entities[id].maxHealth, entities[id].health + amt);
}

void EntityManager::Update(Map& map, Vector2 zoneCenter, float zoneRadius, ItemManager& itemMgr, ProjectileManager& projMgr) {
    float dt = GetFrameTime();
    float worldLimit = MAP_SIZE * TILE_SIZE;
    float killZoneRadius = zoneRadius + 2000.0f;

    for (int y = 0; y < GRID_COUNT; y++) for (int x = 0; x < GRID_COUNT; x++) spatialGrid[x][y].clear();
    for (int i = 0; i < (int)entities.size(); i++) {
        if (!entities[i].active) continue;
        entities[i].position.x = Clamp(entities[i].position.x, 0.0f, worldLimit);
        entities[i].position.y = Clamp(entities[i].position.y, 0.0f, worldLimit);
        int gx = (int)(entities[i].position.x / GRID_CELL_SIZE); 
        int gy = (int)(entities[i].position.y / GRID_CELL_SIZE);
        if (gx >= 0 && gx < GRID_COUNT && gy >= 0 && gy < GRID_COUNT) spatialGrid[gx][gy].push_back(i);
    }
    
    for (int i = 0; i < (int)entities.size(); i++) {
        Entity& ent = entities[i];
        if (!ent.active) continue;

        if (ent.poisonTimer > 0) {
            ent.poisonTimer -= dt;
            ApplyDamage(ent.id, 5.0f * dt, ent.poisonSourceID, itemMgr);
        }

        float distToZone = Vector2Distance(ent.position, zoneCenter);
        if (distToZone > zoneRadius) {
            if (distToZone > killZoneRadius) ent.health = 0;
            else {
                float severity = 1.0f + (distToZone - zoneRadius) / 200.0f;
                ent.health -= (ZONE_DAMAGE_PER_SEC * severity * severity) * dt;
            }
            if (ent.health <= 0) {
                ent.active = false;
                if (ent.itemHeld != ITEM_NONE) itemMgr.DropItem(ent.position, ent.itemHeld);
                continue;
            }
        }
        
        AISystem::Think(ent, *this, itemMgr, projMgr, map, zoneCenter, zoneRadius, dt);
    }
}

void EntityManager::PullEntities(Vector2 center, float radius, float force, int excludeID) {
    int gx = (int)(center.x / GRID_CELL_SIZE); int gy = (int)(center.y / GRID_CELL_SIZE);
    for (int y = gy - 1; y <= gy + 1; y++) {
        for (int x = gx - 1; x <= gx + 1; x++) {
            if (x >= 0 && x < GRID_COUNT && y >= 0 && y < GRID_COUNT) {
                for (int id : spatialGrid[x][y]) {
                    if (entities[id].active && id != excludeID) {
                        float dist = Vector2Distance(center, entities[id].position);
                        if (dist < radius) {
                            Vector2 pullDir = Vector2Normalize(Vector2Subtract(center, entities[id].position));
                            entities[id].position = Vector2Add(entities[id].position, Vector2Scale(pullDir, (1.0f - (dist/radius)) * force));
                        }
                    }
                }
            }
        }
    }
}

int EntityManager::FindNearestEnemyExcluding(Vector2 pos, float range, const std::vector<int>& excludedIDs) {
    int gx = (int)(pos.x / GRID_CELL_SIZE); int gy = (int)(pos.y / GRID_CELL_SIZE);
    int bestID = -1; float minDistSq = range*range;
    for(int y=gy-1; y<=gy+1; ++y) {
        for(int x=gx-1; x<=gx+1; ++x) {
            if(x>=0&&x<GRID_COUNT&&y>=0&&y<GRID_COUNT) {
                for(int id:spatialGrid[x][y]) {
                    if(!entities[id].active) continue;
                    bool skip=false; for(int ex:excludedIDs) if(ex==id){skip=true;break;}
                    if(skip)continue;
                    float d2=Vector2DistanceSqr(pos, entities[id].position);
                    if(d2<minDistSq){minDistSq=d2;bestID=id;}
                }
            }
        }
    }
    return bestID;
}

int EntityManager::FindNearestEnemy(int myID, float range) {
    if (myID < 0 || myID >= (int)entities.size()) return -1;
    const Vector2& myPos = entities[myID].position;
    int gx = (int)(myPos.x/GRID_CELL_SIZE); int gy = (int)(myPos.y/GRID_CELL_SIZE);
    int bestID=-1; float min=range*range;
    for(int y=gy-1; y<=gy+1; ++y) {
        for(int x=gx-1; x<=gx+1; ++x) {
            if(x>=0&&x<GRID_COUNT&&y>=0&&y<GRID_COUNT) {
                for(int id:spatialGrid[x][y]) {
                    if(id!=myID && entities[id].active) {
                        float d2=Vector2DistanceSqr(myPos, entities[id].position);
                        if(d2<min){min=d2;bestID=id;}
                    }
                }
            }
        }
    }
    return bestID;
}

int EntityManager::GetEntityIdAtPositionOptimized(Vector2 p, float r) {
    int gx=(int)(p.x/GRID_CELL_SIZE); int gy=(int)(p.y/GRID_CELL_SIZE);
    for(int y=gy-1; y<=gy+1; ++y) {
        for(int x=gx-1; x<=gx+1; ++x) {
            if(x>=0&&x<GRID_COUNT&&y>=0&&y<GRID_COUNT) {
                for(int id:spatialGrid[x][y]) {
                    if(entities[id].active && CheckCollisionPointCircle(p, entities[id].position, AI_SIZE*0.5f+r)) return id;
                }
            }
        }
    }
    return -1;
}

int EntityManager::GetEntityIdAtPosition(Vector2 p) {
    return GetEntityIdAtPositionOptimized(p, 0);
}

int EntityManager::GetEntityIdAtIndex(int idx, SortMode m) {
    auto r = GetAllRanked(m);
    return (idx >= 0 && idx < (int)r.size()) ? r[idx]->id : -1;
}

bool EntityManager::IsEnemyArmed(int id) {
    return (id >= 0 && id < (int)entities.size()) ? entities[id].itemHeld >= ITEM_PISTOL : false;
}

void EntityManager::Draw(Camera2D c, int sw, int sh, int sel) {} 

int EntityManager::GetAliveCount() const { int c=0; for(const auto& e:entities) if(e.active)c++; return c; }
float EntityManager::GetAverageElo() const { float t=0;int c=0; for(const auto& e:entities) if(e.active){t+=e.elo;c++;} return c?t/c:0; }

std::vector<const Entity*> EntityManager::GetAllRanked(SortMode m) {
    std::vector<const Entity*> r; r.reserve(entities.size());
    for(const auto& e : entities) r.push_back(&e);
    std::sort(r.begin(), r.end(), [m](const auto* a, const auto* b){
        if(m==SortMode::ELO) return a->elo > b->elo;
        if(m==SortMode::KILLS) return a->kills > b->kills;
        if(m==SortMode::HEALTH) return a->health > b->health;
        return a->id < b->id;
    });
    return r;
}

const Entity* EntityManager::GetEntityByID(int id) const { return (id>=0 && id<(int)entities.size()) ? &entities[id] : nullptr; }
Vector2 EntityManager::GetEntityPosition(int id) { return (id>=0) ? entities[id].position : Vector2{0,0}; }
bool EntityManager::IsEntityActive(int id) { return (id>=0) ? entities[id].active : false; }

void EntityManager::ProcessNearMiss(Vector2 p, float r, std::vector<int>& ids) {
    int gx=(int)(p.x/GRID_CELL_SIZE); int gy=(int)(p.y/GRID_CELL_SIZE);
    for (int y = gy - 1; y <= gy + 1; ++y) {
        for (int x = gx - 1; x <= gx + 1; ++x) {
            if (x >= 0 && x < GRID_COUNT && y >= 0 && y < GRID_COUNT) {
                for (int id : spatialGrid[x][y]) {
                    if (entities[id].active) {
                        bool found = false; for (int i : ids) if (i == id) found = true;
                        if (found) continue;
                        if (Vector2DistanceSqr(p, entities[id].position) < (r + AI_SIZE * 0.5f) * (r + AI_SIZE * 0.5f)) {
                            entities[id].elo += ELO_GAIN_DODGE; ids.push_back(id);
                        }
                    }
                }
            }
        }
    }
}

Vector2 EntityManager::FindValidPosition(Map& map) {
    float l = MAP_SIZE * TILE_SIZE;
    for (int i = 0; i < 50; ++i) {
        float x = GetRandomFloat(0, l); float y = GetRandomFloat(0, l);
        if (map.IsWalkable(x, y)) return { x, y };
    }
    return { l / 2, l / 2 };
}