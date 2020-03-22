#include "Patch.h"

using namespace std;

Patch::Patch(std::string sName) : m_sName(sName)
{
}

Patch::~Patch()
{
    for (size_t iPatchColumnIndex = 0; iPatchColumnIndex < m_PatchData.size(); ++iPatchColumnIndex)
    {
        if (m_PatchData[iPatchColumnIndex].TopDelta == 0xFF)
        {
            continue;
        }

        delete[] m_PatchData[iPatchColumnIndex].pColumnData;
        m_PatchData[iPatchColumnIndex].pColumnData = nullptr;
    }
}

void Patch::Initialize(WADPatchHeader &PatchHeader)
{
    m_iWidth = PatchHeader.Width;
    m_iHeight = PatchHeader.Height;
    m_iXOffset = PatchHeader.LeftOffset;
    m_iYOffset = PatchHeader.TopOffset;
}

void Patch::AppendPatchColumn(WADPatchColumn &PatchColumn)
{
    m_PatchData.push_back(PatchColumn);
}

void Patch::Render(uint8_t *pScreenBuffer, int iBufferPitch, int iXScreenLocation, int iYScreenLocation)
{
    int iXIndex = 0;
    for (size_t iPatchColumnIndex = 0; iPatchColumnIndex < m_PatchData.size(); iPatchColumnIndex++)
    {
        if (m_PatchData[iPatchColumnIndex].TopDelta == 0xFF)
        {
            ++iXIndex;
            continue;
        }

        for (int iYIndex = 0; iYIndex < m_PatchData[iPatchColumnIndex].Length; ++iYIndex)
        {
            pScreenBuffer[320 * (iYScreenLocation + m_PatchData[iPatchColumnIndex].TopDelta + iYIndex) + (iXScreenLocation + iXIndex)] = m_PatchData[iPatchColumnIndex].pColumnData[iYIndex];
        }
    }
}

int Patch::GetHeight()
{
    return m_iHeight;
}

int Patch::GetWidth()
{
    return m_iWidth;
}

int Patch::GetXOffset()
{
    return m_iXOffset;
}

int Patch::GetYOffset()
{
    return m_iYOffset;
}

