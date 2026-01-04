#include "ProjectileRenderer.h"
#include "../Config/ItemConfig.h"
#include "raymath.h"
#include "rlgl.h"
#include <cmath>

void DrawHighVisLightning(Vector2 start, Vector2 end, float timer) {
    float alpha = timer / 0.35f;
    int segments = (int)(Vector2Distance(start, end) / 25.0f);
    if (segments < 2) segments = 2;
    Color glowCol = { 0, 150, 255, (unsigned char)(200 * alpha) };
    Color coreCol = { 255, 255, 255, (unsigned char)(255 * alpha) };
    Vector2 current = start;
    for (int i = 1; i <= segments; i++) {
        float t = (float)i / segments;
        Vector2 target = Vector2Lerp(start, end, t);
        if (i < segments) {
            target.x += (float)GetRandomValue(-20, 20);
            target.y += (float)GetRandomValue(-20, 20);
        }
        DrawLineEx(current, target, 12.0f * alpha, glowCol);
        DrawLineEx(current, target, 3.0f * alpha, coreCol);
        current = target;
    }
}

void ProjectileRenderer::Draw(const std::vector<Projectile>& projs, 
                              const std::vector<Explosion>& exps,
                              const std::vector<LightningBolt>& bolts) {
    BeginBlendMode(BLEND_ADDITIVE);

    for (const auto& b : bolts) {
        if (!b.active) continue;
        DrawHighVisLightning(b.start, b.end, b.timer);
    }

    for (const auto& p : projs) {
        if (!p.active) continue;

        // --- RENDU SPÉCIFIQUE SHURIKEN STORM ---
        if (p.weaponID == ITEM_SHURIKEN_STORM) {
            float rotation = (float)GetTime() * 2000.0f; // Rotation ultra rapide
            float size = 15.0f;
            
            // Dessin d'une croix tournante (Shuriken à 4 branches)
            for (int i = 0; i < 4; i++) {
                float angle = (rotation + (i * 90.0f)) * DEG2RAD;
                Vector2 tip = { p.position.x + cosf(angle) * size, p.position.y + sinf(angle) * size };
                DrawLineEx(p.position, tip, 4.0f, LIGHTGRAY);
                DrawCircleV(tip, 3.0f, WHITE); // Pointes brillantes
            }
            DrawCircleV(p.position, 5.0f, GRAY); // Centre
            // Sillage (Trail)
            Vector2 tail = Vector2Subtract(p.position, Vector2Scale(Vector2Normalize(p.velocity), 30.0f));
            DrawLineEx(tail, p.position, 2.0f, ColorAlpha(WHITE, 0.3f));
        }
        else if (p.type == PROJ_LASER) {
            Color beamCol = PURPLE; float thick = 10.0f;
            if (p.weaponID == ITEM_GOD_HAND) { beamCol = GOLD; thick = 35.0f; } 
            else if (p.weaponID == ITEM_GAUSS_RIFLE) { beamCol = SKYBLUE; thick = 20.0f; }
            Vector2 end = Vector2Add(p.position, p.velocity);
            DrawLineEx(p.position, end, thick + 15.0f, ColorAlpha(beamCol, 0.3f));
            DrawLineEx(p.position, end, thick, beamCol);
            DrawLineEx(p.position, end, thick * 0.3f, WHITE);
        }
        else if (p.type == PROJ_NUKE) {
            DrawCircleV(p.position, 35.0f, BLACK);
            DrawCircleV(p.position, 20.0f, RED);
        }
        else if (p.type == PROJ_FLAME) {
            float life = p.timer; 
            float radius = 15.0f + (1.0f - life) * 60.0f;
            Color flameCol = (life > 0.5f) ? Color{255,200,50,255} : Color{255,50,0,255};
            DrawCircleGradient((int)p.position.x, (int)p.position.y, radius, ColorAlpha(flameCol, life), BLANK);
        }
        else {
            Color col = GOLD;
            if (p.weaponID == ITEM_ICE_SHARD) col = SKYBLUE;
            if (p.type == PROJ_SONIC) col = WHITE;
            Vector2 tail = Vector2Subtract(p.position, Vector2Scale(Vector2Normalize(p.velocity), 40.0f));
            DrawLineEx(tail, p.position, 6.0f, ColorAlpha(col, 0.8f));
            DrawCircleV(p.position, 5.0f, WHITE);
        }
    }

    for (const auto& e : exps) {
        if (!e.active) continue;
        float lifePct = 1.0f - (e.timer / 0.5f);
        if (e.timer > 0.5f) lifePct = 1.0f - (e.timer / 2.0f);
        float r = e.radius * lifePct;
        if (e.type == PROJ_NUKE) DrawCircleGradient((int)e.position.x, (int)e.position.y, r, WHITE, RED);
        else if (e.type == PROJ_BLACK_HOLE) {
            DrawRing(e.position, r * 0.5f, r, 0, 360, 32, ColorAlpha(PURPLE, 1.0f-lifePct));
            DrawCircleV(e.position, 20, BLACK);
        } else DrawRing(e.position, r - 20.0f, r, 0, 360, 32, ColorAlpha(ORANGE, 1.0f - lifePct));
    }

    EndBlendMode();
}