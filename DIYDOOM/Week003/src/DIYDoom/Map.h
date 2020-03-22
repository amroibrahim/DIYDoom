#pragma once

#include <vector>
#include <string>

#include "DataTypes.h"

class Map
{
public:
    Map(std::string sName);
    ~Map();

    std::string GetName();
    void AddVertex(Vertex &v);
    void AddLinedef(Linedef &l);

protected:
    std::string m_sName;
    std::vector<Vertex> m_Vertexes;
    std::vector<Linedef> m_Linedef;
};

