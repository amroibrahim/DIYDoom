#include "DoomEngine.h"

using namespace std;

DoomEngine::DoomEngine(SDL_Renderer *pRenderer) : m_pRenderer(pRenderer), m_bIsOver(false), m_iRenderWidth(320), m_iRenderHeight(200), m_pMap(nullptr), m_bRenderAutoMap(false)
{
}

DoomEngine::~DoomEngine()
{
    delete m_pMap;
    delete m_pPlayer;
    delete m_pViewRenderer;
}

bool DoomEngine::Init()
{
    // Delay object creation to this point so renderer is inistilized correctly
    m_pViewRenderer = new ViewRenderer(m_pRenderer);

    m_pPlayer = new Player(m_pViewRenderer, 1);
    m_pMap = new Map(m_pViewRenderer, "E1M1", m_pPlayer);

    m_pViewRenderer->Init(m_pMap, m_pPlayer);

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
    m_pViewRenderer->InitFrame();
    m_pViewRenderer->Render(m_bRenderAutoMap);
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
        m_pPlayer->RotateLeft();
        break;

    case SDLK_RIGHT:
        m_pPlayer->RotateRight();
        break;

    case SDLK_TAB:
        m_bRenderAutoMap = true;
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
    switch (event.key.keysym.sym)
    {
    case SDLK_TAB:
        m_bRenderAutoMap = false;
        break;

    default:
        break;
    }
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
