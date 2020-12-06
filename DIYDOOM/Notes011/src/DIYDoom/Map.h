#pragma once

#include <vector>
#include <string>

#include "DataTypes.h"
#include "Player.h"
#include "ViewRenderer.h"

class Map
{
public:
    Map(ViewRenderer *pViewRenderer, std::string sName, Player *pPlayer);
    ~Map();

    std::string GetName();
    void AddVertex(Vertex &v);
    void AddLinedef(Linedef &l);
    void AddThing(Thing &thing);
    void AddNode(Node &node);
    void AddSubsector(Subsector &subsector);
    void AddSeg(Seg &seg);
    void RenderAutoMap();
    void Render3DView();
    void SetLumpIndex(int iIndex);

    int GetXMin();
    int GetXMax();
    int GetYMin();
    int GetYMax();
    int GetLumpIndex();

protected:
    void RenderBSPNodes();
    void RenderAutoMapNode(int iNodeID);
    void RenderBSPNodes(int iNodeID);
    void RenderSubsector(int iSubsectorID);

    bool IsPointOnLeftSide(int XPosition, int YPosition, int iNodeID);

    std::string m_sName;
    std::vector<Vertex> m_Vertexes;
    std::vector<Linedef> m_Linedefs;
    std::vector<Thing> m_Things;
    std::vector<Node> m_Nodes;
    std::vector<Subsector> m_Subsector;
    std::vector<Seg> m_Segs;

    int m_XMin;
    int m_XMax;
    int m_YMin;
    int m_YMax;
    int m_iLumpIndex;

    Player *m_pPlayer;
    ViewRenderer *m_pViewRenderer;
};
