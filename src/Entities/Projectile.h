#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "raylib.h"
#include "../Config/ItemConfig.h"
#include <vector>

struct Projectile {
    Vector2 position;
    Vector2 velocity;
    int ownerID;
    ProjType type;
    ItemType weaponID; // Ajouté pour que le Renderer sache quoi dessiner
    float timer;
    bool active;
    
    Vector2 targetPos;
    std::vector<int> dodgedEntityIDs;
    int bounceCount;
    bool damageApplied; 
};

struct Explosion {
    Vector2 position;
    float radius;
    float timer;
    bool active;
    ProjType type;
    int ownerID;
};

struct LightningBolt {
    Vector2 start;
    Vector2 end;
    float timer;
    bool active;
};

#endif