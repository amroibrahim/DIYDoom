#include "ViewRenderer.h"

#include <math.h>
#include <iostream>

#include "Map.h"
#include "Player.h"

ViewRenderer::ViewRenderer(SDL_Renderer *pRenderer) : m_pRenderer(pRenderer), m_iAutoMapScaleFactor(15)
{
}

ViewRenderer::~ViewRenderer()
{
}

void  ViewRenderer::Init(Map *pMap, Player *pPlayer)
{
    m_pMap = pMap;
    m_pPlayer = pPlayer;

    SDL_RenderGetLogicalSize(m_pRenderer, &m_iRenderXSize, &m_iRenderYSize);
    --m_iRenderXSize;
    --m_iRenderYSize;
}

int ViewRenderer::RemapXToScreen(int XMapPosition)
{
    return (XMapPosition + (-m_pMap->GetXMin())) / m_iAutoMapScaleFactor;
}

int ViewRenderer::RemapYToScreen(int YMapPosition)
{
    return m_iRenderYSize - (YMapPosition + (-m_pMap->GetYMin())) / m_iAutoMapScaleFactor;
}

void ViewRenderer::Render(bool IsRenderAutoMap)
{
    if (IsRenderAutoMap)
    {
        RenderAutoMap();
    }
    else
    {
        Render3DView();
    }

    SDL_RenderPresent(m_pRenderer);
}

void ViewRenderer::InitFrame()
{
    SetDrawColor(0, 0, 0);
    SDL_RenderClear(m_pRenderer);
}

void ViewRenderer::AddWallInFOV(Seg &seg, Angle V1Angle, Angle V2Angle)
{
    // Solid walls don't have a Left side.
    if (seg.pLeftSector == nullptr)
    {
        AddSolidWall(seg, V1Angle, V2Angle);
    }
}

void ViewRenderer::AddSolidWall(Seg &seg, Angle V1Angle, Angle V2Angle)
{
    int V1XScreen = AngleToScreen(V1Angle);
    int V2XScreen = AngleToScreen(V2Angle);

    SDL_RenderDrawLine(m_pRenderer, V1XScreen, 0, V1XScreen, m_iRenderYSize);
    SDL_RenderDrawLine(m_pRenderer, V2XScreen, 0, V2XScreen, m_iRenderYSize);
}

void ViewRenderer::RenderAutoMap()
{
    m_pMap->RenderAutoMap();
    m_pPlayer->RenderAutoMap();
}

void ViewRenderer::Render3DView()
{
    m_pMap->Render3DView();
}

int ViewRenderer::AngleToScreen(Angle angle)
{
    Angle tempAngle = angle;
    int iX = 0;

    if (angle > 90)
    {
        angle -= 90;
        iX = 160 - round(tanf(angle.GetValue() * PI / 180.0f) * 160);
    }
    else
    {
        angle = 90 - angle.GetValue();
        float f = tanf(angle.GetValue());
        iX = round(tanf(angle.GetValue() * PI / 180.0f) * 160);
        iX += 160;
    }

    return iX;
}

void ViewRenderer::SetDrawColor(int R, int G, int B)
{
    SDL_SetRenderDrawColor(m_pRenderer, R, G, B, SDL_ALPHA_OPAQUE);
}

void ViewRenderer::DrawRect(int X1, int Y1, int X2, int Y2)
{
    SDL_Rect Rect = {
        RemapXToScreen(X1),
        RemapYToScreen(Y1),
        RemapXToScreen(X2) - RemapXToScreen(X1) + 1,
        RemapYToScreen(Y2) - RemapYToScreen(Y1) + 1 };

    SDL_RenderDrawRect(m_pRenderer, &Rect);
}

void ViewRenderer::DrawLine(int X1, int Y1, int X2, int Y2)
{
    SDL_RenderDrawLine(m_pRenderer,
        RemapXToScreen(X1),
        RemapYToScreen(Y1),
        RemapXToScreen(X2),
        RemapYToScreen(Y2));
}