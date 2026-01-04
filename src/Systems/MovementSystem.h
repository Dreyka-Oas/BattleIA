#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "raylib.h"
#include "World/Map.h"
class MovementSystem { public: static Vector2 AvoidObstacles(Vector2 pos, Vector2 target, Map& map); };
#endif
