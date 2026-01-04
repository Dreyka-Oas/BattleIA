#include "ZoneManager.h"
#include "../Config/EloConfig.h"
#include "../Config/GameConfig.h"
#include "raymath.h"

void ZoneManager::Init() {
    initialRadius = (MAP_SIZE * TILE_SIZE) * 0.50f;
    radius = initialRadius;
    targetRadius = radius;
    minRadius = initialRadius * ZONE_MIN_SIZE_PERCENT;
    
    center = { (MAP_SIZE * TILE_SIZE) / 2.0f, (MAP_SIZE * TILE_SIZE) / 2.0f };
    targetCenter = center;
    
    timer = ZONE_SHRINK_INTERVAL;
    isShrinking = false;
    isMoving = false;
    moveTimer = 0.0f;
}

void ZoneManager::Update(float dt) {
    // Si la zone est proche de sa taille minimale (avec une marge de sécurité)
    if (radius <= minRadius + 200.0f) {
        
        // Mouvement vers la cible
        if (Vector2Distance(center, targetCenter) > 10.0f) {
            isMoving = true;
            Vector2 dir = Vector2Normalize(Vector2Subtract(targetCenter, center));
            center = Vector2Add(center, Vector2Scale(dir, ZONE_MOVE_SPEED * dt));
            
            if (Vector2Distance(center, targetCenter) < 15.0f) {
                center = targetCenter;
                isMoving = false;
            }
        }
        else {
            isMoving = false;
            if (moveTimer > 0) {
                moveTimer -= dt;
            }
            else {
                // --- MODIFICATION : DÉPLACEMENT MASSIF ---
                // La zone saute de 1000px à 3000px pour forcer le mouvement
                float moveRange = 2500.0f; 
                
                float offsetX = (float)GetRandomValue((int)-moveRange, (int)moveRange);
                float offsetY = (float)GetRandomValue((int)-moveRange, (int)moveRange);
                
                Vector2 nextPoint = Vector2Add(center, { offsetX, offsetY });

                // Clamp pour rester dans la map
                float mapLimit = (float)(MAP_SIZE * TILE_SIZE);
                float margin = radius + 50.0f;
                nextPoint.x = Clamp(nextPoint.x, margin, mapLimit - margin);
                nextPoint.y = Clamp(nextPoint.y, margin, mapLimit - margin);

                targetCenter = nextPoint;
                moveTimer = 15.0f; // Bouge toutes les 15 sec
            }
        }
    }
    else {
        // Rétrécissement
        if (isShrinking) {
            float shrinkSpeed = (initialRadius * ZONE_SHRINK_PERCENT) / ZONE_SHRINK_DURATION;
            radius -= shrinkSpeed * dt;

            if (radius <= targetRadius) {
                radius = targetRadius;
                isShrinking = false;
                timer = ZONE_SHRINK_INTERVAL;
            }
        }
        else {
            if (timer > 0) timer -= dt;
            else {
                float nextRadius = radius - (initialRadius * ZONE_SHRINK_PERCENT);
                if (nextRadius < minRadius) nextRadius = minRadius;
                targetRadius = nextRadius;
                isShrinking = true;
            }
        }
    }
}

void ZoneManager::Draw() {
    DrawCircleLines((int)center.x, (int)center.y, radius, CLR_ZONE_BORDER);
    
    if (isMoving && Vector2Distance(center, targetCenter) > 10.0f) {
        DrawCircleV(targetCenter, 20.0f, ColorAlpha(RED, 0.4f));
        DrawLineEx(center, targetCenter, 4.0f, ColorAlpha(RED, 0.2f));
    }
}