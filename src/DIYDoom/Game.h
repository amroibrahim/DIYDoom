#pragma once

#include <SDL.h>
#include <memory>

#include "DisplayManager.h"
#include "DoomEngine.h"

class Game
{
public:
    Game();
    virtual ~Game();

    void ProcessInput();
    void Render();
    void Update();
    void Delay();

    bool IsOver();
    bool Init();

protected:
    std::unique_ptr<DoomEngine> m_pDoomEngine;
};
