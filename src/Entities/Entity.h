#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include <string>
#include "Brain.h"
#include "../Config/ItemConfig.h"

struct Entity {
    int id;
    std::string name;
    float elo;
    float eloStartRound; // NOUVEAU : ELO au moment du spawn
    int kills;
    Vector2 position;
    Vector2 target;
    float health;
    float maxHealth;
    float armor;
    float speed;
    bool active;

    float idleTimer;
    float lootTimer;
    float scanTimer;
    float stationaryTimer;
    Vector2 lastPosition;

    ItemType itemHeld;
    float weaponCooldown;

    float poisonTimer;
    int poisonSourceID;

    Brain brain;
};

#endif