#include "ViewRenderer.h"

#include <math.h>
#include <iostream>

#include "Map.h"
#include "Player.h"
#include "ClassicDefs.h"
#include "AssetsManager.h"

ViewRenderer::ViewRenderer() : m_UseClassicDoomScreenToAngle(true)
{
}

ViewRenderer::~ViewRenderer()
{
}

void  ViewRenderer::Init(Map *pMap, Player *pPlayer)
{
    m_pMap = pMap;
    m_pPlayer = pPlayer;

    m_iRenderXSize = 320;
    m_iRenderYSize = 200;

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

void ViewRenderer::Render(uint8_t *pScreenBuffer, int iBufferPitch)
{
    InitFrame();
    Render3DView();
    DrawStoredSegs(pScreenBuffer, iBufferPitch);

    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("AASTINKY");
    Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BROWN1");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BROWNPIP");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BROWN144");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BIGDOOR1");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BIGDOOR2");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BIGDOOR4");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("COMP2");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BRNSMAL1");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BRNBIGC");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BRNPOIS");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("BRNPOIS2");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("EXITDOOR");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("SKY1");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("TEKWALL5");
    //Texture *pTexture = AssetsManager::GetInstance()->GetTexture("SW1DIRT");
	pTexture->Render(pScreenBuffer, iBufferPitch, 10, 10);
       
}

void ViewRenderer::InitFrame()
{
    m_SolidWallRanges.clear();
    m_FrameSegsDrawData.clear();

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
    if (seg.pBackSector == nullptr)
    {
        ClipSolidWalls(seg, V1XScreen, V2XScreen, V1Angle, V2Angle);
        return;
    }

    // Handle closed door
    if (seg.pBackSector->CeilingHeight <= seg.pFrontSector->FloorHeight
        || seg.pBackSector->FloorHeight >= seg.pFrontSector->CeilingHeight)
    {
        ClipSolidWalls(seg, V1XScreen, V2XScreen, V1Angle, V2Angle);
        return;
    }

    // Windowed walls
    if (seg.pFrontSector->CeilingHeight != seg.pBackSector->CeilingHeight ||
        seg.pFrontSector->FloorHeight != seg.pBackSector->FloorHeight)
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

    RenderData.FrontSectorCeiling = seg.pFrontSector->CeilingHeight - m_pPlayer->GetZPosition();
    RenderData.FrontSectorFloor = seg.pFrontSector->FloorHeight - m_pPlayer->GetZPosition();

    RenderData.CeilingStep = -(RenderData.FrontSectorCeiling * RenderData.Steps);
    RenderData.CeilingEnd = round(m_HalfScreenHeight - (RenderData.FrontSectorCeiling * RenderData.V1ScaleFactor));

    RenderData.FloorStep = -(RenderData.FrontSectorFloor * RenderData.Steps);
    RenderData.FloorStart = round(m_HalfScreenHeight - (RenderData.FrontSectorFloor * RenderData.V1ScaleFactor));

    if (seg.pBackSector)
    {
        RenderData.BackSectorCeiling = seg.pBackSector->CeilingHeight - m_pPlayer->GetZPosition();
        RenderData.BackSectorFloor = seg.pBackSector->FloorHeight - m_pPlayer->GetZPosition();

        CeilingFloorUpdate(RenderData, seg);

        if (RenderData.BackSectorCeiling < RenderData.FrontSectorCeiling)
        {
            RenderData.bDrawUpperSection = true;
            RenderData.UpperHeightStep = -(RenderData.BackSectorCeiling * RenderData.Steps);
            RenderData.iUpperHeight = round(m_HalfScreenHeight - (RenderData.BackSectorCeiling * RenderData.V1ScaleFactor));
        }

        if (RenderData.BackSectorFloor > RenderData.FrontSectorFloor)
        {
            RenderData.bDrawLowerSection = true;
            RenderData.LowerHeightStep = -(RenderData.BackSectorFloor * RenderData.Steps);
            RenderData.iLowerHeight = round(m_HalfScreenHeight - (RenderData.BackSectorFloor * RenderData.V1ScaleFactor));
        }
    }

    RenderSegment(seg, V1XScreen, V2XScreen, RenderData);
}

void ViewRenderer::CeilingFloorUpdate(ViewRenderer::FrameRenderData &RenderData, Seg & seg)
{
    if (!seg.pBackSector)
    {
        RenderData.UpdateFloor = true;
        RenderData.UpdateCeiling = true;
        return;
    }

    if (RenderData.BackSectorCeiling != RenderData.FrontSectorCeiling)
    {
        RenderData.UpdateCeiling = true;
    }
    else
    {
        RenderData.UpdateCeiling = false;
    }

    if (RenderData.BackSectorFloor != RenderData.FrontSectorFloor)
    {
        RenderData.UpdateFloor = true;
    }
    else
    {
        RenderData.UpdateFloor = false;
    }

    if (seg.pBackSector->CeilingHeight <= seg.pFrontSector->FloorHeight || seg.pBackSector->FloorHeight >= seg.pFrontSector->CeilingHeight)
    {
        // closed door
        RenderData.UpdateCeiling = RenderData.UpdateFloor = true;
    }

    if (seg.pFrontSector->CeilingHeight <= m_pPlayer->GetZPosition())
    {
        // below view plane
        RenderData.UpdateCeiling = false;
    }

    if (seg.pFrontSector->FloorHeight >= m_pPlayer->GetZPosition())
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

void ViewRenderer::CalculateCeilingFloorHeight(Seg &seg, int &VXScreen, float &DistanceToV, float &CeilingVOnScreen, float &FloorVOnScreen)
{
    float Ceiling = seg.pFrontSector->CeilingHeight - m_pPlayer->GetZPosition();
    float Floor = seg.pFrontSector->FloorHeight - m_pPlayer->GetZPosition();

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
    FrameSegDrawData SegDrawData;
    SegDrawData.seg = &seg;
    SegDrawData.bDrawUpperSection = RenderData.bDrawUpperSection;
    SegDrawData.bDrawMiddleSection = !seg.pBackSector;
    SegDrawData.bDrawLowerSection = RenderData.bDrawLowerSection;

    int iXCurrent = V1XScreen;

    while (iXCurrent <= V2XScreen)
    {
        int CurrentCeilingEnd = RenderData.CeilingEnd;
        int CurrentFloorStart = RenderData.FloorStart;

        if (!ValidateRange(RenderData, iXCurrent, CurrentCeilingEnd, CurrentFloorStart))
        {
            continue;
        }

        if (seg.pBackSector)
        {
            RenderUpperSection(RenderData, iXCurrent, CurrentCeilingEnd, SegDrawData);
            RenderLowerSection(RenderData, iXCurrent, CurrentFloorStart, SegDrawData);
        }
        else
        {
            RenderMiddleSection(iXCurrent, CurrentCeilingEnd, CurrentFloorStart, SegDrawData);
        }

        RenderData.CeilingEnd += RenderData.CeilingStep;
        RenderData.FloorStart += RenderData.FloorStep;
        ++iXCurrent;
    }

    if (SegDrawData.bDrawUpperSection | SegDrawData.bDrawMiddleSection | SegDrawData.bDrawLowerSection)
    {
        m_FrameSegsDrawData.push_back(SegDrawData);
    }
}

void ViewRenderer::RenderMiddleSection(int iXCurrent, int CurrentCeilingEnd, int CurrentFloorStart, FrameSegDrawData &SegDrawData)
{
    AddLineToSection(SegDrawData.MiddleSection, iXCurrent, CurrentCeilingEnd, CurrentFloorStart);
    m_CeilingClipHeight[iXCurrent] = m_iRenderYSize;
    m_FloorClipHeight[iXCurrent] = -1;
}

void ViewRenderer::AddLineToSection(std::list<SingleDrawLine> &Section, int iXCurrent, int CurrentCeilingEnd, int CurrentFloorStart)
{
    SingleDrawLine line;
    line.x1 = iXCurrent;
    line.y1 = CurrentCeilingEnd;
    line.x2 = iXCurrent;
    line.y2 = CurrentFloorStart;
    Section.push_back(line);
}

void ViewRenderer::RenderLowerSection(ViewRenderer::FrameRenderData &RenderData, int iXCurrent, int CurrentFloorStart, FrameSegDrawData &SegDrawData)
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
            AddLineToSection(SegDrawData.LowerSection, iXCurrent, iLowerHeight, CurrentFloorStart);
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

void ViewRenderer::RenderUpperSection(ViewRenderer::FrameRenderData &RenderData, int iXCurrent, int CurrentCeilingEnd, FrameSegDrawData &SegDrawData)
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
            AddLineToSection(SegDrawData.UpperSection, iXCurrent, CurrentCeilingEnd, iUpperHeight);
            m_CeilingClipHeight[iXCurrent] = iUpperHeight;
        }
        else
        {
            m_CeilingClipHeight[iXCurrent] = CurrentCeilingEnd - 1;
        }

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

uint8_t ViewRenderer::GetSectionColor(const std::string &TextureName)
{
    uint8_t color;
    if (m_WallColor.count(TextureName))
    {
        color = m_WallColor[TextureName];
    }
    else
    {
        color = rand() % 256;
        m_WallColor[TextureName] = color;
    };

    return color;
}

void ViewRenderer::DrawStoredSegs(uint8_t *pScreenBuffer, int iBufferPitch)
{
    for (std::list<FrameSegDrawData>::iterator SegDrawData = m_FrameSegsDrawData.begin(); SegDrawData != m_FrameSegsDrawData.end(); ++SegDrawData)
    {
        if (SegDrawData->bDrawUpperSection)
        {
            DrawSection(pScreenBuffer, iBufferPitch, SegDrawData->UpperSection, GetSectionColor(SegDrawData->seg->pLinedef->pFrontSidedef->UpperTexture));
        }

        if (SegDrawData->bDrawMiddleSection)
        {
            DrawSection(pScreenBuffer, iBufferPitch, SegDrawData->MiddleSection, GetSectionColor(SegDrawData->seg->pLinedef->pFrontSidedef->MiddleTexture));
        }

        if (SegDrawData->bDrawLowerSection)
        {
            DrawSection(pScreenBuffer, iBufferPitch, SegDrawData->LowerSection, GetSectionColor(SegDrawData->seg->pLinedef->pFrontSidedef->LowerTexture));
        }
    }
}

void ViewRenderer::DrawSection(uint8_t *pScreenBuffer, int iBufferPitch, std::list<ViewRenderer::SingleDrawLine> &Section, uint8_t color)
{
    for (std::list<SingleDrawLine>::iterator line = Section.begin(); line != Section.end(); ++line)
    {
        int iStartY = line->y1;
        while (iStartY < line->y2)
        {
            pScreenBuffer[iBufferPitch * iStartY + line->x1] = color;
            ++iStartY;
        }
    }
}
