#include "WADReader.h"
#include <cstring>

WADReader::WADReader()
{
}

WADReader::~WADReader()
{
}

uint16_t WADReader::Read2Bytes(const uint8_t *pWADData, int offset)
{
    uint16_t ReadValue;
    std::memcpy(&ReadValue, pWADData + offset, sizeof(uint16_t));
    return ReadValue;
}

uint32_t WADReader::Read4Bytes(const uint8_t *pWADData, int offset)
{
    uint32_t ReadValue;
    std::memcpy(&ReadValue, pWADData + offset, sizeof(uint32_t));
    return ReadValue;
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
