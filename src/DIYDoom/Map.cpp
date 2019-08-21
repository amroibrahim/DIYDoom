#include "Map.h"

#include <stdlib.h> 

using namespace std;

Map::Map(ViewRenderer *pViewRenderer, std::string sName, Player *pPlayer) : m_sName(sName), m_XMin(INT_MAX), m_XMax(INT_MIN), m_YMin(INT_MAX), m_YMax(INT_MIN), m_iLumpIndex(-1), m_pPlayer(pPlayer), m_pViewRenderer(pViewRenderer)
{
}

Map::~Map()
{
}

void Map::AddVertex(Vertex &v)
{
    m_Vertexes.push_back(v);

    if (m_XMin > v.XPosition)
    {
        m_XMin = v.XPosition;
    }
    else if (m_XMax < v.XPosition)
    {
        m_XMax = v.XPosition;
    }

    if (m_YMin > v.YPosition)
    {
        m_YMin = v.YPosition;
    }
    else if (m_YMax < v.YPosition)
    {
        m_YMax = v.YPosition;
    }
}

int Map::GetXMin()
{
    return m_XMin;
}

int Map::GetXMax()
{
    return m_XMax;
}

int Map::GetYMin()
{
    return m_YMin;
}

int Map::GetYMax()
{
    return m_YMax;
}

void Map::AddLinedef(Linedef &l)
{
    m_Linedefs.push_back(l);
}

void Map::AddThing(Thing &thing)
{
    if (thing.Type == m_pPlayer->GetID())
    {
        m_pPlayer->SetXPosition(thing.XPosition);
        m_pPlayer->SetYPosition(thing.YPosition);
        m_pPlayer->SetAngle(thing.Angle);
    }

    m_Things.push_back(thing);
}

void Map::AddNode(Node &node)
{
    m_Nodes.push_back(node);
}

void Map::AddSubsector(Subsector &subsector)
{
    m_Subsector.push_back(subsector);
}

void Map::AddSeg(Seg &seg)
{
    m_Segs.push_back(seg);
}

string Map::GetName()
{
    return m_sName;
}

void Map::RenderAutoMap()
{
    m_pViewRenderer->SetDrawColor(255, 255, 255);

    for (Linedef &l : m_Linedefs)
    {
        Vertex vStart = m_Vertexes[l.StartVertexID];
        Vertex vEnd = m_Vertexes[l.EndVertexID];

        m_pViewRenderer->DrawLine(vStart.XPosition, vStart.YPosition, vEnd.XPosition, vEnd.YPosition);
    }
}

void Map::Render3DView()
{
    RenderBSPNodes();
}

void Map::RenderBSPNodes()
{
    RenderBSPNodes(m_Nodes.size() - 1);
}

void Map::RenderBSPNodes(int iNodeID)
{
    // Masking all the bits exipt the last one
    // to check if this is a subsector
    if (iNodeID & SUBSECTORIDENTIFIER)
    {
        RenderSubsector(iNodeID & (~SUBSECTORIDENTIFIER));
        return;
    }

    bool isOnBack = IsPointOnBackSide(m_pPlayer->GetXPosition(), m_pPlayer->GetYPosition(), iNodeID);

    if (isOnBack)
    {
        RenderBSPNodes(m_Nodes[iNodeID].BackChildID);
        RenderBSPNodes(m_Nodes[iNodeID].FrontChildID);
    }
    else
    {
        RenderBSPNodes(m_Nodes[iNodeID].FrontChildID);
        RenderBSPNodes(m_Nodes[iNodeID].BackChildID);
    }
}

void Map::RenderSubsector(int iSubsectorID)
{
    Subsector subsector = m_Subsector[iSubsectorID];
    m_pViewRenderer->SetDrawColor(rand() % 255, rand() % 255, rand() % 255);

    for (int i = 0; i < subsector.SegCount; i++)
    {
        Seg seg = m_Segs[subsector.FirstSegID + i];
        Angle V1Angle, V2Angle;
        if (m_pPlayer->ClipVertexesInFOV(m_Vertexes[seg.StartVertexID], m_Vertexes[seg.EndVertexID], V1Angle, V2Angle))
        {
            m_pViewRenderer->AddWallInFOV(seg, V1Angle, V2Angle);
        }
    }
}

bool Map::IsPointOnBackSide(int XPosition, int YPosition, int iNodeID)
{
    int dx = XPosition - m_Nodes[iNodeID].XPartition;
    int dy = YPosition - m_Nodes[iNodeID].YPartition;

    return (((dx * m_Nodes[iNodeID].ChangeYPartition) - (dy * m_Nodes[iNodeID].ChangeXPartition)) <= 0);
}

void Map::RenderAutoMapNode(int iNodeID)
{
    Node node = m_Nodes[iNodeID];

    m_pViewRenderer->SetDrawColor(0, 255, 0);
    m_pViewRenderer->DrawRect(node.FrontBoxLeft, node.FrontBoxTop, node.FrontBoxRight, node.FrontBoxBottom);

    m_pViewRenderer->SetDrawColor(255, 0, 0);
    m_pViewRenderer->DrawRect(node.BackBoxLeft, node.BackBoxTop, node.BackBoxRight, node.BackBoxLeft);

    m_pViewRenderer->SetDrawColor(0, 0, 255);
    m_pViewRenderer->DrawLine(node.XPartition, node.YPartition, node.XPartition + node.ChangeXPartition, node.YPartition + node.ChangeYPartition);
}

void Map::SetLumpIndex(int iIndex)
{
    m_iLumpIndex = iIndex;
}

int Map::GetLumpIndex()
{
    return m_iLumpIndex;
}
