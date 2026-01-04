#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "../World/Map.h"
#include "../Managers/EntityManager.h"
#include "../Managers/ItemManager.h"
#include "../Managers/ProjectileManager.h"
#include "../Managers/ZoneManager.h"
#include "../UI/Sidebar.h"

enum class GameState { PLAYING, END_SCREEN };

class Game {
public:
    void Init();
    void Update();
    void Draw();
    void Close();

private:
    Map map;
    EntityManager entityManager;
    ItemManager itemManager;
    ProjectileManager projectileManager;
    ZoneManager zoneManager;
    Sidebar sidebar;

    Camera2D camera;
    int selectedEntityID = -1;
    bool autoCamEnabled = false;
    float leaderHoldTimer = 0.0f;
    
    // VARIABLE AJOUTÉE ICI
    SortMode currentSort = SortMode::ELO;

    GameState gameState = GameState::PLAYING;
    float endGameTimer = 0.0f;
    std::vector<const Entity*> winners;

    void StartNewRound();
};

#endif