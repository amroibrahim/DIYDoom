#include "DoomEngine.h"

using namespace std;

DoomEngine::DoomEngine() : m_sAppName("DIYDoom"), m_bIsOver(false), m_iRenderWidth(320), m_iRenderHeight(200), m_pMap(nullptr), m_pPlayer(nullptr)
{
}

DoomEngine::~DoomEngine()
{
}

bool DoomEngine::Init()
{
    // Load WAD 
    LoadWAD();
    AssetsManager::GetInstance()->Init(&m_WADLoader);

    m_pDisplayManager = std::unique_ptr < DisplayManager>(new DisplayManager(m_iRenderWidth, m_iRenderHeight));
    m_pDisplayManager->Init(GetAppName());

    // Delay object creation to this point so renderer is inistilized correctly
    m_pViewRenderer = unique_ptr<ViewRenderer> (new ViewRenderer());
    m_pThings = unique_ptr<Things>(new Things());
    m_pPlayer = unique_ptr<Player>(new Player(m_pViewRenderer.get(), 1));
    m_pMap = unique_ptr<Map>(new Map(m_pViewRenderer.get(), "E1M1", m_pPlayer.get(), m_pThings.get()));
   
    ReadDataFromWAD();

    m_pViewRenderer->Init(m_pMap.get(), m_pPlayer.get());
    m_pPlayer->Init((m_pMap->GetThings())->GetThingByID(m_pPlayer->GetID()));
    m_pMap->Init();

    return true;
}

void DoomEngine::LoadWAD()
{
    m_WADLoader.SetWADFilePath(GetWADFileName());
    m_WADLoader.LoadWADToMemory();
}

std::string DoomEngine::GetWADFileName()
{
    return "..\\..\\..\\external\\assets\\DOOM.WAD";
}

void DoomEngine::Render()
{
    uint8_t *pScreenBuffer = m_pDisplayManager->GetScreenBuffer();

    m_pDisplayManager->InitFrame();
    m_pViewRenderer->Render(pScreenBuffer, m_iRenderWidth);
    m_pPlayer->Render(pScreenBuffer, m_iRenderWidth);

    m_pDisplayManager->Render();
}

void DoomEngine::KeyPressed(SDL_Event &event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_TAB:
        //m_bRenderAutoMap = true;
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
        m_pPlayer->MoveLeftward();
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
        //m_bRenderAutoMap = false;
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

string DoomEngine::GetAppName()
{
    return m_sAppName;
}

void DoomEngine::ReadDataFromWAD()
{
    m_WADLoader.LoadPalette(m_pDisplayManager.get());
    m_WADLoader.LoadMapData(m_pMap.get());
}

int DoomEngine::GetTimePerFrame()
{
    return 1000 / 60;
}
