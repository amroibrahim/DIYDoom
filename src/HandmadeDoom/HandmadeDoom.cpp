#include <iostream>
#include "WADLoader.h"

int main()
{
    WADLoader wadloader("D:\\SDKs\\Assets\\Doom\\DOOM.WAD");
    wadloader.LoadWAD();
    return 0;
}
