#pragma once

#include <list>
#include <map>
#include <string>

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
    void AddWallInFOV(Seg &seg, Angle V1Angle, Angle V2Angle);
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

    struct SolidSegmentData
    {
        Seg &seg;
        int XStart;
        int XEnd;
    };

    void RenderAutoMap();
    void Render3DView();
    void DrawSolidWall(SolidSegmentData &visableSeg);

    void ClipSolidWalls(Seg &seg, int V1XScreen, int V2XScreen);
    void StoreWallRange(Seg &seg, int V1XScreen, int V2XScreen);

    int AngleToScreen(Angle angle);
    int RemapXToScreen(int XMapPosition);
    int RemapYToScreen(int YMapPosition);

    int m_iRenderXSize;
    int m_iRenderYSize;
    int m_iAutoMapScaleFactor;

    SDL_Color GetWallColor(std::string textureName);

    Map *m_pMap;
    Player *m_pPlayer;

    SDL_Renderer *m_pRenderer;

    std::list<SolidSegmentRange> m_SolidWallRanges;
    std::map<std::string, SDL_Color> m_WallColor;
};
