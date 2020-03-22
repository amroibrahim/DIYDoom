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

    Directory directory;

    for (unsigned int i = 0; i < header.DirectoryCount; ++i)
    {
        m_Reader.ReadDirectoryData(m_WADData, header.DirectoryOffset + i * 16, directory);
        m_WADDirectories.push_back(directory);
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

    std::cout << "Info: Processing Map Nodes" << endl;
    if (!ReadMapNodes(pMap))
    {
        cout << "Error: Failed to load map node data MAP: " << pMap->GetName() << endl;
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
    }

    return true;
}

bool WADLoader::ReadMapNodes(Map *pMap)
{
    int iMapIndex = FindMapIndex(pMap);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eNODES;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "NODES") != 0)
    {
        return false;
    }

    int iNodesSizeInBytes = sizeof(Node);
    int iNodesCount = m_WADDirectories[iMapIndex].LumpSize / iNodesSizeInBytes;

    Node node;
    for (int i = 0; i < iNodesCount; ++i)
    {
        m_Reader.ReadNodesData(m_WADData, m_WADDirectories[iMapIndex].LumpOffset + i * iNodesSizeInBytes, node);

        pMap->AddNode(node);
    }

    return true;
}
