# 📦 INSTALLATION DE FLECS v4.1.4

## TÉLÉCHARGEMENT (2 MÉTHODES)

### Méthode 1: Téléchargement Direct (RECOMMANDÉ) ⭐
1. Aller sur: https://github.com/SanderMertens/flecs/releases/tag/v4.1.4
2. Télécharger **"Source code (zip)"**
3. Extraire et copier les fichiers suivants dans `O:\Others\BattleIA\external\flecs\`:
   - `flecs.h` (header principal)
   - `flecs.c` (implémentation)

### Méthode 2: Git Clone
```bash
cd O:\Others\BattleIA\external\flecs
git clone --depth 1 --branch v4.1.4 https://github.com/SanderMertens/flecs.git .
```

## STRUCTURE FINALE
```
external/flecs/
├── flecs.h       <- Header C++ API
├── flecs.c       <- Implémentation C
└── INSTALLATION.md
```

## INTÉGRATION DANS LE PROJET

### 1. Ajouter au vcxproj
Dans `BattleIA.vcxproj`, ajouter:
```xml
<ItemGroup>
  <ClCompile Include="external\flecs\flecs.c" />
</ItemGroup>
<ItemGroup>
  <ClInclude Include="external\flecs\flecs.h" />
</ItemGroup>
```

### 2. Configurer les includes
Dans les propriétés du projet (Visual Studio):
- **C/C++** → **General** → **Additional Include Directories**: Ajouter `$(ProjectDir)external\flecs`

### 3. Compiler flecs.c en C (IMPORTANT!)
Dans les propriétés de `flecs.c`:
- Clic droit sur `flecs.c` → **Properties**
- **C/C++** → **Advanced** → **Compile As**: Sélectionner **"Compile as C Code (/TC)"**

## VÉRIFICATION
Une fois installé, testez en incluant:
```cpp
#include <flecs.h>

flecs::world ecs;
// Devrait compiler sans erreur
```

## ACTIVER L'ÉDITEUR WEB 🎮
Pour activer le dashboard Flecs (inspecteur d'entités en temps réel):
```cpp
#define FLECS_REST
#define FLECS_HTTP
#include <flecs.h>

flecs::world ecs;
ecs.import<flecs::rest>();  // Active l'API REST
ecs.app().enable_rest().run();

// Ouvrir dans le navigateur: http://localhost:27750
```

## PROCHAINES ÉTAPES
Une fois flecs installé, exécutez les fichiers dans `src/Flecs/` pour intégrer l'ECS !
