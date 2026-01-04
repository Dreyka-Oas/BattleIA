#ifndef APPLICATION_H
#define APPLICATION_H
#include "Game.h"

class Application {
public:
    void Run() {
        game.Init();
        while (!WindowShouldClose()) {
            game.Update();
            game.Draw();
        }
        game.Close();
    }
private:
    Game game;
};
#endif