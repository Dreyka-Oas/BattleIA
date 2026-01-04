#define _CRT_SECURE_NO_WARNINGS
#include "Sidebar.h"
#include "../Managers/ResourceManager.h"
#include "../Config/GameConfig.h"
#include "../Config/ItemConfig.h"
#include "../Managers/ZoneManager.h" 
#include "../Managers/ItemManager.h"
#include "raygui.h"
#include "raymath.h"

float GetListStartY(bool hasSelection) {
    float y = 15.0f + 60.0f + 10.0f + 40.0f + 10.0f + 20.0f + 10.0f;
    if (hasSelection) y += 80.0f + 10.0f;
    return y + 35.0f; 
}

void Sidebar::Update(EntityManager& em, float sidebarWidth, int& selectedID, float screenHeight, bool& autoCam, SortMode& sortMode) {
    bool hasSelection = (selectedID != -1 && em.GetEntityByID(selectedID) != nullptr);
    float listStartY = GetListStartY(hasSelection);

    if (GetMousePosition().x < sidebarWidth) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            scrollY -= wheel * 50.0f;
            if (scrollY < 0) scrollY = 0;
            if (scrollY > maxScrollY) scrollY = maxScrollY;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            float headerY = listStartY - 35.0f;

            if (m.y > headerY && m.y < listStartY) {
                if (m.x < 50) sortMode = SortMode::RANK;
                else if (m.x < 130) sortMode = SortMode::NAME;
                else if (m.x < 170) sortMode = SortMode::HEALTH;
                else if (m.x < 210) sortMode = SortMode::KILLS;
                else sortMode = SortMode::ELO;
            }

            if (m.y > listStartY && m.y < screenHeight) {
                float rowH = 50.0f;
                int idx = (int)((m.y - listStartY + scrollY) / rowH);
                auto ranked = em.GetAllRanked(sortMode);
                if (idx >= 0 && idx < (int)ranked.size()) {
                    selectedID = ranked[idx]->id;
                    autoCam = false; 
                }
            }
        }
    }
}

void Sidebar::Draw(EntityManager& em, float sidebarWidth, int selectedID, float screenHeight, bool& autoCam, SortMode& sortMode, const ZoneManager& zm, const ItemManager& im) {
    Font font = ResourceManager::GetSystemFont();
    DrawRectangle(0, 0, (int)sidebarWidth, (int)screenHeight, UI_SIDEBAR_BG);
    DrawRectangleGradientH((int)sidebarWidth, 0, 15, (int)screenHeight, ColorAlpha(BLACK, 0.15f), BLANK);

    float lY = 15.0f; float cardW = (sidebarWidth - 45.0f) / 2.0f;
    DrawRectangleRounded(Rectangle{ 15, lY, cardW, 60 }, 0.2f, 8, UI_MAIN_BG);
    DrawTextEx(font, "VIVANTS", Vector2{ 25, lY + 8 }, 10, 1.0f, UI_TEXT_DIM);
    DrawTextEx(font, TextFormat("%i", em.GetAliveCount()), Vector2{ 25, lY + 22 }, 22, 1.0f, UI_TEXT_MAIN);
    DrawRectangleRounded(Rectangle{ 15 + cardW + 15, lY, cardW, 60 }, 0.2f, 8, UI_ACCENT_LIGHT);
    DrawTextEx(font, "AVG ELO", Vector2{ 15 + cardW + 25, lY + 8 }, 10, 1.0f, UI_ACCENT);
    DrawTextEx(font, TextFormat("%.0f", em.GetAverageElo()), Vector2{ 15 + cardW + 25, lY + 22 }, 24, 1.0f, UI_ACCENT);
    lY += 70.0f;

    Rectangle cZ = { 15, lY, sidebarWidth - 30, 40 };
    Color zoneBg = zm.isShrinking ? UI_DEAD_BG : UI_MAIN_BG;
    const char* zS = zm.isMoving ? "ZONE MOBILE" : (zm.isShrinking ? "RETRECISSEMENT" : TextFormat("REDUCTION %.0fs", zm.timer));
    DrawRectangleRounded(cZ, 0.2f, 8, zoneBg);
    DrawTextEx(font, zS, Vector2{ 25, lY + 12 }, 12, 1.0f, zm.isShrinking ? RED : UI_TEXT_DIM);
    lY += 50.0f;

    GuiCheckBox(Rectangle{ 15, lY, 20, 20 }, "SUIVRE LEADER", &autoCam);
    DrawTextEx(font, TextFormat("LOOT: %.0fs", im.GetTimeNextSpawn()), Vector2{ sidebarWidth - 100, lY + 5 }, 10, 1.0f, UI_TEXT_DIM);
    lY += 30.0f;

    bool hasSel = (selectedID != -1);
    if (hasSel) {
        const Entity* sel = em.GetEntityByID(selectedID);
        if (sel) {
            Rectangle insp = { 15, lY, sidebarWidth - 30, 80 };
            DrawRectangleRounded(insp, 0.2f, 8, sel->active ? UI_CARD_BG : UI_DEAD_BG);
            DrawRectangleRoundedLines(insp, 0.2f, 8, sel->active ? UI_ACCENT : RED);
            DrawCircle((int)(insp.x + 30), (int)(insp.y + 40), 20, sel->active ? UI_ACCENT : RED);
            DrawTextEx(font, TextFormat("%i", sel->id), Vector2{ insp.x + 22, insp.y + 32 }, 16, 1.0f, WHITE);
            DrawTextEx(font, sel->name.c_str(), Vector2{ insp.x + 60, insp.y + 10 }, 16, 1.0f, UI_TEXT_MAIN);
            if (sel->itemHeld != ITEM_NONE) {
                DrawRectangleRounded(Rectangle{ insp.x + 60, insp.y + 30, 120, 20 }, 0.5f, 4, ITEM_STATS[sel->itemHeld].color);
                DrawTextEx(font, ITEM_STATS[sel->itemHeld].name, Vector2{ insp.x + 70, insp.y + 32 }, 14, 1.0f, WHITE);
            }
            Rectangle hpB = { insp.x + 60, insp.y + 55, insp.width - 120, 6 };
            DrawRectangleRec(hpB, UI_BORDER);
            DrawRectangleRec(Rectangle{ hpB.x, hpB.y, hpB.width * Clamp(sel->health / 100.0f, 0.0f, 1.0f), 6 }, UI_HEALTH_GRN);
            lY += 90.0f;
        } else hasSel = false;
    }

    // --- LISTE EN-TÊTES ---
    DrawRectangle(0, (int)lY, (int)sidebarWidth, 35, UI_MAIN_BG);
    float hy = lY + 11;
    DrawTextEx(font, "RANG", Vector2{ 15, hy }, 10, 1.0f, UI_TEXT_DIM);
    DrawTextEx(font, "NOM", Vector2{ 50, hy }, 10, 1.0f, UI_TEXT_DIM);
    DrawTextEx(font, "VIE", Vector2{ 130, hy }, 10, 1.0f, UI_TEXT_DIM);
    DrawTextEx(font, "KILLS", Vector2{ 170, hy }, 10, 1.0f, UI_TEXT_DIM);
    DrawTextEx(font, "GAIN", Vector2{ 215, hy }, 10, 1.0f, UI_ACCENT); // NOUVEAU
    DrawTextEx(font, "ELO", Vector2{ sidebarWidth - 45, hy }, 10, 1.0f, UI_TEXT_DIM);

    auto ranked = em.GetAllRanked(sortMode);
    float rowH = 50.0f;
    float startListY = lY + 40.0f;
    float listH = screenHeight - startListY;

    maxScrollY = (ranked.size() * rowH) - listH;
    if (maxScrollY < 0) maxScrollY = 0;

    BeginScissorMode(0, (int)startListY, (int)sidebarWidth, (int)listH);
    int startIndex = (int)(scrollY / rowH);
    int endIndex = startIndex + (int)(listH / rowH) + 2;
    if (endIndex > (int)ranked.size()) endIndex = (int)ranked.size();

    for (int i = startIndex; i < endIndex; i++) {
        float iy = startListY + (i * rowH) - scrollY;
        const auto* e = ranked[i];
        bool sel = (e->id == selectedID);
        Rectangle r = { 10.0f, iy, sidebarWidth - 20, rowH - 6 };
        DrawRectangleRounded(r, 0.2f, 8, e->active ? (sel ? UI_ACCENT_DIM : UI_MAIN_BG) : UI_DEAD_BG);
        if (sel) DrawRectangleRoundedLines(r, 0.2f, 8, UI_ACCENT);

        DrawTextEx(font, TextFormat("#%i", i + 1), Vector2{ 20, iy + 18 }, 12, 1.0f, UI_TEXT_DIM);
        DrawTextEx(font, e->name.c_str(), Vector2{ 50, iy + 18 }, 12, 1.0f, UI_TEXT_MAIN);
        
        DrawRectangleRec(Rectangle{ 130, iy + 22, 30, 4 }, UI_BORDER);
        DrawRectangleRec(Rectangle{ 130, iy + 22, 30.0f * (Clamp(e->health, 0, 100)/100.0f), 4 }, UI_HEALTH_GRN);

        DrawTextEx(font, TextFormat("%i", e->kills), Vector2{ 180, iy + 18 }, 12, 1.0f, UI_TEXT_MAIN);

        // CALCUL DU GAIN
        float gain = e->elo - e->eloStartRound;
        Color gainCol = (gain >= 0) ? UI_HEALTH_GRN : RED;
        DrawTextEx(font, TextFormat("%+ .0f", gain), Vector2{ 215, iy + 18 }, 12, 1.0f, gainCol);

        DrawTextEx(font, TextFormat("%.0f", e->elo), Vector2{ sidebarWidth - 50, iy + 18 }, 12, 1.0f, UI_TEXT_DIM);
    }
    EndScissorMode();
}