#pragma once

#include <cstdint>

#include "DataTypes.h"

class WADReader
{
public:
    WADReader();
    ~WADReader();

    void ReadHeaderData(const uint8_t *pWADData, int offset, Header &header);
    void ReadDirectoryData(const uint8_t *pWADData, int offset, Directory &directory);
    void ReadVertexData(const uint8_t *pWADData, int offset, Vertex &vertex);
    void ReadSectorData(const uint8_t *pWADData, int offset, WADSector &sector);
    void ReadSidedefData(const uint8_t *pWADData, int offset, WADSidedef &sidedef);
    void ReadLinedefData(const uint8_t *pWADData, int offset, WADLinedef &linedef);
    void ReadThingData(const uint8_t *pWADData, int offset, Thing &thing);
    void ReadNodeData(const uint8_t *pWADData, int offset, Node &node);
    void ReadSubsectorData(const uint8_t *pWADData, int offset, Subsector &subsector);
    void ReadSegData(const uint8_t *pWADData, int offset, WADSeg &seg);

protected:
    uint16_t Read2Bytes(const uint8_t *pWADData, int offset);
    uint32_t Read4Bytes(const uint8_t *pWADData, int offset);
};
