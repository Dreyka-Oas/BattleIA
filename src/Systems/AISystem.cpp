#include "AISystem.h"
#include "../Managers/EntityManager.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/LootSystem.h"
#include "../Config/GameConfig.h"
#include "../Config/ItemConfig.h"
#include "raymath.h"
#include <cmath>

// Helper pour disperser les tirs
Vector2 ApplySpread(Vector2 target, Vector2 origin, float spreadAngleDeg) {
    if (spreadAngleDeg <= 0.1f) return target;
    Vector2 dir = Vector2Normalize(Vector2Subtract(target, origin));
    float angle = atan2f(dir.y, dir.x);
    float spreadRad = spreadAngleDeg * DEG2RAD;
    float randomAngle = angle + ((float)GetRandomValue(-100, 100) / 100.0f) * spreadRad;
    float dist = Vector2Distance(target, origin);
    return { origin.x + cosf(randomAngle) * dist, origin.y + sinf(randomAngle) * dist };
}

void AISystem::Think(Entity& ent, EntityManager& em, ItemManager& im, ProjectileManager& pm, Map& map, Vector2 zoneCenter, float zoneRadius, float dt) {
    if (!ent.active) return;

    // --- 1. MISE À JOUR DES TIMERS ---
    ent.scanTimer -= dt;
    if (ent.weaponCooldown > 0) ent.weaponCooldown -= dt;
    if (ent.lootTimer > 0) ent.lootTimer -= dt;

    float distToZone = Vector2Distance(ent.position, zoneCenter);

    // --- 2. LOGIQUE DE DÉCISION (CERVEAU) ---
    // On force une nouvelle cible si l'IA est trop proche de sa cible actuelle (bloquée)
    bool isAtTarget = Vector2DistanceSqr(ent.position, ent.target) < 100.0f;

    if (ent.scanTimer <= 0 || isAtTarget) {
        ent.scanTimer = 0.8f + ((float)GetRandomValue(0, 40) / 100.0f);

        // PRIORITÉ 1 : Revenir dans la zone
        if (distToZone > zoneRadius * 0.9f) {
            ent.target = zoneCenter;
        }
        else {
            int nearestEnemyIdx = em.FindNearestEnemy(ent.id, 15000.0f);
            int nearestLootIdx = im.GetNearestItemIndex(ent.position);
            bool hasTarget = false;

            // PRIORITÉ 2 : Soin si vie basse
            if (ent.health < 40.0f && nearestLootIdx != -1) {
                Vector2 lootPos = im.GetItemPosition(nearestLootIdx);
                if (Vector2Distance(ent.position, lootPos) < 2000.0f) {
                    ent.target = lootPos;
                    hasTarget = true;
                }
            }

            // PRIORITÉ 3 : Attaquer si on a une arme
            if (!hasTarget && nearestEnemyIdx != -1 && ent.itemHeld >= ITEM_PISTOL) {
                const Entity* enemy = em.GetEntityByID(nearestEnemyIdx);
                if (enemy && enemy->active) {
                    float range = ITEM_STATS[ent.itemHeld].range;
                    // On s'approche si on est hors de portée, sinon on tourne autour
                    if (Vector2Distance(ent.position, enemy->position) > range * 0.8f) {
                        ent.target = enemy->position;
                    }
                    else {
                        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                        ent.target = Vector2Add(ent.position, { cosf(angle) * 500.0f, sinf(angle) * 500.0f });
                    }
                    hasTarget = true;
                }
            }

            // PRIORITÉ 4 : Chercher une arme si on n'en a pas
            if (!hasTarget && nearestLootIdx != -1 && ent.itemHeld == ITEM_NONE) {
                ent.target = im.GetItemPosition(nearestLootIdx);
                hasTarget = true;
            }

            // PRIORITÉ 5 : Exploration aléatoire (si rien d'autre)
            if (!hasTarget) {
                float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
                float dist = (float)GetRandomValue(600, 2000);
                Vector2 randomDest = Vector2Add(ent.position, { cosf(angle) * dist, sinf(angle) * dist });

                // On s'assure que la cible est dans la map
                ent.target.x = Clamp(randomDest.x, 50, (MAP_SIZE * TILE_SIZE) - 50);
                ent.target.y = Clamp(randomDest.y, 50, (MAP_SIZE * TILE_SIZE) - 50);
            }
        }
    }

    // --- 3. MOUVEMENT PHYSIQUE (LES MUSCLES) ---
    // On calcule le vecteur de mouvement
    Vector2 moveDir = MovementSystem::AvoidObstacles(ent.position, ent.target, map);

    // Si l'évitement d'obstacles échoue, on force la direction vers la cible
    if (Vector2LengthSqr(moveDir) < 0.01f) {
        moveDir = Vector2Normalize(Vector2Subtract(ent.target, ent.position));
    }

    float speedMult = 1.0f;
    if (map.GetTileTypeAt(ent.position.x, ent.position.y) == TILE_WATER) speedMult = CFG_WATER_SPEED_MULT;

    // Application du mouvement (vitesse * multiplicateur eau * delta time * normalisation 60fps)
    Vector2 velocity = Vector2Scale(moveDir, ent.speed * speedMult * dt * 60.0f);

    ent.lastPosition = ent.position;
    ent.position = Vector2Add(ent.position, velocity);

    // --- 4. ACTION DE TIR ---
    if (ent.itemHeld >= ITEM_PISTOL && ent.weaponCooldown <= 0) {
        const ItemProperties& stats = ITEM_STATS[ent.itemHeld];
        int targetID = em.FindNearestEnemy(ent.id, stats.range);
        if (targetID != -1) {
            const Entity* enemy = em.GetEntityByID(targetID);
            if (enemy && enemy->active) {
                int count = (stats.projCount > 0) ? stats.projCount : 1;
                for (int i = 0; i < count; i++) {
                    Vector2 aimPos = ApplySpread(enemy->position, ent.position, stats.spread);
                    pm.Spawn(ent.position, aimPos, ent.id, ent.itemHeld, stats.projType, stats.projSpeed);
                }
                ent.weaponCooldown = stats.cooldown;
            }
        }
    }

    // --- 5. RAMASSAGE D'OBJETS ---
    LootSystem::CheckPickup(ent, im);
}