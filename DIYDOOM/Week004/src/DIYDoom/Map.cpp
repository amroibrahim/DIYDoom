#include "Map.h"

using namespace std;

Map::Map(std::string sName) : m_sName(sName), m_XMin(INT_MAX), m_XMax(INT_MIN), m_YMin(INT_MAX), m_YMax(INT_MIN), m_iAutoMapScaleFactor(15)
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

void Map::AddLinedef(Linedef &l)
{
    m_Linedef.push_back(l);
}

string Map::GetName()
{
    return m_sName;
}

void Map::RenderAutoMap(SDL_Renderer *pRenderer)
{
    int iXShift = -m_XMin;
    int iYShift = -m_YMin;

    int iRenderXSize;
    int iRenderYSize;

    SDL_RenderGetLogicalSize(pRenderer, &iRenderXSize, &iRenderYSize);

    --iRenderXSize;
    --iRenderYSize;

    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    for (Linedef &l : m_Linedef)
    {
        Vertex vStart = m_Vertexes[l.StartVertex];
        Vertex vEnd = m_Vertexes[l.EndVertex];

        SDL_RenderDrawLine(pRenderer,
            (vStart.XPosition + iXShift) / m_iAutoMapScaleFactor,
            iRenderYSize - (vStart.YPosition + iYShift) / m_iAutoMapScaleFactor,
            (vEnd.XPosition + iXShift) / m_iAutoMapScaleFactor,
            iRenderYSize - (vEnd.YPosition + iYShift) / m_iAutoMapScaleFactor);
    }
}