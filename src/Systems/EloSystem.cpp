#include "EloSystem.h"
#include "../Config/EloConfig.h"
#include <algorithm>
#include <cmath>

void EloSystem::AwardSurvival(Entity& ent, float dt) {
    if (ent.active) {
        // Utilisation du nouveau nom standardisé
        ent.elo += ELO_GAIN_SURVIVAL_SEC * dt;
    }
}

void EloSystem::AwardKill(Entity& killer, Entity& victim) {
    killer.elo += ELO_GAIN_KILL;
    
    // Utilisation du facteur par défaut pour la pénalité générique
    victim.elo *= ELO_DEATH_FACTOR_UNARMED;
}

void EloSystem::AwardDamage(Entity& attacker, float damage) {
    // Gain proportionnel aux dégâts infligés
    attacker.elo += (damage / 100.0f) * ELO_GAIN_HIT;
}

void EloSystem::AwardLoot(Entity& ent) {
    ent.elo += ELO_GAIN_LOOT;
}

void EloSystem::AwardDodge(Entity& ent) {
    ent.elo += ELO_GAIN_DODGE;
}

void EloSystem::PenalizeZone(Entity& ent, float dt) {
    // Utilisation de fmaxf pour float
    ent.elo = fmaxf(0.0f, ent.elo - (ZONE_ELO_LOSS_PER_SEC * dt));
}