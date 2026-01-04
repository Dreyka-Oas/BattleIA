#ifndef ELO_CONFIG_H
#define ELO_CONFIG_H

// --- Configuration de la Zone ---
inline constexpr float ZONE_SHRINK_INTERVAL = 15.0f;
inline constexpr float ZONE_SHRINK_DURATION = 5.0f;
inline constexpr float ZONE_SHRINK_PERCENT = 0.15f;
inline constexpr float ZONE_MIN_SIZE_PERCENT = 0.03f; 
inline constexpr float ZONE_MOVE_SPEED = 200.0f; 
inline constexpr float ZONE_DAMAGE_PER_SEC = 5.0f; 
inline constexpr float ZONE_ELO_LOSS_PER_SEC = 10.0f;

// --- Gains ELO ---
inline constexpr float ELO_GAIN_KILL = 50.0f;
inline constexpr float ELO_GAIN_HIT = 5.0f;
inline constexpr float ELO_GAIN_LOOT = 15.0f;
inline constexpr float ELO_GAIN_DODGE = 1.0f;
inline constexpr float ELO_GAIN_STATIONARY = 1.0f;
inline constexpr float ELO_GAIN_SURVIVAL_SEC = 2.0f;

// --- Pertes ELO (RESTAURÉ) ---
inline constexpr float ELO_LOSS_HIT = 5.0f; 

// --- Facteurs de Mort ---
inline constexpr float ELO_DEATH_FACTOR_UNARMED = 0.5f;
inline constexpr float ELO_DEATH_FACTOR_ARMED = 0.6f;

inline constexpr float STATIONARY_THRESHOLD = 3.0f;
inline constexpr float PROJ_DODGE_RADIUS = 60.0f;

#endif