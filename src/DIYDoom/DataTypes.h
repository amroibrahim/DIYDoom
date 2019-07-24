#pragma once

#include <cstdint>

enum EMAPLUMPSINDEX
{
    eName,
    eTHINGS,
    eLINEDEFS,
    eSIDEDDEFS,
    eVERTEXES,
    eSEAGS,
    eSSECTORS,
    eNODES,
    eSECTORS,
    eREJECT,
    eBLOCKMAP,
    eCOUNT
};

enum ELINEDEFFLAGS
{
    eBLOCKING = 0,
    eBLOCKMONSTERS = 1,
    eTWOSIDED = 2,
    eDONTPEGTOP = 4,
    eDONTPEGBOTTOM = 8,
    eSECRET = 16,
    eSOUNDBLOCK = 32,
    eDONTDRAW = 64,
    eDRAW = 128
};

struct Header
{
    char WADType[5];
    uint32_t DirectoryCount;
    uint32_t DirectoryOffset;
};

struct Directory
{
    uint32_t LumpOffset;
    uint32_t LumpSize;
    char LumpName[9];
};

struct Vertex
{
    int16_t XPosition;
    int16_t YPosition;
};

struct Linedef
{
    uint16_t StartVertex;
    uint16_t EndVertex;
    uint16_t Flags;
    uint16_t LineType;
    uint16_t SectorTag;
    uint16_t FrontSidedef; //0xFFFF means there is no sidedef
    uint16_t BackSidedef;  //0xFFFF means there is no sidedef
};

struct Thing
{
    int16_t XPosition;
    int16_t YPosition;
    uint16_t Angle;
    uint16_t Type;
    uint16_t Flags;
};

struct Node
{
    int16_t XPartition;
    int16_t YPartition;
    int16_t ChangeXPartition;
    int16_t ChangeYPartition;

    int16_t FrontBoxTop;
    int16_t FrontBoxBottom;
    int16_t FrontBoxLeft;
    int16_t FrontBoxRight;

    int16_t BackBoxTop;
    int16_t BackBoxBottom;
    int16_t BackBoxLeft;
    int16_t BackBoxRight;

    uint16_t FrontChildID;
    uint16_t BackChildID;
};
