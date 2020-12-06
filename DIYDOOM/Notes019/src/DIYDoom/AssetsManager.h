#pragma once
#include <map>
#include <vector>
#include <memory>
#include <string>

#include "Patch.h"
#include "DataTypes.h"
#include "WADLoader.h"

class WADLoader;

class AssetsManager
{
public:
    static AssetsManager* GetInstance();
    void Init(WADLoader* pWADLoader);

    ~AssetsManager();

    Patch* AddPatch(const std::string &sPatchName, WADPatchHeader &PatchHeader);
    Patch* GetPatch(const std::string &sPatchName);

protected:
    static bool m_bInitialized;
    static std::unique_ptr <AssetsManager> m_pInstance;

    AssetsManager();

    void LoadPatch(const std::string &sPatchName);

    std::map<std::string, std::unique_ptr<Patch>> m_PatchesCache;

    WADLoader* m_pWADLoader;
};
