#pragma once
#include "Patch.h"

class Weapon
{
public:
    Weapon(const std::string &sWeaponPatch);
    virtual ~Weapon();

    void Render(uint8_t *pScreenBuffer, int iBufferPitch);

protected:
    Patch *m_pPatch;
};

