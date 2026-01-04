#include "ProjectileManager.h"
#include "EntityManager.h"
#include "ItemManager.h"
#include "../Config/ItemConfig.h"
#include "../Config/EloConfig.h"
#include "raymath.h"
#include <cmath>

void ProjectileManager::Spawn(Vector2 pos, Vector2 target, int ownerID, ItemType weaponID, ProjType type, float speed) {
    Projectile p;
    p.position = pos;
    p.ownerID = ownerID;
    p.weaponID = weaponID;
    p.type = type;
    p.active = true;
    p.targetPos = target; 
    p.damageApplied = false;

    Vector2 dir = Vector2Normalize(Vector2Subtract(target, pos));

    if (type == PROJ_LASER) {
        p.velocity = Vector2Scale(dir, 15000.0f);
        p.timer = 0.4f; 
    } 
    else if (type == PROJ_CHAIN) {
        p.timer = 0.5f; 
        p.velocity = {0,0};
        // --- AUGMENTATION A 16 CIBLES ---
        p.bounceCount = (weaponID == ITEM_MJOLNIR) ? 16 : 8; 
    }
    else {
        p.velocity = Vector2Scale(dir, speed);
        float distance = Vector2Distance(pos, target);
        p.timer = distance / (speed > 0 ? speed : 1.0f);
        if (type == PROJ_FLAME) p.timer = 0.7f;
    }
    projectiles.push_back(p);
}

void ProjectileManager::Update(EntityManager& entityMgr, ItemManager& itemMgr) {
    float dt = GetFrameTime();
    
    for (auto it = lightnings.begin(); it != lightnings.end();) {
        it->timer -= dt;
        if (it->timer <= 0) it = lightnings.erase(it); else ++it;
    }

    for (auto& e : explosions) { 
        if (!e.active) continue; 
        e.timer -= dt; 
        if (e.timer <= 0) e.active = false; 
    }

    for (auto& p : projectiles) {
        if (!p.active) continue;

        // --- LOGIQUE CHAIN LIGHTNING (DÉGRESSIF) ---
        if (p.type == PROJ_CHAIN) {
            if (!p.damageApplied) {
                std::vector<int> history;
                history.push_back(p.ownerID);

                int targetID = entityMgr.GetEntityIdAtPositionOptimized(p.targetPos, 150.0f);
                if (targetID == -1) targetID = entityMgr.FindNearestEnemyExcluding(p.targetPos, 400.0f, history);

                if (targetID != -1) {
                    Vector2 currentPos = p.position;
                    // Initialisation des dégâts avec la valeur de l'arme
                    float currentDmg = ITEM_STATS[p.weaponID].damage;

                    for (int b = 0; b < p.bounceCount; b++) {
                        const Entity* target = entityMgr.GetEntityByID(targetID);
                        if (!target || !target->active) break;

                        // Applique les dégâts ACTUELS
                        entityMgr.ApplyDamage(target->id, currentDmg, p.ownerID, itemMgr);
                        
                        // --- DIVISION PAR DEUX POUR LA PROCHAINE IA ---
                        currentDmg *= 0.5f;
                        if (currentDmg < 0.5f) currentDmg = 0.5f; // Dégât minimum symbolique

                        lightnings.push_back({ currentPos, target->position, 0.35f, true });

                        currentPos = target->position;
                        history.push_back(targetID);
                        
                        // Rebond vers le suivant
                        targetID = entityMgr.FindNearestEnemyExcluding(currentPos, 600.0f, history);
                        if (targetID == -1) break;
                    }
                }
                p.damageApplied = true;
            }
            p.active = false;
            continue;
        }

        // --- AUTRES LOGIQUES ---
        if (p.type == PROJ_LASER) {
            if (!p.damageApplied) {
                Vector2 end = Vector2Add(p.position, p.velocity);
                int steps = 150; 
                for(int i=1; i<=steps; i++) {
                    Vector2 check = Vector2Lerp(p.position, end, (float)i/steps);
                    int hitID = entityMgr.GetEntityIdAtPositionOptimized(check, 40.0f);
                    if (hitID != -1 && hitID != p.ownerID) {
                        float dmg = ITEM_STATS[p.weaponID].damage;
                        if (p.weaponID == ITEM_GAUSS_RIFLE) {
                            float falloff = fmaxf(0.2f, 1.0f - (Vector2Distance(p.position, check) / 10000.0f));
                            dmg *= falloff;
                        }
                        entityMgr.ApplyDamage(hitID, dmg, p.ownerID, itemMgr);
                        if (p.weaponID != ITEM_ORBITAL_BEAM && p.weaponID != ITEM_GAUSS_RIFLE && p.weaponID != ITEM_GOD_HAND) break; 
                    }
                }
                p.damageApplied = true;
            }
            p.timer -= dt; if (p.timer <= 0) p.active = false;
            continue;
        }

        p.position = Vector2Add(p.position, Vector2Scale(p.velocity, dt * 60.0f));
        p.timer -= dt;
        int hitID = entityMgr.GetEntityIdAtPositionOptimized(p.position, PROJECTILE_RADIUS);
        bool triggered = (hitID != -1 && hitID != p.ownerID) || (p.timer <= 0);
        if (triggered) {
            if (hitID != -1) entityMgr.ApplyDamage(hitID, ITEM_STATS[p.weaponID].damage, p.ownerID, itemMgr);
            p.active = false;
        }
        if (p.position.x < 0 || p.position.x > MAP_SIZE * TILE_SIZE || p.position.y < 0 || p.position.y > MAP_SIZE * TILE_SIZE) p.active = false;
    }
}

void ProjectileManager::Draw() { }
void ProjectileManager::Clear() { projectiles.clear(); explosions.clear(); lightnings.clear(); }