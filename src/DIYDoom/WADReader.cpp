#include "WADReader.h"

WADReader::WADReader()
{
}

WADReader::~WADReader()
{
}

uint16_t WADReader::Read2Bytes(const uint8_t *pWADData, int offset)
{
    return (pWADData[offset + 1] << 8) | pWADData[offset];
}

uint32_t WADReader::Read4Bytes(const uint8_t *pWADData, int offset)
{
    return (pWADData[offset + 3] << 24) | (pWADData[offset + 2] << 16) | (pWADData[offset + 1] << 8) | pWADData[offset];
}

void WADReader::ReadHeaderData(const uint8_t *pWADData, int offset, Header &header)
{
    //0x00 to 0x03
    header.WADType[0] = pWADData[offset];
    header.WADType[1] = pWADData[offset + 1];
    header.WADType[2] = pWADData[offset + 2];
    header.WADType[3] = pWADData[offset + 3];
    header.WADType[4] = '\0';

    //0x04 to 0x07
    header.DirectoryCount = Read4Bytes(pWADData, offset + 4);

    //0x08 to 0x0b
    header.DirectoryOffset = Read4Bytes(pWADData, offset + 8);
}

void WADReader::ReadDirectoryData(const uint8_t *pWADData, int offset, Directory &directory)
{
    //0x00 to 0x03
    directory.LumpOffset = Read4Bytes(pWADData, offset);

    //0x04 to 0x07
    directory.LumpSize = Read4Bytes(pWADData, offset + 4);

    //0x08 to 0x0F
    directory.LumpName[0] = pWADData[offset + 8];
    directory.LumpName[1] = pWADData[offset + 9];
    directory.LumpName[2] = pWADData[offset + 10];
    directory.LumpName[3] = pWADData[offset + 11];
    directory.LumpName[4] = pWADData[offset + 12];
    directory.LumpName[5] = pWADData[offset + 13];
    directory.LumpName[6] = pWADData[offset + 14];
    directory.LumpName[7] = pWADData[offset + 15];
    directory.LumpName[8] = '\0';
}

void WADReader::ReadVertexData(const uint8_t *pWADData, int offset, Vertex &vertex)
{
    //0x00 to 0x01
    vertex.XPosition = Read2Bytes(pWADData, offset);

    //0x02 to 0x03
    vertex.YPosition = Read2Bytes(pWADData, offset + 2);
}

void WADReader::ReadLinedefData(const uint8_t *pWADData, int offset, Linedef &linedef)
{
    linedef.StartVertex = Read2Bytes(pWADData, offset);
    linedef.EndVertex = Read2Bytes(pWADData, offset + 2);
    linedef.Flags = Read2Bytes(pWADData, offset + 4);
    linedef.LineType = Read2Bytes(pWADData, offset + 6);
    linedef.SectorTag = Read2Bytes(pWADData, offset + 8);
    linedef.FrontSidedef = Read2Bytes(pWADData, offset + 10);
    linedef.BackSidedef = Read2Bytes(pWADData, offset + 12);
}

void WADReader::ReadThingData(const uint8_t *pWADData, int offset, Thing &thing)
{
    thing.XPosition = Read2Bytes(pWADData, offset);
    thing.YPosition = Read2Bytes(pWADData, offset + 2);
    thing.Angle = Read2Bytes(pWADData, offset + 4);
    thing.Type = Read2Bytes(pWADData, offset + 6);
    thing.Flags = Read2Bytes(pWADData, offset + 8);
}
