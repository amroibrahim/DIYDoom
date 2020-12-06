#pragma once

#include <cstdint> 
#include <fstream>
#include <string>
#include <vector>

#include "DataTypes.h"

class WADLoader
{
public:
    WADLoader(std::string sWADFilePath);
    bool LoadWAD();

    ~WADLoader();

protected:
    bool OpenAndLoad();
    bool ReadDirectories();

    std::string m_sWADFilePath;
    std::ifstream m_WADFile;
    uint8_t *m_WADData;
    std::vector<Directory> m_WADDirectories;
};
