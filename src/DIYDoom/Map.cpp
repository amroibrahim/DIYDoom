#include "Map.h"

using namespace std;

Map::Map(SDL_Renderer *pRenderer, std::string sName, Player *pPlayer) : m_pRenderer(pRenderer), m_sName(sName), m_XMin(INT_MAX), m_XMax(INT_MIN), m_YMin(INT_MAX), m_YMax(INT_MIN), m_iAutoMapScaleFactor(15), m_iLumpIndex(-1), m_pPlayer(pPlayer)
{
    SDL_RenderGetLogicalSize(m_pRenderer, &m_iRenderXSize, &m_iRenderYSize);
    --m_iRenderXSize;
    --m_iRenderYSize;
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

void Map::AddLinedef(Linedef &l)
{
    m_Linedef.push_back(l);
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

string Map::GetName()
{
    return m_sName;
}

int Map::RemapXToScreen(int XMapPosition)
{
    return (XMapPosition + (-m_XMin)) / m_iAutoMapScaleFactor;
}

int Map::RemapYToScreen(int YMapPosition)
{
    return m_iRenderYSize - (YMapPosition + (-m_YMin)) / m_iAutoMapScaleFactor;

}

void Map::RenderAutoMap()
{
    RenderAutoMapWalls();
    RenderAutoMapPlayer();
    RenderAutoMapNode();
}

void Map::RenderAutoMapPlayer()
{
    SDL_SetRenderDrawColor(m_pRenderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

    pair<int, int> Direction[] = {
        make_pair(-1, -1), make_pair(0, -1), make_pair(+1, -1),
        make_pair(-1, 0), make_pair(0, 0), make_pair(+1, 0),
        make_pair(-1, +1), make_pair(0, +1), make_pair(+1, +1)
    };

    for (int i = 0; i < 9; ++i)
    {
        SDL_RenderDrawPoint(m_pRenderer,
            RemapXToScreen(m_pPlayer->GetXPosition()) + Direction[i].first,
            RemapYToScreen(m_pPlayer->GetYPosition()) + Direction[i].second);
    }
}

void Map::RenderAutoMapWalls()
{

    SDL_SetRenderDrawColor(m_pRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    for (Linedef &l : m_Linedef)
    {
        Vertex vStart = m_Vertexes[l.StartVertex];
        Vertex vEnd = m_Vertexes[l.EndVertex];

        SDL_RenderDrawLine(m_pRenderer,
            RemapXToScreen(vStart.XPosition),
            RemapYToScreen(vStart.YPosition),
            RemapXToScreen(vEnd.XPosition),
            RemapYToScreen(vEnd.YPosition));
    }
}

void Map::RenderAutoMapNode()
{
    Node node = m_Nodes[m_Nodes.size() - 1];

    SDL_Rect FrontRect = {
        RemapXToScreen(node.FrontBoxLeft),
        RemapYToScreen(node.FrontBoxTop),
        RemapXToScreen(node.FrontBoxRight) - RemapXToScreen(node.FrontBoxLeft) + 1,
        RemapYToScreen(node.FrontBoxBottom) - RemapYToScreen(node.FrontBoxTop) + 1
    };

    SDL_Rect BackRect = {
        RemapXToScreen(node.BackBoxLeft),
        RemapYToScreen(node.BackBoxTop),
        RemapXToScreen(node.BackBoxRight) - RemapXToScreen(node.BackBoxLeft) + 1,
        RemapYToScreen(node.BackBoxBottom) - RemapYToScreen(node.BackBoxTop) + 1
    };

    SDL_SetRenderDrawColor(m_pRenderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(m_pRenderer, &FrontRect);
    SDL_SetRenderDrawColor(m_pRenderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(m_pRenderer, &BackRect);

    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(m_pRenderer,
        RemapXToScreen(node.XPartition),
        RemapYToScreen(node.YPartition),
        RemapXToScreen(node.XPartition + node.ChangeXPartition),
        RemapYToScreen(node.YPartition + node.ChangeYPartition));
}

void Map::SetLumpIndex(int iIndex)
{
    m_iLumpIndex = iIndex;
}

int Map::GetLumpIndex()
{
    return m_iLumpIndex;
}
