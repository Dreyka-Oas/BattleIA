// FICHIER DE DIAGNOSTIC - Pour trouver les vrais noms dans Entity
// Compilez ce fichier et regardez les erreurs

#include "Entities/Entity.h"
#include <iostream>

void DiagnoseEntityStructure() {
    Entity entity;
    
    // Essayer d'accéder à différents noms possibles
    // Décommentez une ligne à la fois pour tester
    
    // 1. Position
    // auto pos = entity.position;  // Essayez: position, pos, m_position, Position, m_Pos
    
    // 2. Taille/Radius
    // auto size = entity.radius;   // Essayez: radius, size, m_radius, Radius, m_Size
    
    // 3. Couleur
    // auto col = entity.color;     // Essayez: color, m_color, Color, m_Color, col
    
    // 4. Sélection
    // auto sel = entity.isSelected; // Essayez: isSelected, selected, m_selected, bSelected
    
    // 5. IA
    // auto ai = entity.isAI;       // Essayez: isAI, ai, m_ai, bAI, isAi
}
