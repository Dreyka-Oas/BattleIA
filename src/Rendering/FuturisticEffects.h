// Effets visuels futuristes pour la sélection d'IA - Version compatible raylib
#ifndef FUTURISTIC_EFFECTS_H
#define FUTURISTIC_EFFECTS_H

#include "raylib.h"
#include <math.h>

namespace FuturisticEffects {
    
    // Couleurs néon pour la sélection
    const Color SELECTION_NEON_BLUE = { 0, 200, 255, 255 };
    const Color SELECTION_NEON_PURPLE = { 150, 0, 255, 255 };
    const Color SELECTION_GLOW_CYAN = { 0, 255, 255, 200 };
    const Color SELECTION_HIGHLIGHT = { 100, 255, 255, 150 };
    
    // Paramètres
    const float SELECTION_GLOW_THICKNESS = 3.0f;
    const float SELECTION_PULSE_SPEED = 2.5f;
    const unsigned char SELECTION_GLOW_ALPHA = 180;
    
    // Effet de pulsation néon
    inline Color GetPulsingNeonColor(float time) {
        float pulse = (sinf(time * SELECTION_PULSE_SPEED) + 1.0f) * 0.5f;
        return Color{
            (unsigned char)(100 + pulse * 100),
            (unsigned char)(200 + pulse * 55),
            255,
            (unsigned char)(200 + pulse * 55)
        };
    }
    
    // Effet de contour glow (compatible raylib)
    inline void DrawSelectionGlow(Rectangle rect, float thickness, Color baseColor) {
        // Dessiner plusieurs couches pour l'effet glow
        for(int i = 3; i > 0; i--) {
            Color glowColor = baseColor;
            glowColor.a = (unsigned char)(baseColor.a * (0.3f * i));
            
            Rectangle glowRect = {
                rect.x - i * 0.5f,
                rect.y - i * 0.5f,
                rect.width + i,
                rect.height + i
            };
            
            DrawRectangleLinesEx(glowRect, thickness + i, glowColor);
        }
        
        // Contour principal
        DrawRectangleLinesEx(rect, thickness, baseColor);
    }
    
    // Effet de sélection holographique
    inline void DrawHoloSelection(Vector2 position, float radius, float time) {
        // Cercle intérieur
        DrawCircleV(position, radius, Fade(SKYBLUE, 0.3f));
        
        // Anneaux holographiques
        for(int i = 0; i < 3; i++) {
            float ringRadius = radius + sinf(time + i) * 5.0f;
            DrawCircleLines((int)position.x, (int)position.y, ringRadius, Fade(CYAN, 0.5f));
        }
        
        // Points de connexion
        for(int i = 0; i < 8; i++) {
            float angle = (time * 2.0f) + (i * PI / 4);
            Vector2 point = {
                position.x + cosf(angle) * radius,
                position.y + sinf(angle) * radius
            };
            DrawCircleV(point, 3.0f, SKYBLUE);
        }
    }
    
    // Effet de scan futuriste
    inline void DrawFutureScanEffect(Rectangle bounds, float scanProgress) {
        if(scanProgress < 0 || scanProgress > 1) return;
        
        // Ligne de scan
        float scanY = bounds.y + (bounds.height * scanProgress);
        DrawRectangle((int)bounds.x, (int)(scanY - 1), (int)bounds.width, 3, Fade(GREEN, 0.7f));
        
        // Effet de remplissage progressif avec dégradé
        Rectangle fillRect = {
            bounds.x,
            bounds.y,
            bounds.width,
            bounds.height * scanProgress
        };
        
        // Créer un dégradé vertical
        Color topColor = Fade((Color){0, 255, 100, 100}, 0.3f);
        Color bottomColor = Fade((Color){0, 255, 100, 100}, 0.1f);
        
        // Dessiner plusieurs rectangles pour simuler un dégradé (simplifié)
        DrawRectangle((int)fillRect.x, (int)fillRect.y, (int)fillRect.width, (int)fillRect.height, topColor);
    }
    
    // Effet de sélection futuriste pour les entités
    inline void DrawFuturisticEntitySelection(Vector2 position, float radius, bool isSelected, bool isAI, float time) {
        if (!isSelected) return;
        
        if (isAI) {
            // Style spécial pour les IA
            DrawHoloSelection(position, radius * 1.2f, time);
            
            // Contour glow néon
            Rectangle selectionRect = {
                position.x - radius * 1.2f,
                position.y - radius * 1.2f,
                radius * 2.4f,
                radius * 2.4f
            };
            DrawSelectionGlow(selectionRect, SELECTION_GLOW_THICKNESS, GetPulsingNeonColor(time));
        } else {
            // Style normal futuriste
            Rectangle selectionRect = {
                position.x - radius * 1.1f,
                position.y - radius * 1.1f,
                radius * 2.2f,
                radius * 2.2f
            };
            DrawSelectionGlow(selectionRect, SELECTION_GLOW_THICKNESS, SELECTION_NEON_BLUE);
        }
    }
}

#endif // FUTURISTIC_EFFECTS_H
