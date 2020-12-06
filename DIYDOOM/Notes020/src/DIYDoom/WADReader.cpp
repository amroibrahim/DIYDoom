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

void WADReader::ReadVertexData(const uint8_t *pWADData, int offset, Vertex &vertex)
{
    //0x00 to 0x01
    vertex.XPosition = Read2Bytes(pWADData, offset);

    //0x02 to 0x03
    vertex.YPosition = Read2Bytes(pWADData, offset + 2);
}

void WADReader::ReadSectorData(const uint8_t *pWADData, int offset, WADSector &sector)
{
    sector.FloorHeight = Read2Bytes(pWADData, offset);
    sector.CeilingHeight = Read2Bytes(pWADData, offset + 2);

    sector.FloorTexture[0] = pWADData[offset + 4];
    sector.FloorTexture[1] = pWADData[offset + 5];
    sector.FloorTexture[2] = pWADData[offset + 6];
    sector.FloorTexture[3] = pWADData[offset + 7];
    sector.FloorTexture[4] = pWADData[offset + 8];
    sector.FloorTexture[5] = pWADData[offset + 9];
    sector.FloorTexture[6] = pWADData[offset + 10];
    sector.FloorTexture[7] = pWADData[offset + 11];

    sector.CeilingTexture[0] = pWADData[offset + 12];
    sector.CeilingTexture[1] = pWADData[offset + 13];
    sector.CeilingTexture[2] = pWADData[offset + 14];
    sector.CeilingTexture[3] = pWADData[offset + 15];
    sector.CeilingTexture[4] = pWADData[offset + 16];
    sector.CeilingTexture[5] = pWADData[offset + 17];
    sector.CeilingTexture[6] = pWADData[offset + 18];
    sector.CeilingTexture[7] = pWADData[offset + 19];

    sector.Lightlevel = Read2Bytes(pWADData, offset + 20);
    sector.Type = Read2Bytes(pWADData, offset + 22);
    sector.Tag = Read2Bytes(pWADData, offset + 24);
}

void WADReader::ReadSidedefData(const uint8_t *pWADData, int offset, WADSidedef &sidedef)
{
    sidedef.XOffset = Read2Bytes(pWADData, offset);
    sidedef.YOffset = Read2Bytes(pWADData, offset + 2);
    sidedef.UpperTexture[0] = pWADData[offset + 4];
    sidedef.UpperTexture[1] = pWADData[offset + 5];
    sidedef.UpperTexture[2] = pWADData[offset + 6];
    sidedef.UpperTexture[3] = pWADData[offset + 7];
    sidedef.UpperTexture[4] = pWADData[offset + 8];
    sidedef.UpperTexture[5] = pWADData[offset + 9];
    sidedef.UpperTexture[6] = pWADData[offset + 10];
    sidedef.UpperTexture[7] = pWADData[offset + 11];

    sidedef.LowerTexture[0] = pWADData[offset + 12];
    sidedef.LowerTexture[1] = pWADData[offset + 13];
    sidedef.LowerTexture[2] = pWADData[offset + 14];
    sidedef.LowerTexture[3] = pWADData[offset + 15];
    sidedef.LowerTexture[4] = pWADData[offset + 16];
    sidedef.LowerTexture[5] = pWADData[offset + 17];
    sidedef.LowerTexture[6] = pWADData[offset + 18];
    sidedef.LowerTexture[7] = pWADData[offset + 19];

    sidedef.MiddleTexture[0] = pWADData[offset + 20];
    sidedef.MiddleTexture[1] = pWADData[offset + 21];
    sidedef.MiddleTexture[2] = pWADData[offset + 22];
    sidedef.MiddleTexture[3] = pWADData[offset + 23];
    sidedef.MiddleTexture[4] = pWADData[offset + 24];
    sidedef.MiddleTexture[5] = pWADData[offset + 25];
    sidedef.MiddleTexture[6] = pWADData[offset + 26];
    sidedef.MiddleTexture[7] = pWADData[offset + 27];

    sidedef.SectorID = Read2Bytes(pWADData, offset + 28);
}

void WADReader::ReadLinedefData(const uint8_t *pWADData, int offset, WADLinedef &linedef)
{
    linedef.StartVertexID = Read2Bytes(pWADData, offset);
    linedef.EndVertexID = Read2Bytes(pWADData, offset + 2);
    linedef.Flags = Read2Bytes(pWADData, offset + 4);
    linedef.LineType = Read2Bytes(pWADData, offset + 6);
    linedef.SectorTag = Read2Bytes(pWADData, offset + 8);
    linedef.RightSidedef = Read2Bytes(pWADData, offset + 10);
    linedef.LeftSidedef = Read2Bytes(pWADData, offset + 12);
}

void WADReader::ReadThingData(const uint8_t *pWADData, int offset, Thing &thing)
{
    thing.XPosition = Read2Bytes(pWADData, offset);
    thing.YPosition = Read2Bytes(pWADData, offset + 2);
    thing.Angle = Read2Bytes(pWADData, offset + 4);
    thing.Type = Read2Bytes(pWADData, offset + 6);
    thing.Flags = Read2Bytes(pWADData, offset + 8);
}

void WADReader::ReadNodeData(const uint8_t *pWADData, int offset, Node &node)
{
    node.XPartition = Read2Bytes(pWADData, offset);
    node.YPartition = Read2Bytes(pWADData, offset + 2);
    node.ChangeXPartition = Read2Bytes(pWADData, offset + 4);
    node.ChangeYPartition = Read2Bytes(pWADData, offset + 6);

    node.RightBoxTop = Read2Bytes(pWADData, offset + 8);
    node.RightBoxBottom = Read2Bytes(pWADData, offset + 10);
    node.RightBoxLeft = Read2Bytes(pWADData, offset + 12);
    node.RightBoxRight = Read2Bytes(pWADData, offset + 14);

    node.LeftBoxTop = Read2Bytes(pWADData, offset + 16);
    node.LeftBoxBottom = Read2Bytes(pWADData, offset + 18);
    node.LeftBoxLeft = Read2Bytes(pWADData, offset + 20);
    node.LeftBoxRight = Read2Bytes(pWADData, offset + 22);

    node.RightChildID = Read2Bytes(pWADData, offset + 24);
    node.LeftChildID = Read2Bytes(pWADData, offset + 26);
}

void WADReader::ReadSubsectorData(const uint8_t *pWADData, int offset, Subsector &subsector)
{
    subsector.SegCount = Read2Bytes(pWADData, offset);
    subsector.FirstSegID = Read2Bytes(pWADData, offset + 2);
}

void WADReader::ReadSegData(const uint8_t *pWADData, int offset, WADSeg &seg)
{
    seg.StartVertexID = Read2Bytes(pWADData, offset);
    seg.EndVertexID = Read2Bytes(pWADData, offset + 2);
    seg.SlopeAngle = Read2Bytes(pWADData, offset + 4);
    seg.LinedefID = Read2Bytes(pWADData, offset + 6);
    seg.Direction = Read2Bytes(pWADData, offset + 8);
    seg.Offset = Read2Bytes(pWADData, offset + 10);
}

void WADReader::ReadPalette(const uint8_t *pWADData, int offset, WADPalette &palette)
{
    for (int i = 0; i < 256; ++i)
    {
        palette.Colors[i].r = pWADData[offset++];
        palette.Colors[i].g = pWADData[offset++];
        palette.Colors[i].b = pWADData[offset++];
        palette.Colors[i].a = 255;
    }
}

void WADReader::ReadPatchHeader(const uint8_t *pWADData, int offset, WADPatchHeader &patchheader)
{
    //0x00 to 0x01
    patchheader.Width = Read2Bytes(pWADData, offset);

    //0x02 to 0x03
    patchheader.Height = Read2Bytes(pWADData, offset + 2);

    patchheader.LeftOffset = Read2Bytes(pWADData, offset + 4);
    patchheader.TopOffset = Read2Bytes(pWADData, offset + 6);
    patchheader.pColumnOffsets = new uint32_t[patchheader.Width];

    offset = offset + 8;
    for (int i = 0; i < patchheader.Width; ++i)
    {
        patchheader.pColumnOffsets[i] = Read4Bytes(pWADData, offset);
        offset += 4;
    }
}

void WADReader::ReadPName(const uint8_t *pWADData, int offset, WADPNames &PNames)
{
    PNames.PNameCount = Read4Bytes(pWADData, offset);
    PNames.PNameOffset = offset + 4;
}

void WADReader::ReadTextureHeader(const uint8_t *pWADData, int offset, WADTextureHeader &textureheader)
{
    textureheader.TexturesCount = Read4Bytes(pWADData, offset);
    textureheader.TexturesOffset = Read4Bytes(pWADData, offset + 4);
    textureheader.pTexturesDataOffset = new uint32_t[textureheader.TexturesCount];
    offset = offset + 4;
    for (int i = 0; i < textureheader.TexturesCount; ++i)
    {
        textureheader.pTexturesDataOffset[i] = Read4Bytes(pWADData, offset);
        offset += 4;
    }
}

int WADReader::ReadPatchColumn(const uint8_t *pWADData, int offset, PatchColumnData &patch)
{
    patch.TopDelta = pWADData[offset++];
    int iDataIndex = 0;
    if (patch.TopDelta != 0xFF)
    {
        patch.Length = pWADData[offset++];
        patch.PaddingPre = pWADData[offset++];

        // TODO: use smart pointer
        patch.pColumnData = new uint8_t[patch.Length];

        for (int i = 0; i < patch.Length; ++i)
        {
            patch.pColumnData[i] = pWADData[offset++];
        }

        patch.PaddingPost = pWADData[offset++];
    }
    return offset;
}


void WADReader::ReadTextureData(const uint8_t *pWADData, int offset, WADTextureData &texture)
{
    texture.TextureName[0] = pWADData[offset];
    texture.TextureName[1] = pWADData[offset + 1];
    texture.TextureName[2] = pWADData[offset + 2];
    texture.TextureName[3] = pWADData[offset + 3];
    texture.TextureName[4] = pWADData[offset + 4];
    texture.TextureName[5] = pWADData[offset + 5];
    texture.TextureName[6] = pWADData[offset + 6];
    texture.TextureName[7] = pWADData[offset + 7];
    texture.TextureName[8] = '\0';

    texture.Flags = Read4Bytes(pWADData, offset + 8);
    texture.Width = Read2Bytes(pWADData, offset + 12);
    texture.Height = Read2Bytes(pWADData, offset + 14);
    texture.ColumnDirectory = Read4Bytes(pWADData, offset + 16);
    texture.PatchCount = Read2Bytes(pWADData, offset + 20);
    texture.pTexturePatch = new WADTexturePatch[texture.PatchCount];

    offset += 22;
    for (int i = 0; i < texture.PatchCount; ++i)
    {
        ReadTexturePatch(pWADData, offset, texture.pTexturePatch[i]);
        offset += 10;
    }
}

void WADReader::ReadTexturePatch(const uint8_t *pWADData, int offset, WADTexturePatch &texturepatch)
{
    texturepatch.XOffset = Read2Bytes(pWADData, offset);
    texturepatch.YOffset = Read2Bytes(pWADData, offset + 2);
    texturepatch.PNameIndex = Read2Bytes(pWADData, offset + 4);
    texturepatch.StepDir = Read2Bytes(pWADData, offset + 6);
    texturepatch.ColorMap = Read2Bytes(pWADData, offset + 8);
}

void WADReader::Read8Characters(const uint8_t * pWADData, int offset, char *pName)
{
    pName[0] = pWADData[offset++];
    pName[1] = pWADData[offset++];
    pName[2] = pWADData[offset++];
    pName[3] = pWADData[offset++];
    pName[4] = pWADData[offset++];
    pName[5] = pWADData[offset++];
    pName[6] = pWADData[offset++];
    pName[7] = pWADData[offset];
}
