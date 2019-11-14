#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include <SDL.h>

#include "DataTypes.h"
#include "Angle.h"

class Map;
class Player;

class ViewRenderer
{
public:
    ViewRenderer(SDL_Renderer *pRenderer);
    ~ViewRenderer();

    void Init(Map *pMap, Player *pPlayer);
    void Render(bool IsAutomap);
    void AddWallInFOV(Seg &seg, Angle V1Angle, Angle V2Angle, Angle V1AngleFromPlayer, Angle V2AngleFromPlayer);
    void InitFrame();
    void SetDrawColor(int R, int G, int B);
    void DrawRect(int X1, int Y1, int X2, int Y2);
    void DrawLine(int X1, int Y1, int X2, int Y2);

protected:
    struct SolidSegmentRange
    {
        int XStart;
        int XEnd;
    };


    struct FrameRenderData
    {
        float DistanceToV1;
        float DistanceToNormal;
        float V1ScaleFactor;
        float V2ScaleFactor;
        float Steps;

        float FrontSectorCeiling;
        float FrontSectorFloor;
        float CeilingStep;
        float CeilingEnd;
        float FloorStep;
        float FloorStart;

        float BackSectorCeiling;
        float BackSectorFloor;

        bool bDrawUpperSection;
        bool bDrawLowerSection;

        float UpperHeightStep;
        float iUpperHeight;
        float LowerHeightStep;
        float iLowerHeight;

        bool UpdateFloor;
        bool UpdateCeiling;
    };

    struct SingleDrawLine
    {
        int x1;
        int y1;
        int x2;
        int y2;
    };

    struct FrameSegDrawData
    {
        Seg *seg;

        bool bDrawUpperSection;
        bool bDrawLowerSection;
        bool bDrawMiddleSection;

        std::list<SingleDrawLine> UpperSection;
        std::list<SingleDrawLine> LowerSection;
        std::list<SingleDrawLine> MiddleSection;
    };

    void RenderAutoMap();
    void Render3DView();
    void DrawStoredSegs();

    void DrawSection(std::list<ViewRenderer::SingleDrawLine> &Section);

    void ClipSolidWalls(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void ClipPassWalls(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void StoreWallRange(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void CalculateWallHeight(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void CeilingFloorUpdate(ViewRenderer::FrameRenderData &RenderData, Seg & seg);
    void CalculateWallHeightSimple(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void CalculateCeilingFloorHeight(Seg &seg, int &VXScreen, float &DistanceToV, float &CeilingVOnScreen, float &FloorVOnScreen);
    void PartialSeg(Seg &seg, Angle &V1Angle, Angle &V2Angle, float &DistanceToV1, bool IsLeftSide);
    void RenderSegment(Seg &seg, int V1XScreen, int V2XScreen, FrameRenderData &RenderData);
    void RenderUpperSection(ViewRenderer::FrameRenderData &RenderData, int iXCurrent, int CurrentCeilingEnd, FrameSegDrawData &SegDrawData);
    void RenderMiddleSection(int iXCurrent, int CurrentCeilingEnd, int CurrentFloorStart, FrameSegDrawData &SegDrawData);
    void AddLineToSection(std::list<SingleDrawLine> &Section, int iXCurrent, int CurrentCeilingEnd, int CurrentFloorStart);
    void RenderLowerSection(ViewRenderer::FrameRenderData &RenderData, int iXCurrent, int CurrentFloorStart, FrameSegDrawData &SegDrawData);
    void SetSectionColor(std::string textureName);

    bool ValidateRange(ViewRenderer::FrameRenderData &RenderData, int &iXCurrent, int &CurrentCeilingEnd, int &CurrentFloorStart);

    float GetScaleFactor(int VXScreen, Angle NormalAngle, float NormalDistance);

    int AngleToScreen(Angle angle);
    int RemapXToScreen(int XMapPosition);
    int RemapYToScreen(int YMapPosition);

    int m_iRenderXSize;
    int m_iRenderYSize;
    int m_iAutoMapScaleFactor;
    int m_iDistancePlayerToScreen;
    int m_HalfScreenWidth;
    int m_HalfScreenHeight;

    Map *m_pMap;
    Player *m_pPlayer;

    SDL_Renderer *m_pRenderer;

    std::list<SolidSegmentRange> m_SolidWallRanges;
    std::list<FrameSegDrawData> m_FrameSegsDrawData;
    std::vector<int> m_FloorClipHeight;
    std::vector<int> m_CeilingClipHeight;
    std::map<std::string, SDL_Color> m_WallColor;
    std::map<int, Angle> m_ScreenXToAngle;
    bool m_UseClassicDoomScreenToAngle;
};

