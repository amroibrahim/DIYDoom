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

    struct SegmentRenderData
    {
		int V1XScreen;
		int V2XScreen;
		Angle V1Angle;
		Angle V2Angle;
        float DistanceToV1;
        float DistanceToNormal;
        float V1ScaleFactor;
        float V2ScaleFactor;
        float Steps;

        float RightSectorCeiling;
        float RightSectorFloor;
        float CeilingStep;
        float CeilingEnd;
        float FloorStep;
        float FloorStart;

        float LeftSectorCeiling;
        float LeftSectorFloor;

        bool bDrawUpperSection;
        bool bDrawLowerSection;

        float UpperHeightStep;
        float iUpperHeight;
        float LowerHeightStep;
        float iLowerHeight;

        bool UpdateFloor;
        bool UpdateCeiling;

		Seg *pSeg;
    };

    void RenderAutoMap();
    void Render3DView();

    void SelectColor(Seg &seg, SDL_Color &color);
    void ClipSolidWalls(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void ClipPassWalls(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void StoreWallRange(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void CalculateWallHeight(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void CeilingFloorUpdate(ViewRenderer::SegmentRenderData &RenderData);
    void CalculateWallHeightSimple(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void CalculateCeilingFloorHeight(Seg &seg, int &VXScreen, float &DistanceToV, float &CeilingVOnScreen, float &FloorVOnScreen);
    void PartialSeg(Seg &seg, Angle &V1Angle, Angle &V2Angle, float &DistanceToV1, bool IsLeftSide);
    void RenderSegment(SegmentRenderData &RenderData);
    void DrawMiddleSection(ViewRenderer::SegmentRenderData &RenderData, int iXCurrent, int CurrentCeilingEnd, int CurrentFloorStart);
    void DrawLowerSection(ViewRenderer::SegmentRenderData &RenderData, int iXCurrent, int CurrentFloorStart);
    void DrawUpperSection(ViewRenderer::SegmentRenderData &RenderData, int iXCurrent, int CurrentCeilingEnd);
    void RenderSolidWall(Seg &seg, int XStart, int XStop);

    bool ValidateRange(ViewRenderer::SegmentRenderData &RenderData, int &iXCurrent, int &CurrentCeilingEnd, int &CurrentFloorStart);

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

    SDL_Color GetWallColor(std::string textureName);

    Map *m_pMap;
    Player *m_pPlayer;

    SDL_Renderer *m_pRenderer;

    std::list<SolidSegmentRange> m_SolidWallRanges;
    std::vector<int> m_FloorClipHeight;
    std::vector<int> m_CeilingClipHeight;
    std::map<std::string, SDL_Color> m_WallColor;
    std::map<int, Angle> m_ScreenXToAngle;
    bool m_UseClassicDoomScreenToAngle;
};

