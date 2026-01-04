#ifndef ZONE_MANAGER_H
#define ZONE_MANAGER_H

#include "raylib.h"

class ZoneManager {
public:
    void Init();
    void Update(float dt);
    void Draw(); // Ajout de la déclaration manquante

    // Membres publics pour accès direct par les systèmes (comme dans l'ancien code)
    Vector2 center;
    Vector2 targetCenter;
    float radius;
    float targetRadius;
    
    float timer;
    bool isShrinking;
    bool isMoving;
    float moveTimer;
    
    float initialRadius;
    float minRadius;
    
    // Getters de compatibilité
    Vector2 GetCenter() const { return center; }
    float GetRadius() const { return radius; }
};

#endif