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
    m_pThings = new Things();

    m_pMap = new Map(m_pViewRenderer, "E1M1", m_pPlayer, m_pThings);

    m_pViewRenderer->Init(m_pMap, m_pPlayer);

    m_WADLoader.SetWADFilePath(GetWADFileName());
    m_WADLoader.LoadWAD();
    m_WADLoader.LoadMapData(m_pMap);

    Thing thing = (m_pMap->GetThings())->GetThingByID(m_pPlayer->GetID());

    m_pPlayer->Init(thing);
    m_pMap->Init();

    return true;
}

std::string DoomEngine::GetWADFileName()
{
    return "..\\..\\..\\external\\assets\\DOOM.WAD";
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
        //m_pPlayer->MoveForward();
        break;

    case SDLK_DOWN:
        //m_pPlayer->MoveBackward();
        break;

    case SDLK_LEFT:
        //m_pPlayer->RotateLeft();
        break;

    case SDLK_RIGHT:
        //m_pPlayer->RotateRight();
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

void DoomEngine::UpdateKeyStatus(const Uint8* KeyStates)
{
    if (KeyStates[SDL_SCANCODE_UP])
    {
        m_pPlayer->MoveForward();
    }

    if (KeyStates[SDL_SCANCODE_DOWN])
    {
        m_pPlayer->MoveBackward();
    }

    if (KeyStates[SDL_SCANCODE_LEFT])
    {
        m_pPlayer->RotateLeft();
    }

    if (KeyStates[SDL_SCANCODE_RIGHT])
    {
        m_pPlayer->RotateRight();
    }

    if (KeyStates[SDL_SCANCODE_Z])
    {
        m_pPlayer->Fly();
    }

    if (KeyStates[SDL_SCANCODE_X])
    {
        m_pPlayer->Sink();
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
    m_pPlayer->Think(m_pMap->GetPlayerSubSectorHieght());
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
