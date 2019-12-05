#include "Game.h"

#include <iostream>
#include <string>

Game::Game()
{
}

Game::~Game()
{

}

bool Game::Init()
{
    m_pDoomEngine = std::unique_ptr<DoomEngine>(new DoomEngine());

    if (!m_pDoomEngine || !m_pDoomEngine->Init())
    {
        std::cout << m_pDoomEngine->GetAppName() << " failed to initialize!" << std::endl;
        return false;
    }
    return true;
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            m_pDoomEngine->KeyPressed(event);
            break;

        case SDL_KEYUP:
            m_pDoomEngine->KeyReleased(event);
            break;

        case SDL_QUIT:
            m_pDoomEngine->Quit();
            break;
        }
    }

    m_pDoomEngine->UpdateKeyStatus(SDL_GetKeyboardState(NULL));
}

void Game::Render()
{
    m_pDoomEngine->Render();
}

void Game::Update()
{
    m_pDoomEngine->Update();
}

bool Game::IsOver()
{
    return m_pDoomEngine->IsOver();
}

void Game::Delay()
{
    SDL_Delay(m_pDoomEngine->GetTimePerFrame());
}
