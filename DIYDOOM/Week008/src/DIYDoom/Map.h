#pragma once

#include <vector>
#include <string>

#include <SDL.h>

#include "DataTypes.h"
#include "Player.h"

class Map
{
public:
    Map(SDL_Renderer *pRenderer, std::string sName, Player *pPlayer);
    ~Map();

    std::string GetName();
    void AddVertex(Vertex &v);
    void AddLinedef(Linedef &l);
    void AddThing(Thing &thing);
    void AddNode(Node &node);
    void RenderAutoMap();
    void SetLumpIndex(int iIndex);


    int GetLumpIndex();

protected:
    void RenderAutoMapPlayer();
    void RenderAutoMapWalls();
    void RenderBSPNodes();
    void RenderAutoMapNode(int iNodeID);
    void RenderBSPNodes(int iNodeID);
    void RenderSubsector(int iSubsectorID);

    int RemapXToScreen(int XMapPosition);
    int RemapYToScreen(int YMapPosition);

    bool IsPointOnLeftSide(int XPosition, int YPosition, int iNodeID);

    std::string m_sName;
    std::vector<Vertex> m_Vertexes;
    std::vector<Linedef> m_Linedef;
    std::vector<Thing> m_Things;
    std::vector<Node> m_Nodes;

    int m_XMin;
    int m_XMax;
    int m_YMin;
    int m_YMax;
    int m_iRenderXSize;
    int m_iRenderYSize;
    int m_iAutoMapScaleFactor;
    int m_iLumpIndex;

    SDL_Renderer *m_pRenderer;

    Player *m_pPlayer;
};
