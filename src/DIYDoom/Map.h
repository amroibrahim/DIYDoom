#pragma once

#include <vector>
#include <string>

#include <SDL.h>

#include "DataTypes.h"
#include "Player.h"

class Map
{
public:
    Map(std::string sName, Player *pPlayer);
    ~Map();

    std::string GetName();
    void AddVertex(Vertex &v);
    void AddLinedef(Linedef &l);
    void AddThing(Thing &thing);
    void RenderAutoMap(SDL_Renderer *pRenderer);
    void RenderAutoMapPlayer(SDL_Renderer * pRenderer, int iXShift, int iYShift);
    void RenderAutoMapWalls(SDL_Renderer * pRenderer, int iXShift, int iYShift);
    void SetLumpIndex(int iIndex);


    int GetLumpIndex();

protected:
    std::string m_sName;
    std::vector<Vertex> m_Vertexes;
    std::vector<Linedef> m_Linedef;
    std::vector<Thing> m_Things;

    int m_XMin;
    int m_XMax;
    int m_YMin;
    int m_YMax;
    int m_iAutoMapScaleFactor;
    int m_iLumpIndex;

    Player *m_pPlayer;
};

