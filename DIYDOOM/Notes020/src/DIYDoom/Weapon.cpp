#include "Weapon.h"

#include "AssetsManager.h"

Weapon::Weapon(const std::string& sWeaponPatch)
{
    m_pPatch = AssetsManager::GetInstance()->GetPatch(sWeaponPatch);
}

Weapon::~Weapon()
{
}

void Weapon::Render(uint8_t *pScreenBuffer, int iBufferPitch)
{
    m_pPatch->Render(pScreenBuffer, iBufferPitch, -m_pPatch->GetXOffset(), -m_pPatch->GetYOffset());
}
