#pragma once

#include <SDL.h>
#include <string>
#include <memory>

#include "Map.h"
#include "Player.h"
#include "WADLoader.h"
#include "DisplayManager.h"
#include "ViewRenderer.h"
#include "AssetsManager.h"

class DoomEngine
{
public:
    DoomEngine();
    ~DoomEngine();

    virtual void Render();
    virtual void KeyPressed(SDL_Event &event);
    virtual void UpdateKeyStatus(const Uint8* KeyStates);
    virtual void KeyReleased(SDL_Event &event);
    virtual void Quit();
    virtual void Update();

    virtual bool IsOver();
    virtual bool Init();

    virtual int GetRenderWidth();
    virtual int GetRenderHeight();
    virtual int GetTimePerFrame();

    virtual std::string GetWADFileName();
    virtual std::string GetAppName();

protected:
    void LoadWAD();
    void ReadDataFromWAD();

    int m_iRenderWidth;
    int m_iRenderHeight;

    bool m_bIsOver;

    std::string m_sAppName;
    WADLoader m_WADLoader;

    std::unique_ptr<Map> m_pMap;
    std::unique_ptr<Player> m_pPlayer;
    std::unique_ptr<Things> m_pThings;
    std::unique_ptr<DisplayManager> m_pDisplayManager;
    std::unique_ptr<ViewRenderer> m_pViewRenderer;
};
