#ifndef PROJECTILE_RENDERER_H
#define PROJECTILE_RENDERER_H

#include "../Entities/Projectile.h"
#include <vector>

class ProjectileRenderer {
public:
    static void Draw(const std::vector<Projectile>& projs, 
                     const std::vector<Explosion>& exps,
                     const std::vector<LightningBolt>& bolts);
};

#endif