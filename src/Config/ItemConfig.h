#ifndef ITEM_CONFIG_H
#define ITEM_CONFIG_H

#include "raylib.h"

enum ProjType { 
    PROJ_BULLET, PROJ_GRENADE, PROJ_LASER, 
    PROJ_BLACK_HOLE, PROJ_CHAIN, PROJ_VAMPIRE, 
    PROJ_CLUSTER, PROJ_SONIC, PROJ_NUKE,
    PROJ_FLAME
};

enum ItemType {
    ITEM_NONE, ITEM_HEAL, ITEM_ARMOR,
    ITEM_PISTOL, ITEM_SMG, ITEM_ARBALETE, ITEM_SHOTGUN, ITEM_FLAMETHROWER,
    ITEM_MINIGUN, ITEM_SNIPER, ITEM_BAZOOKA, ITEM_MISSILE_LAUNCHER,
    ITEM_PLASMA, ITEM_LASER, ITEM_GAUSS_RIFLE, ITEM_EXPLOSIVE_BOW,
    ITEM_ORBITAL_BEAM, ITEM_TESLA, ITEM_BLACK_HOLE, ITEM_VAMPIRE,
    ITEM_CLUSTER, ITEM_SONIC, ITEM_METEOR, ITEM_BIOLAB, ITEM_NUKE, ITEM_GOD_HAND,
    ITEM_MJOLNIR, ITEM_FIRE_DRAGON, ITEM_ICE_SHARD, ITEM_SHURIKEN_STORM,
    ITEM_COUNT_ENUM
};

struct ItemProperties {
    const char* name;
    float range;        
    float cooldown;     
    float damage;       
    int rarityWeight;   
    Color color;        
    ProjType projType;  
    float projSpeed;    
    int projCount;      
    float spread;       
};

// --- ARSENAL COMPLET : TOUTES LES RARETÉS RÉACTIVÉES ---
inline const ItemProperties ITEM_STATS[] = {
    // [0-2] Consommables
    { "Mains Nues",     0,      0,    0,    0,   BLANK, PROJ_BULLET, 0, 0, 0 },
    { "Kit Soin",       0,      0,    50,   45,  GREEN, PROJ_BULLET, 0, 0, 0 },
    { "Armure",         0,      0,    75,   35,  BLUE,  PROJ_BULLET, 0, 0, 0 },

    // [3-7] Armes Courantes (Rarity 60-80)
    { "Pistolet",       1400,   0.5f, 15,   80,  LIGHTGRAY, PROJ_BULLET, 35.0f, 1, 2.0f },
    { "Mitraillette",   1100,   0.1f, 7,    70,  DARKGRAY,  PROJ_BULLET, 32.0f, 1, 7.0f },
    { "Arbalete",       1800,   0.9f, 22,   60,  BEIGE,     PROJ_BULLET, 50.0f, 1, 1.0f },
    { "Shotgun",        800,    1.0f, 9,    40,  BROWN,     PROJ_BULLET, 35.0f, 8, 25.0f },
    { "Lance-Flamme",   650,    0.08f, 2,   35,  ORANGE,    PROJ_FLAME,  22.0f, 3, 20.0f },

    // [8-11] Armes Militaires (Rarity 20-30)
    { "Minigun",        1500,   0.06f, 5,   20,  GOLD,      PROJ_BULLET, 35.0f, 1, 12.0f },
    { "Sniper",         7500,   1.8f, 70,   15,  MAROON,    PROJ_BULLET, 150.0f,1, 0.0f },
    { "Bazooka",        3500,   2.5f, 60,   15,  BLACK,     PROJ_GRENADE,28.0f, 1, 1.0f },
    { "Lance-Missiles", 4500,   3.0f, 75,   10,  RED,       PROJ_GRENADE,18.0f, 1, 0.0f },

    // [12-15] Armes Futures (Rarity 10-20)
    { "Plasma Rifle",   2200,   0.25f, 6,    20,  LIME,      PROJ_LASER,  55.0f, 1, 2.0f },
    { "Laser",          2800,   0.1f, 18,   18,  PURPLE,    PROJ_LASER,  0.0f,  1, 0.0f },
    { "Gauss Rifle",    12000,  2.5f, 15,   8,   SKYBLUE,   PROJ_LASER,  0.0f,  1, 0.0f },
    { "Arc Explosif",   2400,   1.2f, 40,   15,  {200,100,50,255}, PROJ_GRENADE, 45.0f, 1, 1.0f },

    // [16-25] Armes Légendaires & Spéciales (Rarity 1-8)
    { "Rayon Orbital",  9999,   5.0f, 150,  2,   RED,       PROJ_LASER,  0.0f,  1, 0.0f },
    { "Tesla Coil",     1500,   1.2f, 4,    10,  SKYBLUE,   PROJ_CHAIN,  0.0f,  1, 0.0f },
    { "Black Hole Gun", 2500,   5.0f, 3,    4,   DARKPURPLE,PROJ_BLACK_HOLE, 15.0f, 1, 0.0f },
    { "Vampire Staff",  1300,   0.3f, 6,    12,  MAROON,    PROJ_VAMPIRE,30.0f, 1, 5.0f },
    { "Cluster Bomb",   1500,   2.5f, 25,   10,  ORANGE,    PROJ_CLUSTER,20.0f, 1, 5.0f },
    { "Canon Sonique",  1000,   0.8f, 15,   15,  WHITE,     PROJ_SONIC,  40.0f, 5, 30.0f },
    { "Meteor",         4000,   5.0f, 58,   5,   ORANGE,    PROJ_GRENADE,60.0f, 1, 5.0f },
    { "Biolab Gun",     1300,   0.08f,5,    15,  LIME,      PROJ_BULLET, 30.0f, 1, 8.0f },
    { "NUKE",           5000,   8.0f, 85,   1,   GRAY,      PROJ_NUKE,   15.0f, 1, 0.0f },
    { "God Hand",       5000,   0.5f, 35,   3,   GOLD,      PROJ_LASER,  0.0f,  1, 0.0f },
    { "Mjolnir",        2500,   3.5f, 22,   3,   {180,200,255,255}, PROJ_CHAIN, 0.0f, 1, 0.0f },
    { "Dragon Fire",    900,    1.5f, 18,   8,   ORANGE,    PROJ_FLAME,  20.0f, 5, 35.0f },
    { "Ice Shard",      1800,   0.12f,12,   12,  SKYBLUE,   PROJ_BULLET, 65.0f, 1, 2.0f },
    { "Shuriken Storm", 1200,   0.3f, 15,   15,  GRAY,      PROJ_BULLET, 40.0f, 3, 30.0f }
};

inline constexpr float LOOT_SPAWN_INTERVAL = 8.0f; 
inline constexpr int   LOOT_SPAWN_LIMIT = 25;
inline constexpr float PROJECTILE_RADIUS = 30.0f; 
inline constexpr float AOE_RADIUS_STD = 150.0f;
inline constexpr float AOE_RADIUS_BIG = 350.0f;
inline constexpr float AOE_RADIUS_NUKE = 800.0f; 
inline constexpr float AOE_RADIUS_BLACKHOLE = 600.0f;

inline constexpr Color CLR_ITEM_HEAL = { 46, 204, 113, 255 };
inline constexpr Color CLR_ITEM_ARMOR = { 52, 152, 219, 255 };
inline constexpr Color CLR_ITEM_PISTOL = { 236, 240, 241, 255 };
inline constexpr Color CLR_PROJECTILE = { 255, 255, 0, 255 };

#endif