#ifndef ELO_SYSTEM_H
#define ELO_SYSTEM_H
#include "Entities/Entity.h"

class EloSystem {
public:
    static void AwardSurvival(Entity& ent, float dt);
    static void AwardKill(Entity& killer, Entity& victim);
    static void AwardDamage(Entity& attacker, float damage);
    static void AwardLoot(Entity& ent);
    static void AwardDodge(Entity& ent);
    static void PenalizeZone(Entity& ent, float dt);
};
#endif
