#include "EntityRenderer.h"
#include "../Config/GameConfig.h"
#include "../Config/ItemConfig.h"
#include "raymath.h"

// Helper pour dessiner un coin
void DrawCorner(float x, float y, float w, float h, float thick, Color color) {
    DrawRectangle((int)x, (int)y, (int)w, (int)thick, color); 
    DrawRectangle((int)x, (int)y, (int)thick, (int)h, color); 
}

void EntityRenderer::DrawAll(const std::vector<Entity>& entities, Camera2D camera, int sw, int sh, int selID) {
    Vector2 tl = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 br = GetScreenToWorld2D({ (float)sw, (float)sh }, camera);
    float margin = AI_SIZE * 2.5f;

    for (const auto& ent : entities) {
        if (!ent.active) continue;
        if (ent.position.x < tl.x - margin || ent.position.x > br.x + margin ||
            ent.position.y < tl.y - margin || ent.position.y > br.y + margin) continue;

        bool isSelected = (ent.id == selID);
        Vector2 p = { ent.position.x - AI_SIZE / 2.0f, ent.position.y - AI_SIZE / 2.0f };

        // 1. CORPS
        DrawRectangleV(p, { AI_SIZE, AI_SIZE }, CLR_AI_BODY);

        // 2. SELECTION (Coins Blancs + Cercle)
        if (isSelected) {
            float circleRadius = AI_SIZE * 0.85f;
            DrawCircleLines((int)ent.position.x, (int)ent.position.y, circleRadius, ColorAlpha(WHITE, 0.4f));

            float gap = 8.0f;
            float len = 35.0f;
            float thick = 8.0f;
            Color selCol = WHITE;
            Color shadowCol = BLACK;

            float l = p.x - gap;
            float t = p.y - gap;
            float r = p.x + AI_SIZE + gap;
            float b = p.y + AI_SIZE + gap;

            // Ombres
            DrawCorner(l+2, t+2, len, len, thick, ColorAlpha(shadowCol, 0.5f));
            DrawRectangle((int)(r - len + 2), (int)(t + 2), (int)len, (int)thick, ColorAlpha(shadowCol, 0.5f));
            DrawRectangle((int)(r - thick + 2), (int)(t + 2), (int)thick, (int)len, ColorAlpha(shadowCol, 0.5f));
            DrawRectangle((int)(l + 2), (int)(b - thick + 2), (int)len, (int)thick, ColorAlpha(shadowCol, 0.5f));
            DrawRectangle((int)(l + 2), (int)(b - len + 2), (int)thick, (int)len, ColorAlpha(shadowCol, 0.5f));
            DrawRectangle((int)(r - len + 2), (int)(b - thick + 2), (int)len, (int)thick, ColorAlpha(shadowCol, 0.5f));
            DrawRectangle((int)(r - thick + 2), (int)(b - len + 2), (int)thick, (int)len, ColorAlpha(shadowCol, 0.5f));

            // Coins Blancs
            DrawCorner(l, t, len, len, thick, selCol);
            DrawRectangle((int)(r - len), (int)t, (int)len, (int)thick, selCol);
            DrawRectangle((int)(r - thick), (int)t, (int)thick, (int)len, selCol);
            DrawRectangle((int)l, (int)(b - thick), (int)len, (int)thick, selCol);
            DrawRectangle((int)l, (int)(b - len), (int)thick, (int)len, selCol);
            DrawRectangle((int)(r - len), (int)(b - thick), (int)len, (int)thick, selCol);
            DrawRectangle((int)(r - thick), (int)(b - len), (int)thick, (int)len, selCol);
        } 
        else {
            DrawRectangleLinesEx({ p.x, p.y, AI_SIZE, AI_SIZE }, 1.0f, ColorAlpha(BLACK, 0.2f));
        }

        // 3. ARME
        if (ent.itemHeld > ITEM_NONE) {
            DrawCircleV(ent.position, AI_SIZE / 3.0f, ITEM_STATS[ent.itemHeld].color);
            DrawCircleLines((int)ent.position.x, (int)ent.position.y, (int)(AI_SIZE / 3.0f), ColorAlpha(BLACK, 0.5f));
        }

        // 4. BARRE DE VIE (LOD)
        if (camera.zoom > 0.10f) {
            float bW = AI_SIZE + 20.0f; float bH = 8.0f;
            Vector2 bP = { ent.position.x - bW / 2.0f, p.y - bH - 15.0f };
            
            // Fond
            DrawRectangleV(bP, { bW, bH }, ColorAlpha(BLACK, 0.6f));
            // Vie
            DrawRectangleV(bP, { bW * (ent.health / ent.maxHealth), bH }, UI_HEALTH_GRN);
            
            // CONTOUR NOIR ÉPAIS (3.0f)
            DrawRectangleLinesEx({ bP.x - 2, bP.y - 2, bW + 4, bH + 4 }, 3.0f, BLACK);

            if (ent.armor > 0) {
                float armPct = Clamp(ent.armor / 100.0f, 0.0f, 1.0f);
                DrawRectangleV({ bP.x, bP.y + bH + 3 }, { bW * armPct, 4 }, UI_ACCENT);
                // Petit contour noir pour l'armure aussi
                DrawRectangleLinesEx({ bP.x - 1, bP.y + bH + 2, bW * armPct + 2, 6 }, 1.5f, BLACK);
            }
        }
    }
}