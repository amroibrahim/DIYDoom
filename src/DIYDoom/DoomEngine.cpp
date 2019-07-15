#include "DoomEngine.h"

using namespace std;

DoomEngine::DoomEngine(SDL_Renderer *pRenderer) : m_pRenderer(pRenderer), m_bIsOver(false), m_iRenderWidth(320), m_iRenderHeight(200)
{
}

DoomEngine::~DoomEngine()
{
    delete m_pMap;
}

bool DoomEngine::Init()
{
    // Delay object creation to this point so renderer is inistilized correctly
    m_pPlayer = new Player(1);
    m_pMap = new Map(m_pRenderer, "E1M1", m_pPlayer);

    m_WADLoader.SetWADFilePath(GetWADFileName());
    m_WADLoader.LoadWAD();

    m_WADLoader.LoadMapData(m_pMap);
    return true;
}

std::string DoomEngine::GetWADFileName()
{
    return "D:\\SDKs\\Assets\\Doom\\DOOM.WAD";
}

void DoomEngine::Render()
{
    SDL_SetRenderDrawColor(m_pRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(m_pRenderer);
    m_pMap->RenderAutoMap();
}

void DoomEngine::KeyPressed(SDL_Event &event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_UP:
        break;

    case SDLK_DOWN:
        break;

    case SDLK_LEFT:
        break;

    case SDLK_RIGHT:
        break;

    case SDLK_ESCAPE:
        Quit();
        break;

    default:
        break;
    }
}

void DoomEngine::KeyReleased(SDL_Event &event)
{
}

void  DoomEngine::Quit()
{
    m_bIsOver = true;
}

void DoomEngine::Update()
{

}

bool DoomEngine::IsOver()
{
    return m_bIsOver;
}

int DoomEngine::GetRenderWidth()
{
    return m_iRenderWidth;
}

int DoomEngine::GetRenderHeight()
{
    return m_iRenderHeight;
}

string DoomEngine::GetName()
{
    return "DIYDoom";
}

int DoomEngine::GetTimePerFrame()
{
    return 1000 / 60;
}
