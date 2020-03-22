#include <iostream>

#include "Game.h"

int main(int argc, char* argv[])
{
    Game game;
    game.Init();

    while (!game.IsOver())
    {
        game.ProcessInput();
        game.Update();
        game.Render();
        game.Delay();
    }

    return 0;
}
