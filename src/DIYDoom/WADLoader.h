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
    bool ReadMapVertexes(Map *pMap);
    bool ReadMapLinedefs(Map *pMap);
    bool ReadMapThings(Map *pMap);
    bool ReadMapNodes(Map *pMap);
    bool ReadMapSubsectors(Map *pMap);
    bool ReadMapSegs(Map *pMap);

    int FindMapIndex(Map *pMap);

    std::string m_sWADFilePath;
    std::ifstream m_WADFile;
    std::vector<Directory> m_WADDirectories;
    uint8_t *m_WADData;
    WADReader m_Reader;
};
