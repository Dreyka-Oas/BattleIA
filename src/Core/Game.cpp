#define _CRT_SECURE_NO_WARNINGS
#include "Game.h"
#include "../Managers/ResourceManager.h"
#include "../Utils/PlatformUtils.h"
#include "../Config/EloConfig.h"
#include "../Config/GameConfig.h"
#include "../Rendering/EntityRenderer.h"
#include "../Rendering/ProjectileRenderer.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h"
#include <cstdio>
#include <algorithm>

void SetupStyle() {
    GuiSetStyle(DEFAULT, TEXT_SIZE, 14);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0xffffffff); 
}

void Game::Init() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(CFG_INITIAL_WIDTH, CFG_INITIAL_HEIGHT, CFG_WINDOW_TITLE);
    GuiSetFont(ResourceManager::GetSystemFont());
    SetupStyle();
    map.Generate();
    entityManager.Init(map);
    StartNewRound();
}

void Game::StartNewRound() {
    itemManager.Init();
    projectileManager.Clear();
    entityManager.RespawnAll(map);
    zoneManager.Init();
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();
    float sidebarW = sw * CFG_SIDEBAR_RATIO;
    camera.target = { (MAP_SIZE * TILE_SIZE) / 2.0f, (MAP_SIZE * TILE_SIZE) / 2.0f };
    camera.offset = { sidebarW + (sw - sidebarW) / 2.0f, sh / 2.0f };
    camera.zoom = 0.05f; 
    SetTargetFPS(CFG_TARGET_FPS);
    gameState = GameState::PLAYING;
    endGameTimer = 0.0f;
    selectedEntityID = -1;
    winners.clear();
    leaderHoldTimer = 0.0f;
}

void Game::Update() {
    float dt = GetFrameTime();
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();
    float sidebarW = sw * CFG_SIDEBAR_RATIO;
    camera.offset = { sidebarW + (sw - sidebarW) / 2.0f, sh / 2.0f };

    if (gameState == GameState::PLAYING) {
        if (entityManager.GetAliveCount() <= 1) {
            gameState = GameState::END_SCREEN;
            endGameTimer = 5.0f;
            winners = entityManager.GetAllRanked(SortMode::RANK);
            entityManager.EvolveGeneration();
            return;
        }

        zoneManager.Update(dt);
        itemManager.Update(map, zoneManager.center, zoneManager.radius);
        entityManager.Update(map, zoneManager.center, zoneManager.radius, itemManager, projectileManager);
        projectileManager.Update(entityManager, itemManager);
        sidebar.Update(entityManager, sidebarW, selectedEntityID, sh, autoCamEnabled, currentSort);

        static int pendingLeaderID = -1;
        if (autoCamEnabled) {
            auto ranked = entityManager.GetAllRanked(currentSort);
            const Entity* leader = nullptr;
            for (const auto* e : ranked) { if (e->active) { leader = e; break; } }
            if (leader) {
                if (selectedEntityID == -1 || !entityManager.IsEntityActive(selectedEntityID)) {
                    selectedEntityID = leader->id;
                    pendingLeaderID = leader->id;
                    leaderHoldTimer = 0.0f;
                } else if (selectedEntityID != leader->id) {
                    if (leader->id == pendingLeaderID) {
                        leaderHoldTimer += dt;
                        if (leaderHoldTimer >= 2.0f) { 
                            selectedEntityID = leader->id;
                            leaderHoldTimer = 0.0f;
                        }
                    } else {
                        pendingLeaderID = leader->id;
                        leaderHoldTimer = 0.0f;
                    }
                } else {
                    pendingLeaderID = leader->id;
                    leaderHoldTimer = 0.0f;
                }
            }
        }

        if (selectedEntityID != -1 && entityManager.IsEntityActive(selectedEntityID)) {
            Vector2 targetPos = entityManager.GetEntityPosition(selectedEntityID);
            float lerp = autoCamEnabled ? 10.0f : 5.0f;
            camera.target = Vector2Lerp(camera.target, targetPos, lerp * dt);
        }
    } else if (gameState == GameState::END_SCREEN) {
        endGameTimer -= dt;
        if (endGameTimer <= 0) StartNewRound();
    }

    if (GetMousePosition().x > sidebarW) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            if (autoCamEnabled && selectedEntityID != -1) {
                camera.zoom = Clamp(camera.zoom + wheel * 0.1f * camera.zoom, MIN_ZOOM, MAX_ZOOM);
            } else {
                Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), camera);
                camera.offset = GetMousePosition();
                camera.target = mouseWorld;
                camera.zoom = Clamp(camera.zoom + wheel * 0.1f * camera.zoom, MIN_ZOOM, MAX_ZOOM);
            }
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            selectedEntityID = -1; autoCamEnabled = false;
            camera.target = Vector2Subtract(camera.target, Vector2Scale(GetMouseDelta(), 1.0f / camera.zoom));
        }
        Rectangle btnRect = { sw - 120, 20, 100, 32 };
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(GetMousePosition(), btnRect)) {
             int id = entityManager.GetEntityIdAtPosition(GetScreenToWorld2D(GetMousePosition(), camera));
             if (id != -1) { selectedEntityID = id; autoCamEnabled = false; }
        }
    }
    
    if (IsKeyPressed(CFG_FULLSCREEN_KEY)) ToggleBorderlessWindowed();
    SetWindowTitle(CFG_WINDOW_TITLE);
}

void Game::Draw() {
    float sw = (float)GetScreenWidth(); float sh = (float)GetScreenHeight();
    float sidebarW = sw * CFG_SIDEBAR_RATIO;
    Font font = ResourceManager::GetSystemFont();
    BeginDrawing();
    ClearBackground(CLR_DEEP_BLUE);
    BeginScissorMode((int)sidebarW, 0, (int)(sw - sidebarW), (int)sh);
    BeginMode2D(camera);
        map.Draw(camera, (int)sw, (int)sh);
        itemManager.Draw();
        ProjectileRenderer::Draw(projectileManager.GetProjectiles(), projectileManager.GetExplosions(), projectileManager.GetLightnings());
        EntityRenderer::DrawAll(entityManager.GetEntities(), camera, (int)sw, (int)sh, selectedEntityID);
        zoneManager.Draw();
    EndMode2D();
    EndScissorMode();
    sidebar.Draw(entityManager, sidebarW, selectedEntityID, sh, autoCamEnabled, currentSort, zoneManager, itemManager);
    if (gameState == GameState::END_SCREEN) {
        DrawRectangle((int)sidebarW, 0, (int)(sw-sidebarW), (int)sh, ColorAlpha(BLACK, 0.7f));
        DrawTextEx(font, "GENERATION TERMINEE", Vector2{ sidebarW + (sw-sidebarW)/2 - 150, sh/2 - 50 }, 30, 1, WHITE);
    }
    EndDrawing();
}

void Game::Close() { 
    // SAUVEGARDE AVANT DE FERMER
    entityManager.SaveData(); 
    map.Unload(); 
    ResourceManager::UnloadResources(); 
    CloseWindow(); 
}