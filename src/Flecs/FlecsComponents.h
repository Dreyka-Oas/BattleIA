// ============================================================================
// FLECS COMPONENTS - BattleIA ECS Architecture
// ============================================================================
// Composants data-oriented optimisés pour Flecs
// Cache-friendly, POD purs, séparés par responsabilité

#ifndef FLECS_COMPONENTS_H
#define FLECS_COMPONENTS_H

#include "raylib.h"
#include "../Config/ItemConfig.h"
#include <string>

// ============================================================================
// COMPOSANTS DE BASE
// ============================================================================

// Position et mouvement physique
struct Position {
    Vector2 pos;
    Vector2 lastPos;  // Pour collision/mouvement
};

// Cible de déplacement
struct Target {
    Vector2 destination;
};

// Vélocité (pour mouvement physique)
struct Velocity {
    float speed;
    Vector2 direction;
};

// ============================================================================
// COMPOSANTS DE SANTÉ & ÉTAT
// ============================================================================

// Points de vie et armure
struct Health {
    float current;
    float maximum;
    float armor;
};

// Statut poison
struct Poisoned {
    float timer;
    flecs::entity source;  // L'entité qui a empoisonné
};

// Tag pour entités actives (alive)
struct Active {};

// Tag pour entités mortes (à nettoyer)
struct Dead {};

// ============================================================================
// COMPOSANTS D'IDENTITÉ & STATISTIQUES
// ============================================================================

// Nom de l'entité
struct Name {
    std::string value;
};

// Statistiques ELO et performance
struct Stats {
    float elo;
    float eloStart;  // ELO au début du round
    int kills;
};

// ============================================================================
// COMPOSANTS D'IA & COMPORTEMENT
// ============================================================================

// Cerveau de l'IA (personnalité)
struct Brain {
    float aggression;   // 0.0 - 1.0 : Tendance à attaquer
    float teamSpirit;   // 0.0 - 1.0 : Coopération (unused pour l'instant)
    float greed;        // 0.0 - 1.0 : Priorité au loot
    float precision;    // 0.0 - 1.0 : Précision de tir
};

// Timers de scan et décision
struct AITimers {
    float scanTimer;       // Timer pour re-scan de l'environnement
    float lootTimer;       // Cooldown pour ramassage
    float stationaryTimer; // Temps immobile (pour détection)
};

// ============================================================================
// COMPOSANTS D'ÉQUIPEMENT & COMBAT
// ============================================================================

// Arme équipée
struct Weapon {
    ItemType type;
    float cooldown;
};

// Tag pour entités armées (optimisation queries)
struct Armed {};

// ============================================================================
// RELATIONS FLECS (LE SUPER POUVOIR!)
// ============================================================================

// Relation: Attaque une cible
struct Attacking {};

// Relation: En équipe avec
struct TeamedWith {};

// Relation: En fuite de
struct FleeingFrom {};

// ============================================================================
// COMPOSANTS SPÉCIAUX POUR OPTIMISATION
// ============================================================================

// Grille spatiale (pour partitioning spatial)
struct GridCell {
    int x, y;
};

// Cache pour nearest enemy (évite recalcul)
struct NearestEnemyCache {
    flecs::entity target;
    float lastUpdateTime;
    float cacheValidDuration = 0.5f;  // Recalculer toutes les 0.5s
};

// ============================================================================
// COMPOSANTS D'ÉVÉNEMENTS (pour observers)
// ============================================================================

// Événement: a été touché par un projectile
struct HitByProjectile {
    flecs::entity attacker;
    float damage;
    ItemType weaponType;
};

// Événement: a ramassé un item
struct PickedUpItem {
    ItemType item;
};

#endif
