#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include "raylib.h"

// --- Fenêtre ---
inline constexpr int   CFG_INITIAL_WIDTH = 1280;
inline constexpr int   CFG_INITIAL_HEIGHT = 720;
inline constexpr int   CFG_MIN_WIDTH = 900;
inline constexpr int   CFG_MIN_HEIGHT = 600;
inline constexpr const char* CFG_WINDOW_TITLE = "BattleIA - 8000 Entities - Balanced";
inline constexpr int   CFG_TARGET_FPS = 120; 
inline constexpr int   CFG_FULLSCREEN_KEY = KEY_F11;

// --- Map & Camera ---
inline constexpr int   MAP_SIZE = 800; 
inline constexpr float TILE_SIZE = 64.0f;
inline constexpr float MIN_ZOOM = 0.01f;
inline constexpr float MAX_ZOOM = 3.0f;
inline constexpr float CFG_SIDEBAR_RATIO = 0.25f;
inline constexpr float AUTOCAM_MIN_HOLD_TIME = 2.0f;

// --- Entités ---
inline constexpr int   AI_COUNT = 8000; 
inline constexpr float AI_SIZE = 150.0f;
inline constexpr float AI_SPEED = 4.0f;
inline constexpr float CFG_WATER_SPEED_MULT = 0.75f; 
inline constexpr int   AI_BASE_ELO = 100;

// --- Palette UI (VÉRIFIÉE) ---
inline constexpr Color UI_SIDEBAR_BG   = { 255, 255, 255, 255 };
inline constexpr Color UI_MAIN_BG      = { 241, 245, 249, 255 };
inline constexpr Color UI_ACCENT       = { 37, 99, 235, 255 };
inline constexpr Color UI_ACCENT_LIGHT = { 219, 234, 254, 255 };
inline constexpr Color UI_ACCENT_DIM   = { 230, 242, 255, 255 };
inline constexpr Color UI_TEXT_MAIN    = { 15, 23, 42, 255 };
inline constexpr Color UI_TEXT_SUB     = { 100, 116, 139, 255 };
inline constexpr Color UI_TEXT_DIM     = { 148, 163, 184, 255 };
inline constexpr Color UI_BORDER       = { 226, 232, 240, 255 };
inline constexpr Color UI_DEAD_BG      = { 254, 226, 226, 255 };
inline constexpr Color UI_HEALTH_GRN   = { 34, 197, 94, 255 };
inline constexpr Color UI_CARD_BG      = { 255, 255, 255, 255 };

// --- Couleurs Monde ---
inline constexpr Color CLR_DEEP_BLUE   = { 15, 23, 42, 255 };
inline constexpr Color CLR_WATER       = { 56, 189, 248, 255 };
inline constexpr Color CLR_SAND        = { 253, 224, 71, 255 };
inline constexpr Color CLR_GRASS       = { 132, 204, 22, 255 };
inline constexpr Color CLR_FOREST      = { 21, 128, 61, 255 };
inline constexpr Color CLR_MOUNTAIN    = { 71, 85, 105, 255 };
inline constexpr Color CLR_PATH        = { 234, 179, 8, 255 };
inline constexpr Color CLR_AI_BODY     = { 239, 68, 68, 255 };
inline constexpr Color CLR_AI_SELECT   = { 37, 99, 235, 255 };
inline constexpr Color CLR_AI_TARGET   = { 255, 255, 255, 255 };
inline constexpr Color CLR_AI_OUTLINE  = { 255, 255, 255, 150 };
inline constexpr Color CLR_HP_BG       = { 226, 232, 240, 200 };
inline constexpr Color CLR_ZONE_BORDER = { 255, 0, 0, 200 };

#endif