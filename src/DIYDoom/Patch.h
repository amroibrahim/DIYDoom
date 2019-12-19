#pragma once
#include <SDL.h>
#include <string>
#include <vector>
#include <memory>

#include "DataTypes.h"

class Patch
{
public:
    Patch(std::string sName);
    ~Patch();

    void Initialize(WADPatchHeader &PatchHeader);
    void AppendPatchColumn(WADPatchColumn &PatchColumn);
    void Render(uint8_t *pScreenBuffer, int iBufferPitch, int iXScreenLocation, int iYScreenLocation);

    int GetHeight();
    int GetWidth();
    int GetXOffset();
    int GetYOffset();

protected:
    int m_iHeight;
    int m_iWidth;
    int m_iXOffset;
    int m_iYOffset;

    std::string m_sName;
    std::vector<WADPatchColumn> m_PatchData;
};

