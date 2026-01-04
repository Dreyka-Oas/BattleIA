#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "raylib.h"
#include "../Managers/EntityManager.h"

class ItemManager;
class ZoneManager;

class Sidebar {
public:
    // On passe SortMode& pour que la Sidebar puisse modifier le tri du Jeu
    void Update(EntityManager& em, float sidebarWidth, int& selectedID, float screenHeight, bool& autoCam, SortMode& sortMode);
    void Draw(EntityManager& em, float sidebarWidth, int selectedID, float screenHeight, bool& autoCam, SortMode& sortMode, const ZoneManager& zm, const ItemManager& im);

private:
    float scrollY = 0.0f;
    float maxScrollY = 0.0f;
};

#endif