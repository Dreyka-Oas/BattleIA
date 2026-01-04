#ifndef ENTITY_RENDERER_H
#define ENTITY_RENDERER_H

#include "../Entities/Entity.h"
#include <vector>

class EntityRenderer {
public:
    static void DrawAll(const std::vector<Entity>& entities, Camera2D camera, int sw, int sh, int selID);
};

#endif