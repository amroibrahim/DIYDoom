#include "Map.h"

#include <stdlib.h> 

using namespace std;

Map::Map(ViewRenderer *pViewRenderer, const std::string &sName, Player *pPlayer, Things *pThings) : m_sName(sName), m_XMin(INT_MAX), m_XMax(INT_MIN), m_YMin(INT_MAX), m_YMax(INT_MIN), m_iLumpIndex(-1), m_pPlayer(pPlayer), m_pThings(pThings), m_pViewRenderer(pViewRenderer)
{
    m_pSectors = new std::vector<WADSector>();
    m_pSidedefs = new std::vector<WADSidedef>();
    m_pLinedefs = new std::vector<WADLinedef>();
    m_pSegs = new std::vector<WADSeg>();
}

Map::~Map()
{
}

void Map::Init()
{
    BuildSectors();
    BuildSidedefs();
    BuildLinedef();
    BuildSeg();
}

void Map::BuildSectors()
{
    WADSector wadsector;
    Sector sector;
    for (int i = 0; i < m_pSectors->size(); ++i)
    {
        wadsector = m_pSectors->at(i);
        sector.FloorHeight = wadsector.FloorHeight;
        sector.CeilingHeight = wadsector.CeilingHeight;
        strncpy_s(sector.FloorTexture, wadsector.FloorTexture, 8);
        sector.FloorTexture[8] = '\0';
        strncpy_s(sector.CeilingTexture, wadsector.CeilingTexture, 8);
        sector.CeilingTexture[8] = '\0';
        sector.Lightlevel = wadsector.Lightlevel;
        sector.Type = wadsector.Type;
        sector.Tag = wadsector.Tag;
        m_Sectors.push_back(sector);
    }
    delete m_pSectors;
    m_pSectors = nullptr;
}

void Map::BuildSidedefs()
{
    WADSidedef wadsidedef;
    Sidedef sidedef;
    for (int i = 0; i < m_pSidedefs->size(); ++i)
    {
        wadsidedef = m_pSidedefs->at(i);
        sidedef.XOffset = wadsidedef.XOffset;
        sidedef.YOffset = wadsidedef.YOffset;
        strncpy_s(sidedef.UpperTexture, wadsidedef.UpperTexture, 8);
        sidedef.UpperTexture[8] = '\0';
        strncpy_s(sidedef.LowerTexture, wadsidedef.LowerTexture, 8);
        sidedef.LowerTexture[8] = '\0';
        strncpy_s(sidedef.MiddleTexture, wadsidedef.MiddleTexture, 8);
        sidedef.MiddleTexture[8] = '\0';
        sidedef.pSector = &m_Sectors[wadsidedef.SectorID];
        m_Sidedefs.push_back(sidedef);
    }
    delete m_pSidedefs;
    m_pSidedefs = nullptr;
}

void Map::BuildLinedef()
{
    WADLinedef wadlinedef;
    Linedef linedef;

    for (int i = 0; i < m_pLinedefs->size(); ++i)
    {
        wadlinedef = m_pLinedefs->at(i);

        linedef.pStartVertex = &m_Vertexes[wadlinedef.StartVertexID];
        linedef.pEndVertex = &m_Vertexes[wadlinedef.EndVertexID];
        linedef.Flags = wadlinedef.Flags;
        linedef.LineType = wadlinedef.LineType;
        linedef.SectorTag = wadlinedef.SectorTag;

        if (wadlinedef.RightSidedef == 0xFFFF)
        {
            linedef.pRightSidedef = nullptr;
        }
        else
        {
            linedef.pRightSidedef = &m_Sidedefs[wadlinedef.RightSidedef];
        }

        if (wadlinedef.LeftSidedef == 0xFFFF)
        {
            linedef.pLeftSidedef = nullptr;
        }
        else
        {
            linedef.pLeftSidedef = &m_Sidedefs[wadlinedef.LeftSidedef];
        }

        m_Linedefs.push_back(linedef);
    }

    delete m_pLinedefs;
    m_pLinedefs = nullptr;
}

void Map::BuildSeg()
{
    WADSeg wadseg;
    Seg seg;

    for (int i = 0; i < m_pSegs->size(); ++i)
    {
        wadseg = m_pSegs->at(i);

        seg.pStartVertex = &m_Vertexes[wadseg.StartVertexID];
        seg.pEndVertex = &m_Vertexes[wadseg.EndVertexID];
        // 8.38190317e-8 is to convert from Binary angles (BAMS) to float
        seg.SlopeAngle = ((float)(wadseg.SlopeAngle << 16) * 8.38190317e-8);
        seg.pLinedef = &m_Linedefs[wadseg.LinedefID];
        seg.Direction = wadseg.Direction;
        seg.Offset = (float)(wadseg.Offset << 16) / (float)(1 << 16);

        Sidedef *pRightSidedef;
        Sidedef *pLeftSidedef;

        if (seg.Direction)
        {
            pRightSidedef = seg.pLinedef->pLeftSidedef;
            pLeftSidedef = seg.pLinedef->pRightSidedef;
        }
        else
        {
            pRightSidedef = seg.pLinedef->pRightSidedef;
            pLeftSidedef = seg.pLinedef->pLeftSidedef;
        }

        if (pRightSidedef)
        {
            seg.pRightSector = pRightSidedef->pSector;
        }
        else
        {
            seg.pRightSector = nullptr;
        }

        if (pLeftSidedef)
        {
            seg.pLeftSector = pLeftSidedef->pSector;
        }
        else
        {
            seg.pLeftSector = nullptr;
        }

        m_Segs.push_back(seg);
    }

    delete m_pSegs;
    m_pSegs = nullptr;
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

void Map::AddSidedef(WADSidedef &sidedef)
{
    m_pSidedefs->push_back(sidedef);
}

void Map::AddSector(WADSector &sector)
{
    m_pSectors->push_back(sector);
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

void Map::AddLinedef(WADLinedef &l)
{
    m_pLinedefs->push_back(l);
}

void Map::AddNode(Node &node)
{
    m_Nodes.push_back(node);
}

void Map::AddSubsector(Subsector &subsector)
{
    m_Subsector.push_back(subsector);
}

void Map::AddSeg(WADSeg &seg)
{
    m_pSegs->push_back(seg);
}

string Map::GetName()
{
    return m_sName;
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

    bool isOnLeft = IsPointOnLeftSide(m_pPlayer->GetXPosition(), m_pPlayer->GetYPosition(), iNodeID);

    if (isOnLeft)
    {
        RenderBSPNodes(m_Nodes[iNodeID].LeftChildID);
        RenderBSPNodes(m_Nodes[iNodeID].RightChildID);
    }
    else
    {
        RenderBSPNodes(m_Nodes[iNodeID].RightChildID);
        RenderBSPNodes(m_Nodes[iNodeID].LeftChildID);
    }
}

void Map::RenderSubsector(int iSubsectorID)
{
    Subsector &subsector = m_Subsector[iSubsectorID];

    for (int i = 0; i < subsector.SegCount; i++)
    {
        Seg &seg = m_Segs[subsector.FirstSegID + i];
        Angle V1Angle, V2Angle, V1AngleFromPlayer, V2AngleFromPlayer;
        if (m_pPlayer->ClipVertexesInFOV(*(seg.pStartVertex), *(seg.pEndVertex), V1Angle, V2Angle, V1AngleFromPlayer, V2AngleFromPlayer))
        {
            m_pViewRenderer->AddWallInFOV(seg, V1Angle, V2Angle, V1AngleFromPlayer, V2AngleFromPlayer);
        }
    }
}

bool Map::IsPointOnLeftSide(int XPosition, int YPosition, int iNodeID)
{
    int dx = XPosition - m_Nodes[iNodeID].XPartition;
    int dy = YPosition - m_Nodes[iNodeID].YPartition;

    return (((dx * m_Nodes[iNodeID].ChangeYPartition) - (dy * m_Nodes[iNodeID].ChangeXPartition)) <= 0);
}

void Map::SetLumpIndex(int iIndex)
{
    m_iLumpIndex = iIndex;
}

int Map::GetLumpIndex()
{
    return m_iLumpIndex;
}

Things* Map::GetThings()
{
    return m_pThings;
}

int Map::GetPlayerSubSectorHieght()
{
    int iSubsectorID = m_Nodes.size() - 1;
    while (!(iSubsectorID & SUBSECTORIDENTIFIER))
    {

        bool isOnLeft = IsPointOnLeftSide(m_pPlayer->GetXPosition(), m_pPlayer->GetYPosition(), iSubsectorID);

        if (isOnLeft)
        {
            iSubsectorID = m_Nodes[iSubsectorID].LeftChildID;
        }
        else
        {
            iSubsectorID = m_Nodes[iSubsectorID].RightChildID;
        }
    }
    Subsector &subsector = m_Subsector[iSubsectorID & (~SUBSECTORIDENTIFIER)];
    Seg &seg = m_Segs[subsector.FirstSegID];
    return seg.pRightSector->FloorHeight;
    
}
