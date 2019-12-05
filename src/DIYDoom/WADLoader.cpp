#include "WADLoader.h"

#include <iostream>

using namespace std;

WADLoader::WADLoader() : m_pWADData(nullptr)
{
}

WADLoader::~WADLoader()
{
    delete[] m_pWADData;
}

void WADLoader::SetWADFilePath(const string &sWADFilePath)
{
    m_sWADFilePath = sWADFilePath;
}

bool WADLoader::LoadWADToMemory()
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

    if (m_pWADData)
    {
        delete[] m_pWADData;
    }

    m_pWADData = new uint8_t[length];
    if (m_pWADData == nullptr)
    {
        cout << "Error: Failed alocate memory for WAD file of size " << length << endl;
        return false;
    }

    m_WADFile.seekg(ifstream::beg);
    m_WADFile.read((char *)m_pWADData, length);

    m_WADFile.close();

    cout << "Info: Loading complete." << endl;

    return true;
}

bool WADLoader::ReadDirectories()
{
    Header header;
    m_Reader.ReadHeaderData(m_pWADData, 0, header);

    Directory directory;

    for (unsigned int i = 0; i < header.DirectoryCount; ++i)
    {
        m_Reader.ReadDirectoryData(m_pWADData, header.DirectoryOffset + i * 16, directory);
        m_WADDirectories.push_back(directory);
    }

    return true;
}

bool WADLoader::LoadMapData(Map *pMap)
{
    std::cout << "Info: Parsing Map: " << pMap->GetName() << endl;

    std::cout << "Info: Processing Map Vertexes" << endl;
    if (!ReadMapVertexes(pMap))
    {
        cout << "Error: Failed to load map vertexes data MAP: " << pMap->GetName() << endl;
        return false;
    }

    // Load Sector
    std::cout << "Info: Processing Map Sectors" << endl;
    if (!ReadMapSectors(pMap))
    {
        cout << "Error: Failed to load map sectors data MAP: " << pMap->GetName() << endl;
        return false;
    }

    // Load Sidedef
    std::cout << "Info: Processing Map Sidedefs" << endl;
    if (!ReadMapSidedefs(pMap))
    {
        cout << "Error: Failed to load map Sidedefs data MAP: " << pMap->GetName() << endl;
        return false;
    }

    std::cout << "Info: Processing Map Linedefs" << endl;
    if (!ReadMapLinedefs(pMap))
    {
        cout << "Error: Failed to load map linedefs data MAP: " << pMap->GetName() << endl;
        return false;
    }

    std::cout << "Info: Processing Map Segs" << endl;
    if (!ReadMapSegs(pMap))
    {
        cout << "Error: Failed to load map segs data MAP: " << pMap->GetName() << endl;
        return false;
    }

    std::cout << "Info: Processing Map Things" << endl;
    if (!ReadMapThings(pMap))
    {
        cout << "Error: Failed to load map things data MAP: " << pMap->GetName() << endl;
        return false;
    }

    std::cout << "Info: Processing Map Nodes" << endl;
    if (!ReadMapNodes(pMap))
    {
        cout << "Error: Failed to load map nodes data MAP: " << pMap->GetName() << endl;
        return false;
    }

    std::cout << "Info: Processing Map Subsectors" << endl;
    if (!ReadMapSubsectors(pMap))
    {
        cout << "Error: Failed to load map subsectors data MAP: " << pMap->GetName() << endl;
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

    pMap->SetLumpIndex(FindLumpByName(pMap->GetName()));
    
    return pMap->GetLumpIndex();
}

int WADLoader::FindLumpByName(const string &LumpName)
{
    for (int i = 0; i < m_WADDirectories.size(); ++i)
    {
        if (m_WADDirectories[i].LumpName == LumpName)
        {
            return i;
        }
    }
    return -1;
}

bool WADLoader::ReadMapVertexes(Map *pMap)
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
        m_Reader.ReadVertexData(m_pWADData, m_WADDirectories[iMapIndex].LumpOffset + i * iVertexSizeInBytes, vertex);
        pMap->AddVertex(vertex);
    }

    return true;
}

bool WADLoader::ReadMapSectors(Map *pMap)
{
    int iMapIndex = FindMapIndex(pMap);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eSECTORS;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "SECTORS") != 0)
    {
        return false;
    }

    int iSectorSizeInBytes = sizeof(WADSector);
    int iSectorsCount = m_WADDirectories[iMapIndex].LumpSize / iSectorSizeInBytes;

    WADSector sector;
    for (int i = 0; i < iSectorsCount; ++i)
    {
        m_Reader.ReadSectorData(m_pWADData, m_WADDirectories[iMapIndex].LumpOffset + i * iSectorSizeInBytes, sector);
        pMap->AddSector(sector);
    }

    return true;
}

bool WADLoader::ReadMapSidedefs(Map *pMap)
{
    int iMapIndex = FindMapIndex(pMap);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eSIDEDDEFS;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "SIDEDEFS") != 0)
    {
        return false;
    }

    int iSidedefSizeInBytes = sizeof(WADSidedef);
    int iSidedefsCount = m_WADDirectories[iMapIndex].LumpSize / iSidedefSizeInBytes;

    WADSidedef sidedef;
    for (int i = 0; i < iSidedefsCount; ++i)
    {
        m_Reader.ReadSidedefData(m_pWADData, m_WADDirectories[iMapIndex].LumpOffset + i * iSidedefSizeInBytes, sidedef);
        pMap->AddSidedef(sidedef);
    }

    return true;
}

bool WADLoader::ReadMapLinedefs(Map *pMap)
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

    int iLinedefSizeInBytes = sizeof(WADLinedef);
    int iLinedefCount = m_WADDirectories[iMapIndex].LumpSize / iLinedefSizeInBytes;

    WADLinedef linedef;
    for (int i = 0; i < iLinedefCount; ++i)
    {
        m_Reader.ReadLinedefData(m_pWADData, m_WADDirectories[iMapIndex].LumpOffset + i * iLinedefSizeInBytes, linedef);
        pMap->AddLinedef(linedef);
    }

    return true;
}

bool WADLoader::ReadMapThings(Map *pMap)
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
        m_Reader.ReadThingData(m_pWADData, m_WADDirectories[iMapIndex].LumpOffset + i * iThingsSizeInBytes, thing);
        (pMap->GetThings())->AddThing(thing);
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
        m_Reader.ReadNodeData(m_pWADData, m_WADDirectories[iMapIndex].LumpOffset + i * iNodesSizeInBytes, node);

        pMap->AddNode(node);
    }

    return true;
}

bool WADLoader::ReadMapSubsectors(Map *pMap)
{
    int iMapIndex = FindMapIndex(pMap);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eSSECTORS;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "SSECTORS") != 0)
    {
        return false;
    }

    int iSubsectorsSizeInBytes = sizeof(Subsector);
    int iSubsectorsCount = m_WADDirectories[iMapIndex].LumpSize / iSubsectorsSizeInBytes;

    Subsector subsector;
    for (int i = 0; i < iSubsectorsCount; ++i)
    {
        m_Reader.ReadSubsectorData(m_pWADData, m_WADDirectories[iMapIndex].LumpOffset + i * iSubsectorsSizeInBytes, subsector);

        pMap->AddSubsector(subsector);
    }

    return true;
}

bool WADLoader::ReadMapSegs(Map *pMap)
{
    int iMapIndex = FindMapIndex(pMap);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eSEAGS;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "SEGS") != 0)
    {
        return false;
    }

    int iSegsSizeInBytes = sizeof(WADSeg);
    int iSegsCount = m_WADDirectories[iMapIndex].LumpSize / iSegsSizeInBytes;

    WADSeg seg;
    for (int i = 0; i < iSegsCount; ++i)
    {
        m_Reader.ReadSegData(m_pWADData, m_WADDirectories[iMapIndex].LumpOffset + i * iSegsSizeInBytes, seg);

        pMap->AddSeg(seg);
    }

    return true;
}

bool WADLoader::LoadPalette(DisplayManager *pDisplayManager)
{
    std::cout << "Info: Loading PLAYPAL (Color Palettes)" << endl;
    int iPlaypalIndex = FindLumpByName("PLAYPAL");

    if (strcmp(m_WADDirectories[iPlaypalIndex].LumpName, "PLAYPAL") != 0)
    {
        return false;
    }

    WADPalette palette;
    for (int i = 0; i < 14; ++i)
    {
        m_Reader.ReadPalette(m_pWADData, m_WADDirectories[iPlaypalIndex].LumpOffset + (i * 3 * 256 ), palette);
        pDisplayManager->AddColorPalette(palette);
    }
    
    return true;
}
