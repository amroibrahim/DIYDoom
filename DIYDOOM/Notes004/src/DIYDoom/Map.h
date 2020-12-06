#pragma once

#include <vector>
#include <string>

#include <SDL.h>

#include "DataTypes.h"

class Map
{
public:
    Map(std::string sName);
    ~Map();

    std::string GetName();
    void AddVertex(Vertex &v);
    void AddLinedef(Linedef &l);
    void RenderAutoMap(SDL_Renderer *pRenderer);

protected:
    std::string m_sName;
    std::vector<Vertex> m_Vertexes;
    std::vector<Linedef> m_Linedef;

    int m_XMin;
    int m_XMax;
    int m_YMin;
    int m_YMax;
    int m_iAutoMapScaleFactor;
};

