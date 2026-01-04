#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "raylib.h"
#include "../../external/entt/entt.hpp"
#include "../Config/ItemConfig.h"
#include "Brain.h"
#include <string>

struct CName {
    std::string name;
};

struct CPosition {
    Vector2 pos;
    Vector2 lastPos;
    Vector2 target;
};

struct CHealth {
    float hp;
    float maxHp;
    float armor;
};

struct CAI {
    float scanTimer;
    Brain brain;
};

struct CWeapon {
    ItemType activeWeapon;
    float cooldown;
    int kills;
    float elo;
    float eloStart;
};

struct CStatus {
    bool active;
    float poisonTimer;
    entt::entity poisonSourceID; // Changé int -> entt::entity
    float stationaryTimer;
    float lootTimer;
};

#endif