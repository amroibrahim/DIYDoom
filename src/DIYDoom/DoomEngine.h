#pragma once

#include <SDL.h>
#include <string>

#include "Map.h"
#include "Player.h"
#include "WADLoader.h"
#include "ViewRenderer.h"

class DoomEngine
{
public:
    DoomEngine(SDL_Renderer *pRenderer);
    ~DoomEngine();

    virtual void Render();
    virtual void KeyPressed(SDL_Event &event);
    virtual void KeyReleased(SDL_Event &event);
    virtual void Quit();
    virtual void Update();

    virtual bool IsOver();
    virtual bool Init();

    virtual int GetRenderWidth();
    virtual int GetRenderHeight();
    virtual int GetTimePerFrame();

    virtual std::string GetWADFileName();
    virtual std::string GetName();

protected:
    int m_iRenderWidth;
    int m_iRenderHeight;

    bool m_bIsOver;
    bool m_bRenderAutoMap;

    SDL_Renderer *m_pRenderer;
    WADLoader m_WADLoader;
    Map *m_pMap;
    Player *m_pPlayer;
    Things *m_pThings;
    ViewRenderer *m_pViewRenderer;
};
