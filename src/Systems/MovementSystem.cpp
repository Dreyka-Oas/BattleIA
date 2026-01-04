#include "Systems/MovementSystem.h"
#include "raymath.h"

Vector2 MovementSystem::AvoidObstacles(Vector2 pos, Vector2 target, Map& map) {
    Vector2 dir = Vector2Normalize(Vector2Subtract(target, pos));
    float checkDists[] = { 40.0f, 80.0f, 120.0f };
    bool blocked = false;

    for (float d : checkDists) {
        Vector2 cp = Vector2Add(pos, Vector2Scale(dir, d));
        if (!map.IsWalkable(cp.x, cp.y)) { blocked = true; break; }
    }

    if (!blocked) return dir;

    float angles[] = { 45.0f, -45.0f, 90.0f, -90.0f, 180.0f };
    for (float ang : angles) {
        float rad = ang * DEG2RAD;
        Vector2 newDir = { dir.x * cosf(rad) - dir.y * sinf(rad), dir.x * sinf(rad) + dir.y * cosf(rad) };
        bool pathClear = true;
        for (float d : checkDists) {
            Vector2 cp = Vector2Add(pos, Vector2Scale(newDir, d));
            if (!map.IsWalkable(cp.x, cp.y)) { pathClear = false; break; }
        }
        if (pathClear) return newDir;
    }
    return Vector2Negate(dir);
}
