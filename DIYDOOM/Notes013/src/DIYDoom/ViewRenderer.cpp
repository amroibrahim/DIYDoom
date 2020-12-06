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

    m_SolidWallRanges.clear();

    SolidSegmentRange WallLeftSide;
    SolidSegmentRange WallRightSide;

    WallLeftSide.XStart = INT_MIN;
    WallLeftSide.XEnd = -1;
    m_SolidWallRanges.push_back(WallLeftSide);

    WallRightSide.XStart = m_iRenderXSize;
    WallRightSide.XEnd = INT_MAX;
    m_SolidWallRanges.push_back(WallRightSide);
}

void ViewRenderer::AddWallInFOV(Seg &seg, Angle V1Angle, Angle V2Angle)
{
    // Find Wall X Coordinates 
    int V1XScreen = AngleToScreen(V1Angle);
    int V2XScreen = AngleToScreen(V2Angle);

    // Skip same pixel wall
    if (V1XScreen == V2XScreen)
        return;

    // Handle solid walls
    if (seg.pLeftSector == nullptr)
    {
        ClipSolidWalls(seg, V1XScreen, V2XScreen);
    }
}

void ViewRenderer::ClipSolidWalls(Seg &seg, int V1XScreen, int V2XScreen)
{
    // Find clip window 
    SolidSegmentRange CurrentWall = { V1XScreen, V2XScreen };

    std::list<SolidSegmentRange>::iterator FoundClipWall = m_SolidWallRanges.begin();
    while (FoundClipWall != m_SolidWallRanges.end() && FoundClipWall->XEnd < CurrentWall.XStart - 1)
    {
        ++FoundClipWall;
    }

    if (CurrentWall.XStart < FoundClipWall->XStart)
    {
        if (CurrentWall.XEnd < FoundClipWall->XStart - 1)
        {
            //All of the wall is visible, so insert it
            StoreWallRange(seg, CurrentWall.XStart, CurrentWall.XEnd);
            m_SolidWallRanges.insert(FoundClipWall, CurrentWall);
            return;
        }

        // The end is already included, just update start
        StoreWallRange(seg, CurrentWall.XStart, FoundClipWall->XStart - 1);
        FoundClipWall->XStart = CurrentWall.XStart;
    }

    // This part is already occupied
    if (CurrentWall.XEnd <= FoundClipWall->XEnd)
        return;

    std::list<SolidSegmentRange>::iterator NextWall = FoundClipWall;

    while (CurrentWall.XEnd >= next(NextWall, 1)->XStart - 1)
    {
        // partialy clipped by other walls, store each fragment
        StoreWallRange(seg, NextWall->XEnd + 1, next(NextWall, 1)->XStart - 1);
        ++NextWall;

        if (CurrentWall.XEnd <= NextWall->XEnd)
        {
            FoundClipWall->XEnd = NextWall->XEnd;
            if (NextWall != FoundClipWall)
            {
                FoundClipWall++;
                NextWall++;
                m_SolidWallRanges.erase(FoundClipWall, NextWall);
            }

            return;
        }
    }

    StoreWallRange(seg, NextWall->XEnd + 1, CurrentWall.XEnd);
    FoundClipWall->XEnd = CurrentWall.XEnd;

    if (NextWall != FoundClipWall)
    {
        FoundClipWall++;
        NextWall++;
        m_SolidWallRanges.erase(FoundClipWall, NextWall);
    }
}

void ViewRenderer::StoreWallRange(Seg &seg, int V1XScreen, int V2XScreen)
{
    // For now just we will not store the range, we will just draw it
    SolidSegmentData Wall = { seg, V1XScreen, V2XScreen };
    DrawSolidWall(Wall);
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

void ViewRenderer::DrawSolidWall(SolidSegmentData &visibleSeg)
{
    SDL_Color color = GetWallColor(visibleSeg.seg.pLinedef->pRightSidedef->MiddleTexture);
    SDL_Rect Rect = { visibleSeg.XStart, 0, visibleSeg.XEnd - visibleSeg.XStart + 1, m_iRenderYSize };
    SetDrawColor(color.r, color.g, color.b);
    SDL_RenderFillRect(m_pRenderer, &Rect);
    //SDL_RenderPresent(m_pRenderer);
    //SDL_Delay(1000);
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

SDL_Color ViewRenderer::GetWallColor(std::string textureName)
{
    if (m_WallColor.count(textureName))
    {
        return m_WallColor[textureName];
    }
    else
    {
        SDL_Color color{ rand() % 255, rand() % 255, rand() % 255 };
        m_WallColor[textureName] = color;
        return color;
    }
}