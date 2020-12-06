#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include <SDL.h>

#include "DataTypes.h"
#include "Angle.h"

class Map;
class Player;

class ViewRenderer
{
public:
    ViewRenderer();
    ~ViewRenderer();

    void Init(Map *pMap, Player *pPlayer);
    void Render(uint8_t *pScreenBuffer, int iBufferPitch);
    void AddWallInFOV(Seg &seg, Angle V1Angle, Angle V2Angle, Angle V1AngleFromPlayer, Angle V2AngleFromPlayer);
    void InitFrame();

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

    void Render3DView();
	void DrawVerticalLine(int iX, int iStartY, int iEndY, uint8_t color);

    void ClipSolidWalls(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void ClipPassWalls(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void StoreWallRange(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void CalculateWallHeight(Seg &seg, int V1XScreen, int V2XScreen, Angle V1Angle, Angle V2Angle);
    void CeilingFloorUpdate(ViewRenderer::SegmentRenderData &RenderData);
    void CalculateCeilingFloorHeight(Seg &seg, int &VXScreen, float &DistanceToV, float &CeilingVOnScreen, float &FloorVOnScreen);
    void PartialSeg(Seg &seg, Angle &V1Angle, Angle &V2Angle, float &DistanceToV1, bool IsLeftSide);
    void RenderSegment(SegmentRenderData &RenderData);
    void DrawMiddleSection(ViewRenderer::SegmentRenderData &RenderData, int iXCurrent, int CurrentCeilingEnd, int CurrentFloorStart);
    void DrawLowerSection(ViewRenderer::SegmentRenderData &RenderData, int iXCurrent, int CurrentFloorStart);
    void DrawUpperSection(ViewRenderer::SegmentRenderData &RenderData, int iXCurrent, int CurrentCeilingEnd);

	uint8_t GetSectionColor(const std::string &TextureName);

    bool ValidateRange(ViewRenderer::SegmentRenderData &RenderData, int &iXCurrent, int &CurrentCeilingEnd, int &CurrentFloorStart);

    float GetScaleFactor(int VXScreen, Angle NormalAngle, float NormalDistance);

    int AngleToScreen(Angle angle);

    int m_iRenderXSize;
    int m_iRenderYSize;
    int m_iDistancePlayerToScreen;
    int m_HalfScreenWidth;
    int m_HalfScreenHeight;

    SDL_Color GetWallColor(std::string textureName);

    Map *m_pMap;
    Player *m_pPlayer;

    std::list<SolidSegmentRange> m_SolidWallRanges;
    std::vector<int> m_FloorClipHeight;
    std::vector<int> m_CeilingClipHeight;
    std::map<std::string, uint8_t> m_WallColor;
    std::map<int, Angle> m_ScreenXToAngle;
    bool m_UseClassicDoomScreenToAngle;

	uint8_t *m_pScreenBuffer;
	int m_iBufferPitch;

};
