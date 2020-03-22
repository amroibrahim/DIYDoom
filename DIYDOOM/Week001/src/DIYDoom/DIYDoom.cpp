#include <iostream>
#include "WADLoader.h"

int main()
{
    WADLoader wadloader("..\\..\\..\\external\\assets\\DOOM.WAD");
    wadloader.LoadWAD();
    return 0;
}
