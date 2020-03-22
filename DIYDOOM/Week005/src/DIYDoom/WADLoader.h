#pragma once

#include <cstdint> 
#include <fstream>
#include <string>
#include <vector>

#include "DataTypes.h"
#include "WADReader.h"
#include "Map.h"

class WADLoader
{
public:
    WADLoader();
    void SetWADFilePath(std::string sWADFilePath);
    bool LoadWAD();
    bool LoadMapData(Map *pMap);

    ~WADLoader();

protected:
    bool OpenAndLoad();
    bool ReadDirectories();
    bool ReadMapVertex(Map *pMap);
    bool ReadMapLinedef(Map *pMap);
    bool ReadMapThing(Map *pMap);
    int FindMapIndex(Map *pMap);

    std::string m_sWADFilePath;
    std::ifstream m_WADFile;
    std::vector<Directory> m_WADDirectories;
    uint8_t *m_WADData;
    WADReader m_Reader;
};

