#include "Texture.h"
#include "AssetsManager.h"

#include <string>
using namespace std;

Texture::Texture(WADTextureData &TextureData) : m_IsComposed(false), m_iOverLapSize(0), m_pOverLapColumnData(nullptr)
{
    m_sName = TextureData.TextureName;
    //m_Flags = TextureData.Flags;
    m_iWidth = TextureData.Width;
    m_iHeight = TextureData.Height;

    for (int i = 0; i < TextureData.PatchCount; ++i)
    {
        m_TexturePatches.push_back(TextureData.pTexturePatch[i]);
    }

    m_ColumnPatchCount.resize(TextureData.Width, 0);
    m_ColumnIndex.resize(TextureData.Width, 0);
    m_ColumnPatch.resize(TextureData.Width, 0);
}

Texture::~Texture()
{
}

bool Texture::IsComposed()
{
    return m_IsComposed;
}

bool Texture::Initialize()
{
    AssetsManager *pAssetsManager = AssetsManager::GetInstance();

    for (int i = 0; i < m_TexturePatches.size(); ++i)
    {
        //Lookup patch
        Patch *pPatch = pAssetsManager->GetPatch(pAssetsManager->GetPName(m_TexturePatches[i].PNameIndex));

        int iXStart = m_TexturePatches[i].XOffset;
        int iMaxWidth = iXStart + pPatch->GetWidth();


        int iXIndex = iXStart;

        if (iXStart < 0)
        {
            iXIndex = 0;
        }

        //Does this patch extend outside the Texture?
        if (iMaxWidth > m_iWidth)
        {
            iMaxWidth = m_iWidth;
        }

        while (iXIndex < iMaxWidth)
        {
            m_ColumnPatchCount[iXIndex]++;
            m_ColumnPatch[iXIndex] = i/*pPatch*/;
            m_ColumnIndex[iXIndex] = pPatch->GetColumnDataIndex(iXIndex - iXStart);
            ++iXIndex;
        }
    }

    // Cleanup and update
    for (int i = 0; i < m_iWidth; ++i)
    {
        // Is the column covered by more than one patch?
        if (m_ColumnPatchCount[i] > 1)
        {
            m_ColumnPatch[i] = -1;
            m_ColumnIndex[i] = m_iOverLapSize;
            m_iOverLapSize += m_iHeight;
        }
    }

    return true;
}

bool Texture::Compose()
{
    Initialize();

    // allocate required memory for 
    m_pOverLapColumnData = std::unique_ptr<uint8_t[]>(new uint8_t[m_iOverLapSize]);

    AssetsManager *pAssetsManager = AssetsManager::GetInstance();

    for (int i = 0; i < m_TexturePatches.size(); ++i)
    {
        //Lookup patch
        Patch *pPatch = pAssetsManager->GetPatch(pAssetsManager->GetPName(m_TexturePatches[i].PNameIndex));

        int iXStart = m_TexturePatches[i].XOffset;
        int iMaxWidth = iXStart + pPatch->GetWidth();


        int iXIndex = iXStart;

        if (iXStart < 0)
        {
            iXIndex = 0;
        }

        //Does this patch extend outside the Texture?
        if (iMaxWidth > m_iWidth)
        {
            iMaxWidth = m_iWidth;
        }

        while (iXIndex < iMaxWidth)
        {
            // Does this column have more than one patch?
            // if yes compose it, else skip it
            if (m_ColumnPatch[iXIndex] < 0)
            {
                int iPatchColumnIndex = pPatch->GetColumnDataIndex(iXIndex - iXStart);
                pPatch->ComposeColumn(m_pOverLapColumnData.get(), m_iHeight, iPatchColumnIndex, m_ColumnIndex[iXIndex], m_TexturePatches[i].YOffset);
            }

            ++iXIndex;
        }
    }

    m_IsComposed = true;
    return m_IsComposed;
}

void Texture::Render(uint8_t * pScreenBuffer, int iBufferPitch, int iXScreenLocation, int iYScreenLocation)
{
    for (size_t iCurrentColumnIndex = 0; iCurrentColumnIndex < m_iWidth; ++iCurrentColumnIndex)
    {
        RenderColumn(pScreenBuffer, iBufferPitch, iXScreenLocation + iCurrentColumnIndex, iYScreenLocation, iCurrentColumnIndex);
    }
}

void Texture::RenderColumn(uint8_t *pScreenBuffer, int iBufferPitch, int iXScreenLocation, int iYScreenLocation, int iCurrentColumnIndex)
{
    AssetsManager *pAssetsManager = AssetsManager::GetInstance();
    if (m_ColumnPatch[iCurrentColumnIndex] > -1 )
    {
        Patch *pPatch = pAssetsManager->GetPatch(pAssetsManager->GetPName(m_TexturePatches[m_ColumnPatch[iCurrentColumnIndex]].PNameIndex));
        
        pPatch->RenderColumn(pScreenBuffer, iBufferPitch, m_ColumnIndex[iCurrentColumnIndex], iXScreenLocation, iYScreenLocation, m_iHeight, m_TexturePatches[m_ColumnPatch[iCurrentColumnIndex]].YOffset);
    }
    else
    {
        for (int iYIndex = 0; iYIndex < m_iHeight; ++iYIndex)
        {
            pScreenBuffer[iBufferPitch * (iYScreenLocation + iYIndex) + iXScreenLocation] = m_pOverLapColumnData[m_ColumnIndex[iCurrentColumnIndex] + iYIndex];
        }
    }
}
