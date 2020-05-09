#include "ViewRenderer.h"

#include <math.h>
#include <iostream>

#include "Map.h"
#include "Player.h"
#include "ClassicDefs.h"

ViewRenderer::ViewRenderer(SDL_Renderer *pRenderer) : m_pRenderer(pRenderer), m_iAutoMapScaleFactor(15), m_UseClassicDoomScreenToAngle(true)
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

    m_HalfScreenWidth = m_iRenderXSize / 2;
    m_HalfScreenHeight = m_iRenderYSize / 2;
    Angle HalfFOV = m_pPlayer->GetFOV() / 2;
    m_iDistancePlayerToScreen = m_HalfScreenWidth / HalfFOV.GetTanValue();

    for (int i = 0; i <= m_iRenderXSize; ++i)
    {
        if (m_UseClassicDoomScreenToAngle)
        {
            m_ScreenXToAngle[i] = classicDoomScreenXtoView[i];
        }
        else
        {
            m_ScreenXToAngle[i] = atan((m_HalfScreenWidth - i) / (float)m_iDistancePlayerToScreen) * 180 / PI;
        }
    }

    m_CeilingClipHeight.resize(m_iRenderXSize);
    m_FloorClipHeight.resize(m_iRenderXSize);
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

    std::fill(m_CeilingClipHeight.begin(), m_CeilingClipHeight.end(), -1);
    std::fill(m_FloorClipHeight.begin(), m_FloorClipHeight.end(), m_iRenderYSize);
}

void ViewRenderer::AddWallInFOV(Seg &seg, Angle V1Angle, Angle V2Angle, Angle V1AngleFromPlayer, Angle V2AngleFromPlayer)
{
    // Find Wall X Coordinates 
    int V1XScreen = AngleToScreen(V1AngleFromPlayer);
    int V2XScreen = AngleToScreen(V2AngleFromPlayer);

    // Skip same pixel wall
    if (V1XScreen == V2XScreen)
        return;

    // Handle solid walls
    if (seg.pLeftSector == nullptr)
    {
        ClipSolidWalls(seg, V1XScreen, V2XScreen, V1Angle, V2Angle);
        return;
    }

    // Handle closed door
    if (seg.pLeftSector->CeilingHeight <= seg.pRightSector->FloorHeight
        || seg.pLeftSector->FloorHeight >= seg.pRightSector->CeilingHeight)
    {
        ClipSolidWalls(seg, V1XScreen, V2XScreen, V1Angle, V2Angle);
        return;
    }

    // Windowed walls
    if (seg.pRightSector->CeilingHeight != seg.pLeftSector->CeilingHeight ||
        seg.pRightSector->FloorHeight != seg.pLeftSector->FloorHeight)
    {
        ClipPassWalls(seg, V1XScreen, V2XScreen, V1Angle, V2Angle);
        return;
    }
}

void ViewRenderer::ClipSolidWalls(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle)
{
    if (m_SolidWallRanges.size() < 2)
    {
        return;
    }
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
            StoreWallRange(seg, CurrentWall.XStart, CurrentWall.XEnd, V1Angle, V2Angle);
            m_SolidWallRanges.insert(FoundClipWall, CurrentWall);
            return;
        }

        // The end is already included, just update start
        StoreWallRange(seg, CurrentWall.XStart, FoundClipWall->XStart - 1, V1Angle, V2Angle);
        FoundClipWall->XStart = CurrentWall.XStart;
    }

    // This part is already occupied
    if (CurrentWall.XEnd <= FoundClipWall->XEnd)
        return;

    std::list<SolidSegmentRange>::iterator NextWall = FoundClipWall;

    while (CurrentWall.XEnd >= next(NextWall, 1)->XStart - 1)
    {
        // partialy clipped by other walls, store each fragment
        StoreWallRange(seg, NextWall->XEnd + 1, next(NextWall, 1)->XStart - 1, V1Angle, V2Angle);
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

    StoreWallRange(seg, NextWall->XEnd + 1, CurrentWall.XEnd, V1Angle, V2Angle);
    FoundClipWall->XEnd = CurrentWall.XEnd;

    if (NextWall != FoundClipWall)
    {
        FoundClipWall++;
        NextWall++;
        m_SolidWallRanges.erase(FoundClipWall, NextWall);
    }
}

void ViewRenderer::ClipPassWalls(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle)
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
            StoreWallRange(seg, CurrentWall.XStart, CurrentWall.XEnd, V1Angle, V2Angle);
            return;
        }

        // The end is already included, just update start
        StoreWallRange(seg, CurrentWall.XStart, FoundClipWall->XStart - 1, V1Angle, V2Angle);
    }

    // This part is already occupied
    if (CurrentWall.XEnd <= FoundClipWall->XEnd)
        return;

    std::list<SolidSegmentRange>::iterator NextWall = FoundClipWall;

    while (CurrentWall.XEnd >= next(NextWall, 1)->XStart - 1)
    {
        // partialy clipped by other walls, store each fragment
        StoreWallRange(seg, NextWall->XEnd + 1, next(NextWall, 1)->XStart - 1, V1Angle, V2Angle);
        ++NextWall;

        if (CurrentWall.XEnd <= NextWall->XEnd)
        {
            return;
        }
    }

    StoreWallRange(seg, NextWall->XEnd + 1, CurrentWall.XEnd, V1Angle, V2Angle);
}

void ViewRenderer::StoreWallRange(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle)
{
    // For now just lets sotre the range 
    CalculateWallHeight(seg, V1XScreen, V2XScreen, V1Angle, V2Angle);
    // CalculateWallHeightSimple(seg, V1XScreen, V2XScreen, V1Angle, V2Angle);
}

void ViewRenderer::CalculateWallHeight(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle)
{
    // Calculate the distance to the first edge of the wall
    Angle Angle90(90);
    FrameRenderData RenderData{ 0 };
    Angle SegToNormalAngle = seg.SlopeAngle + Angle90;

    //Angle NomalToV1Angle = abs(SegToNormalAngle.GetSignedValue() - V1Angle.GetSignedValue());
    Angle NomalToV1Angle = SegToNormalAngle.GetValue() - V1Angle.GetValue();

    // Normal angle is 90 degree to wall
    Angle SegToPlayerAngle = Angle90 - NomalToV1Angle;

    RenderData.DistanceToV1 = m_pPlayer->DistanceToPoint(*seg.pStartVertex);
    RenderData.DistanceToNormal = SegToPlayerAngle.GetSinValue() * RenderData.DistanceToV1;

    RenderData.V1ScaleFactor = GetScaleFactor(V1XScreen, SegToNormalAngle, RenderData.DistanceToNormal);
    RenderData.V2ScaleFactor = GetScaleFactor(V2XScreen, SegToNormalAngle, RenderData.DistanceToNormal);

    RenderData.Steps = (RenderData.V2ScaleFactor - RenderData.V1ScaleFactor) / (V2XScreen - V1XScreen);

    RenderData.RightSectorCeiling = seg.pRightSector->CeilingHeight - m_pPlayer->GetZPosition();
    RenderData.RightSectorFloor = seg.pRightSector->FloorHeight - m_pPlayer->GetZPosition();

    RenderData.CeilingStep = -(RenderData.RightSectorCeiling * RenderData.Steps);
    RenderData.CeilingEnd = round(m_HalfScreenHeight - (RenderData.RightSectorCeiling * RenderData.V1ScaleFactor));

    RenderData.FloorStep = -(RenderData.RightSectorFloor * RenderData.Steps);
    RenderData.FloorStart = round(m_HalfScreenHeight - (RenderData.RightSectorFloor * RenderData.V1ScaleFactor));

    if (seg.pLeftSector)
    {
        RenderData.LeftSectorCeiling = seg.pLeftSector->CeilingHeight - m_pPlayer->GetZPosition();
        RenderData.LeftSectorFloor = seg.pLeftSector->FloorHeight - m_pPlayer->GetZPosition();

        CeilingFloorUpdate(RenderData, seg);

        if (RenderData.LeftSectorCeiling < RenderData.RightSectorCeiling)
        {
            RenderData.bDrawUpperSection = true;
            RenderData.UpperHeightStep = -(RenderData.LeftSectorCeiling * RenderData.Steps);
            RenderData.iUpperHeight = round(m_HalfScreenHeight - (RenderData.LeftSectorCeiling * RenderData.V1ScaleFactor));
        }

        if (RenderData.LeftSectorFloor > RenderData.RightSectorFloor)
        {
            RenderData.bDrawLowerSection = true;
            RenderData.LowerHeightStep = -(RenderData.LeftSectorFloor * RenderData.Steps);
            RenderData.iLowerHeight = round(m_HalfScreenHeight - (RenderData.LeftSectorFloor * RenderData.V1ScaleFactor));
        }
    }

    RenderSegment(seg, V1XScreen, V2XScreen, RenderData);
}

void ViewRenderer::CeilingFloorUpdate(ViewRenderer::FrameRenderData &RenderData, Seg & seg)
{
    if (!seg.pLeftSector)
    {
        RenderData.UpdateFloor = true;
        RenderData.UpdateCeiling = true;
        return;
    }

    if (RenderData.LeftSectorCeiling != RenderData.RightSectorCeiling)
    {
        RenderData.UpdateCeiling = true;
    }
    else
    {
        RenderData.UpdateCeiling = false;
    }

    if (RenderData.LeftSectorFloor != RenderData.RightSectorFloor)
    {
        RenderData.UpdateFloor = true;
    }
    else
    {
        RenderData.UpdateFloor = false;
    }

    if (seg.pLeftSector->CeilingHeight <= seg.pRightSector->FloorHeight || seg.pLeftSector->FloorHeight >= seg.pRightSector->CeilingHeight)
    {
        // closed door
        RenderData.UpdateCeiling = RenderData.UpdateFloor = true;
    }

    if (seg.pRightSector->CeilingHeight <= m_pPlayer->GetZPosition())
    {
        // below view plane
        RenderData.UpdateCeiling = false;
    }

    if (seg.pRightSector->FloorHeight >= m_pPlayer->GetZPosition())
    {
        // above view plane
        RenderData.UpdateFloor = false;
    }
}

float ViewRenderer::GetScaleFactor(int VXScreen, Angle SegToNormalAngle, float DistanceToNormal)
{
    static float MAX_SCALEFACTOR = 64.0f;
    static float MIN_SCALEFACTOR = 0.00390625f;

    Angle Angle90(90);

    Angle ScreenXAngle = m_ScreenXToAngle[VXScreen];
    Angle SkewAngle = m_ScreenXToAngle[VXScreen] + m_pPlayer->GetAngle() - SegToNormalAngle;

    float ScreenXAngleCos = ScreenXAngle.GetCosValue();
    float SkewAngleCos = SkewAngle.GetCosValue();
    float ScaleFactor = (m_iDistancePlayerToScreen * SkewAngleCos) / (DistanceToNormal * ScreenXAngleCos);

    if (ScaleFactor > MAX_SCALEFACTOR)
    {
        ScaleFactor = MAX_SCALEFACTOR;
    }
    else if (MIN_SCALEFACTOR > ScaleFactor)
    {
        ScaleFactor = MIN_SCALEFACTOR;
    }

    return ScaleFactor;
}

void ViewRenderer::CalculateWallHeightSimple(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle)
{
    // We have V1 and V2, do calculations for V1 and V2 sepratly then interpolate values in between
    float DistanceToV1 = m_pPlayer->DistanceToPoint(*seg.pStartVertex);
    float DistanceToV2 = m_pPlayer->DistanceToPoint(*seg.pEndVertex);

    // Special Case partial seg on the left
    if (V1XScreen <= 0)
    {
        PartialSeg(seg, V1Angle, V2Angle, DistanceToV1, true);
    }

    // Special Case partial seg on the right
    if (V2XScreen >= 319)
    {
        PartialSeg(seg, V1Angle, V2Angle, DistanceToV2, false);
    }

    float CeilingV1OnScreen;
    float FloorV1OnScreen;
    float CeilingV2OnScreen;
    float FloorV2OnScreen;

    CalculateCeilingFloorHeight(seg, V1XScreen, DistanceToV1, CeilingV1OnScreen, FloorV1OnScreen);
    CalculateCeilingFloorHeight(seg, V2XScreen, DistanceToV2, CeilingV2OnScreen, FloorV2OnScreen);

    //SDL_Color color = { 255,255,255 };
    SDL_Color color = GetWallColor(seg.pLinedef->pRightSidedef->MiddleTexture);
    SetDrawColor(color.r, color.g, color.b);

    SDL_RenderDrawLine(m_pRenderer, V1XScreen, CeilingV1OnScreen, V1XScreen, FloorV1OnScreen);
    SDL_RenderDrawLine(m_pRenderer, V2XScreen, CeilingV2OnScreen, V2XScreen, FloorV2OnScreen);
    SDL_RenderDrawLine(m_pRenderer, V1XScreen, CeilingV1OnScreen, V2XScreen, CeilingV2OnScreen);
    SDL_RenderDrawLine(m_pRenderer, V1XScreen, FloorV1OnScreen, V2XScreen, FloorV2OnScreen);
}

void ViewRenderer::CalculateCeilingFloorHeight(Seg &seg, int &VXScreen, float &DistanceToV, float &CeilingVOnScreen, float &FloorVOnScreen)
{
    float Ceiling = seg.pRightSector->CeilingHeight - m_pPlayer->GetZPosition();
    float Floor = seg.pRightSector->FloorHeight - m_pPlayer->GetZPosition();

    Angle VScreenAngle = m_ScreenXToAngle[VXScreen];

    float DistanceToVScreen = m_iDistancePlayerToScreen / VScreenAngle.GetCosValue();

    CeilingVOnScreen = (abs(Ceiling) * DistanceToVScreen) / DistanceToV;
    FloorVOnScreen = (abs(Floor) * DistanceToVScreen) / DistanceToV;

    if (Ceiling > 0)
    {
        CeilingVOnScreen = m_HalfScreenHeight - CeilingVOnScreen;
    }
    else
    {
        CeilingVOnScreen += m_HalfScreenHeight;
    }

    if (Floor > 0)
    {
        FloorVOnScreen = m_HalfScreenHeight - FloorVOnScreen;
    }
    else
    {
        FloorVOnScreen += m_HalfScreenHeight;
    }
}

void ViewRenderer::PartialSeg(Seg &seg, Angle &V1Angle, Angle &V2Angle, float &DistanceToV, bool IsLeftSide)
{
    float SideC = sqrt(pow(seg.pStartVertex->XPosition - seg.pEndVertex->XPosition, 2) + pow(seg.pStartVertex->YPosition - seg.pEndVertex->YPosition, 2));
    Angle V1toV2Span = V1Angle - V2Angle;
    float SINEAngleB = DistanceToV * V1toV2Span.GetSinValue() / SideC;
    Angle AngleB(asinf(SINEAngleB) * 180.0 / PI);
    Angle AngleA(180 - V1toV2Span.GetValue() - AngleB.GetValue());

    Angle AngleVToFOV;
    if (IsLeftSide)
    {
        AngleVToFOV = V1Angle - (m_pPlayer->GetAngle() + 45);
    }
    else
    {
        AngleVToFOV = (m_pPlayer->GetAngle() - 45) - V2Angle;
    }

    Angle NewAngleB(180 - AngleVToFOV.GetValue() - AngleA.GetValue());
    DistanceToV = DistanceToV * AngleA.GetSinValue() / NewAngleB.GetSinValue();
}

void ViewRenderer::RenderSegment(Seg &seg, int V1XScreen, int V2XScreen, FrameRenderData &RenderData)
{
    SDL_Color color;
    int iXCurrent = V1XScreen;

    SelectColor(seg, color);

    while (iXCurrent <= V2XScreen)
    {
        int CurrentCeilingEnd = RenderData.CeilingEnd;
        int CurrentFloorStart = RenderData.FloorStart;

        if (!ValidateRange(RenderData, iXCurrent, CurrentCeilingEnd, CurrentFloorStart))
        {
            continue;
        }

        if (seg.pLeftSector)
        {
            DrawUpperSection(RenderData, iXCurrent, CurrentCeilingEnd);
            DrawLowerSection(RenderData, iXCurrent, CurrentFloorStart);
        }
        else
        {
            DrawMiddleSection(iXCurrent, CurrentCeilingEnd, CurrentFloorStart);
        }

        RenderData.CeilingEnd += RenderData.CeilingStep;
        RenderData.FloorStart += RenderData.FloorStep;
        ++iXCurrent;
    }
}

void ViewRenderer::SelectColor(Seg &seg, SDL_Color &color)
{
    if (seg.pLeftSector)
    {
        color = GetWallColor(seg.pLinedef->pRightSidedef->UpperTexture);
        SetDrawColor(color.r, color.g, color.b);
    }
    else
    {
        color = GetWallColor(seg.pLinedef->pRightSidedef->MiddleTexture);
        SetDrawColor(color.r, color.g, color.b);
    }
}

void ViewRenderer::DrawMiddleSection(int iXCurrent, int CurrentCeilingEnd, int CurrentFloorStart)
{
    SDL_RenderDrawLine(m_pRenderer, iXCurrent, CurrentCeilingEnd, iXCurrent, CurrentFloorStart);
    m_CeilingClipHeight[iXCurrent] = m_iRenderYSize;
    m_FloorClipHeight[iXCurrent] = -1;
}

void ViewRenderer::DrawLowerSection(ViewRenderer::FrameRenderData &RenderData, int iXCurrent, int CurrentFloorStart)
{
    if (RenderData.bDrawLowerSection)
    {
        int iLowerHeight = RenderData.iLowerHeight;
        RenderData.iLowerHeight += RenderData.LowerHeightStep;

        if (iLowerHeight <= m_CeilingClipHeight[iXCurrent])
        {
            iLowerHeight = m_CeilingClipHeight[iXCurrent] + 1;
        }

        if (iLowerHeight <= CurrentFloorStart)
        {
            SDL_RenderDrawLine(m_pRenderer, iXCurrent, iLowerHeight, iXCurrent, CurrentFloorStart);
            m_FloorClipHeight[iXCurrent] = iLowerHeight;
        }
        else
            m_FloorClipHeight[iXCurrent] = CurrentFloorStart + 1;
    }
    else if (RenderData.UpdateFloor)
    {
        m_FloorClipHeight[iXCurrent] = CurrentFloorStart + 1;
    }
}

void ViewRenderer::DrawUpperSection(ViewRenderer::FrameRenderData &RenderData, int iXCurrent, int CurrentCeilingEnd)
{
    if (RenderData.bDrawUpperSection)
    {
        int iUpperHeight = RenderData.iUpperHeight;
        RenderData.iUpperHeight += RenderData.UpperHeightStep;

        if (iUpperHeight >= m_FloorClipHeight[iXCurrent])
        {
            iUpperHeight = m_FloorClipHeight[iXCurrent] - 1;
        }

        if (iUpperHeight >= CurrentCeilingEnd)
        {
            SDL_RenderDrawLine(m_pRenderer, iXCurrent, CurrentCeilingEnd, iXCurrent, iUpperHeight);
            m_CeilingClipHeight[iXCurrent] = iUpperHeight;
        }
        else
            m_CeilingClipHeight[iXCurrent] = CurrentCeilingEnd - 1;
    }
    else if (RenderData.UpdateCeiling)
    {
        m_CeilingClipHeight[iXCurrent] = CurrentCeilingEnd - 1;
    }
}

bool ViewRenderer::ValidateRange(ViewRenderer::FrameRenderData & RenderData, int &iXCurrent, int &CurrentCeilingEnd, int &CurrentFloorStart)
{
    if (CurrentCeilingEnd < m_CeilingClipHeight[iXCurrent] + 1)
    {
        CurrentCeilingEnd = m_CeilingClipHeight[iXCurrent] + 1;
    }

    if (CurrentFloorStart >= m_FloorClipHeight[iXCurrent])
    {
        CurrentFloorStart = m_FloorClipHeight[iXCurrent] - 1;
    }

    if (CurrentCeilingEnd > CurrentFloorStart)
    {
        RenderData.CeilingEnd += RenderData.CeilingStep;
        RenderData.FloorStart += RenderData.FloorStep;
        ++iXCurrent;
        return false;
    }
    return true;
}

void ViewRenderer::RenderSolidWall(Seg &seg, int XStart, int XStop)
{
    int iXCurrent = XStart;
    SDL_Color color = GetWallColor(seg.pLinedef->pRightSidedef->MiddleTexture);
    SetDrawColor(color.r, color.g, color.b);
    while (iXCurrent <= XStop)
    {
        SDL_RenderDrawLine(m_pRenderer, iXCurrent, 10, iXCurrent, 20);
        ++iXCurrent;
    }
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
    int iX = 0;

    if (angle > 90)
    {
        angle -= 90;
        iX = m_iDistancePlayerToScreen - round(angle.GetTanValue() * m_HalfScreenWidth);
    }
    else
    {
        angle = 90 - angle.GetValue();
        iX = round(angle.GetTanValue() * m_HalfScreenWidth);
        iX += m_iDistancePlayerToScreen;
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
    int val1 = RemapXToScreen(X1);
    int val2 = RemapYToScreen(Y1);
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
