#include "LootSystem.h"
#include "../Managers/ItemManager.h"
#include "../Entities/Entity.h"
#include "raymath.h"
#include <cmath>

bool LootSystem::CheckPickup(Entity& ent, ItemManager& im) {
    int idx = im.GetNearestItemIndex(ent.position);
    if (idx != -1) {
        Vector2 itemPos = im.GetItemPosition(idx);
        
        // Distance de ramassage
        if (Vector2Distance(ent.position, itemPos) < 40.0f) {
             ItemType type = im.PickUpItem(idx);
             if (type != ITEM_NONE) {
                 if (type == ITEM_HEAL) { 
                     ent.health = fminf(100, ent.health + 30); 
                     return true; 
                 }
                 if (type == ITEM_ARMOR) { 
                     ent.armor = fminf(100, ent.armor + 50); 
                     return true; 
                 }
                 
                 // --- LOGIQUE D'ÉCHANGE D'ARME ---
                 if (type >= ITEM_PISTOL) {
                     // Si l'IA a déjà une arme, elle la lâche au sol
                     if (ent.itemHeld != ITEM_NONE) {
                         im.DropItem(ent.position, ent.itemHeld);
                     }
                     // Elle prend la nouvelle
                     ent.itemHeld = type;
                     return true;
                 }
             }
        }
    }
    return false;
}