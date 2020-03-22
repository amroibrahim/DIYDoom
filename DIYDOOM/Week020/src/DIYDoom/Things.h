#pragma once
#include <vector>

#include "DataTypes.h"

class Things
{
public:
    Things();
    ~Things();

    void AddThing(Thing &thing);
    Thing GetThingByID(int iID);

protected:
    std::vector<Thing> m_Things;
};

