#include "WADLoader.h"

#include <iostream>

using namespace std;

WADLoader::WADLoader() : m_WADData(NULL)
{
}

WADLoader::~WADLoader()
{
    delete[] m_WADData;
}

void WADLoader::SetWADFilePath(std::string sWADFilePath)
{
    m_sWADFilePath = sWADFilePath;
}

bool WADLoader::LoadWAD()
{
    if (!OpenAndLoad())
    {
        return false;
    }

    if (!ReadDirectories())
    {
        return false;
    }

    return true;
}

bool WADLoader::OpenAndLoad()
{
    std::cout << "Info: Loading WAD file: " << m_sWADFilePath << endl;

    m_WADFile.open(m_sWADFilePath, ifstream::binary);
    if (!m_WADFile.is_open())
    {
        cout << "Error: Failed to open WAD file" << m_sWADFilePath << endl;
        return false;
    }

    m_WADFile.seekg(0, m_WADFile.end);
    size_t length = m_WADFile.tellg();

    if (m_WADData)
    {
        delete[] m_WADData;
    }

    m_WADData = new uint8_t[length];
    if (m_WADData == NULL)
    {
        cout << "Error: Failed alocate memory for WAD file of size " << length << endl;
        return false;
    }

    m_WADFile.seekg(ifstream::beg);
    m_WADFile.read((char *)m_WADData, length);

    m_WADFile.close();

    cout << "Info: Loading complete." << endl;
    return true;
}


bool WADLoader::ReadDirectories()
{

    Header header;
    m_Reader.ReadHeaderData(m_WADData, 0, header);

    //std::cout << header.WADType << std::endl;
    //std::cout << header.DirectoryCount << std::endl;
    //std::cout << header.DirectoryOffset << std::endl;
    //std::cout << std::endl << std::endl;

    Directory directory;

    for (unsigned int i = 0; i < header.DirectoryCount; ++i)
    {
        m_Reader.ReadDirectoryData(m_WADData, header.DirectoryOffset + i * 16, directory);

        m_WADDirectories.push_back(directory);

        //std::cout << directory.LumpOffset << std::endl;
        //std::cout << directory.LumpSize << std::endl;
        //std::cout << directory.LumpName << std::endl;
        //std::cout << std::endl;
    }

    return true;
}

bool WADLoader::LoadMapData(Map *pMap)
{
    std::cout << "Info: Parsing Map: " << pMap->GetName() << endl;

    std::cout << "Info: Processing Map Vertex" << endl;
    if (!ReadMapVertex(pMap))
    {
        cout << "Error: Failed to load map vertex data MAP: " << pMap->GetName() << endl;
        return false;
    }

    std::cout << "Info: Processing Map Linedef" << endl;
    if (!ReadMapLinedef(pMap))
    {
        cout << "Error: Failed to load map linedef data MAP: " << pMap->GetName() << endl;
        return false;
    }

    std::cout << "Info: Processing Map Things" << endl;
    if (!ReadMapThing(pMap))
    {
        cout << "Error: Failed to load map thing data MAP: " << pMap->GetName() << endl;
        return false;
    }
    return true;
}

int WADLoader::FindMapIndex(Map *pMap)
{
    if (pMap->GetLumpIndex() > -1)
    {
        return pMap->GetLumpIndex();
    }

    for (int i = 0; i < m_WADDirectories.size(); ++i)
    {
        if (m_WADDirectories[i].LumpName == pMap->GetName())
        {
            pMap->SetLumpIndex(i);
            return i;
        }
    }

    return -1;
}

bool WADLoader::ReadMapVertex(Map *pMap)
{
    int iMapIndex = FindMapIndex(pMap);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eVERTEXES;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "VERTEXES") != 0)
    {
        return false;
    }

    int iVertexSizeInBytes = sizeof(Vertex);
    int iVertexesCount = m_WADDirectories[iMapIndex].LumpSize / iVertexSizeInBytes;



    Vertex vertex;
    for (int i = 0; i < iVertexesCount; ++i)
    {
        m_Reader.ReadVertexData(m_WADData, m_WADDirectories[iMapIndex].LumpOffset + i * iVertexSizeInBytes, vertex);

        pMap->AddVertex(vertex);

        //cout << vertex.XPosition << endl;
        //cout << vertex.YPosition << endl;
        //std::cout << std::endl;
    }

    return true;
}

bool WADLoader::ReadMapLinedef(Map *pMap)
{
    int iMapIndex = FindMapIndex(pMap);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eLINEDEFS;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "LINEDEFS") != 0)
    {
        return false;
    }


    int iLinedefSizeInBytes = sizeof(Linedef);
    int iLinedefCount = m_WADDirectories[iMapIndex].LumpSize / iLinedefSizeInBytes;

    Linedef linedef;
    for (int i = 0; i < iLinedefCount; ++i)
    {
        m_Reader.ReadLinedefData(m_WADData, m_WADDirectories[iMapIndex].LumpOffset + i * iLinedefSizeInBytes, linedef);

        pMap->AddLinedef(linedef);

        //cout << linedef.StartVertex << endl;
        //cout << linedef.EndVertex << endl;
        //cout << linedef.Flags << endl;
        //cout << linedef.LineType << endl;
        //cout << linedef.SectorTag << endl;
        //cout << linedef.FrontSidedef << endl;
        //cout << linedef.BackSidedef << endl;
        //std::cout << std::endl;
    }

    return true;
}

bool WADLoader::ReadMapThing(Map *pMap)
{
    int iMapIndex = FindMapIndex(pMap);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eTHINGS;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "THINGS") != 0)
    {
        return false;
    }


    int iThingsSizeInBytes = sizeof(Thing);
    int iThingsCount = m_WADDirectories[iMapIndex].LumpSize / iThingsSizeInBytes;

    Thing thing;
    for (int i = 0; i < iThingsCount; ++i)
    {
        m_Reader.ReadThingData(m_WADData, m_WADDirectories[iMapIndex].LumpOffset + i * iThingsSizeInBytes, thing);

        pMap->AddThing(thing);

        //cout << thing.XPosition << endl;
        //cout << thing.YPosition << endl;
        //cout << thing.Angle << endl;
        //cout << thing.Type << endl;
        //cout << thing.Flags << endl;
        //std::cout << std::endl;
    }

    return true;
}
