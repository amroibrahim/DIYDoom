#pragma once

#include <SDL.h>
#include <string>

#include "Map.h"
#include "WADLoader.h"

class DoomEngine
{
public:
    DoomEngine();
    ~DoomEngine();

    virtual void Render(SDL_Renderer *pRenderer);
    virtual void KeyPressed(SDL_Event &event);
    virtual void KeyReleased(SDL_Event &event);
    virtual void Quit();
    virtual void Update();

    virtual bool IsOver();
    virtual bool Init();

    virtual int GetRenderWidth();
    virtual int GetRenderHeight();
    virtual int GetTimePerFrame();

    virtual std::string GetName();
    virtual std::string GetWADFileName();

protected:
    int m_iRenderWidth;
    int m_iRenderHeight;

    bool m_bIsOver;

    WADLoader m_WADLoader;
    Map *m_pMap;
};
