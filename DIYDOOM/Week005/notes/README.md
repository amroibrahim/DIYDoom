# Week 005 - Player and Things
Before we start looking into any rendering we will need to know where the player is looking to render what he is looking at. So, let's focus on where the player spawns on a map.  
The player is a "Thing"! Player spawn location is specified in the Things lump of the map.  
  
## Goals
1. Load the Things lump.  
2. Create a player class.  
3. Add the player to automap.  
  
## Design
Adding a new Player class and refactor code.  

## Coding
Let's start by adding the player class. All we need to keep track of now is the player location and angle he is looking at, in other words, some Setters and Getters. Doom supports up to four players in CO-OP, and uses the player number as their ID. So, player 1 will have ID 1, player 2 will have ID 2 and so on. Our focus now will mainly be on player 1.  

``` cpp
class Player
{
public:
    Player(int iID);
    ~Player();

    void SetXPosition(int XPosition);
    void SetYPosition(int YPosition);
    void SetAngle(int Angle);

    int GetID();
    int GetXPosition();
    int GetYPosition();
    int GetAngle();

    
protected:
    int m_iPlayerID;
    int m_XPosition;
    int m_YPosition;
    int m_Angle;
};
```
  
One thing to note here is that some of those variables will at some point change type from int to a float.
  
Let's update the map class to be aware of the player so we will update the constructor to take a copy of the player and keep.
  
``` cpp
Map::Map(std::string sName, Player *pPlayer)
```

Also, we will need to keep track of the things on the map, let's add another vector and a function that will help us add things in the map.

``` cpp
void AddThing(Thing &thing);
...
std::vector<Thing> m_Things;
```

With that out of the way, let's get serious!
Let's look at the "Things" format.

| Field Size | Data Type      | Content    |  
|------------|----------------|------------| 
|  0x00-0x01 | Signed short   | X Position |
|  0x02-0x03 | Signed short   | Y Position |
|  0x04-0x05 | Unsigned short | Angle      |
|  0x06-0x07 | Unsigned short | Type       |
|  0x08-0x09 | Unsigned short | Flags      |

with this information in mind let's parse and extract this information. That should be a simple copy paste by now! (copy pasting code is a sign your coding can be refactored, but let's keep going for now).

Let's add the Thing struct in DataTypes.h.

``` cpp
struct Thing
{
    int16_t XPosition;
    int16_t YPosition;
    uint16_t Angle;
    uint16_t Type;
    uint16_t Flags;
};
```

Nothing new here, just the usual!

``` cpp
bool WADLoader::ReadMapThing(Map *pMap)
{
    int iMapIndex = FindMapIndex(pMap);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eTHINGS;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "THINGS") != 0)
    {
        return false;
    }

    int iThingsSizeInBytes = sizeof(Thing);
    int iThingsCount = m_WADDirectories[iMapIndex].LumpSize / iThingsSizeInBytes;

    Thing;
    for (int i = 0; i < iThingsCount; ++i)
    {
        m_Reader.ReadThingData(m_WADData, m_WADDirectories[iMapIndex].LumpOffset + i * iThingsSizeInBytes, thing);

        pMap->AddThing(thing);

        //cout << thing.XPosition << endl;
        //cout << thing.YPosition << endl;
        //cout << thing.Angle << endl;
        //cout << thing.Type << endl;
        //cout << thing.Flags << endl;
        //std::cout << std::endl;
    }

    return true;
}
```
  
Also, same here just reading the data!
  
``` cpp
void WADReader::ReadThingData(const uint8_t *pWADData, int offset, Thing &thing)
{
    thing.XPosition = Read2Bytes(pWADData, offset);
    thing.YPosition = Read2Bytes(pWADData, offset + 2);
    thing.Angle = Read2Bytes(pWADData, offset + 4);
    thing.Type = Read2Bytes(pWADData, offset + 6);
    thing.Flags = Read2Bytes(pWADData, offset + 8);
}
```
  
Let's look at what we have read from the lump and match it with Slade3.
  
![Things](./img/things.PNG)  
  
All looks good!
Now let's try and draw the player as a small dot on the automap.
  
I have done some refactoring to the drawing function; I have broken down RenderAutoMap to two smaller helper functions.
  
``` cpp
void RenderAutoMapPlayer(SDL_Renderer * pRenderer, int iXShift, int iYShift);
void RenderAutoMapWalls(SDL_Renderer * pRenderer, int iXShift, int iYShift);
```
  
Now RenderAutoMap looks cleaner
  
``` cpp
void Map::RenderAutoMap(SDL_Renderer *pRenderer)
{
    int iXShift = -m_XMin;
    int iYShift = -m_YMin;

    RenderAutoMapWalls(pRenderer, iXShift, iYShift);
    RenderAutoMapPlayer(pRenderer, iXShift, iYShift);
}
``` 
  
Most of the code that was in RenderAutoMap was moved into this function so nothing new.
  
```cpp
void Map::RenderAutoMapWalls(SDL_Renderer *pRenderer, int iXShift, int iYShift)
{
    int iRenderXSize;
    int iRenderYSize;

    SDL_RenderGetLogicalSize(pRenderer, &iRenderXSize, &iRenderYSize);
    --iRenderXSize;
    --iRenderYSize;

    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    for (Linedef &l : m_Linedef)
    {
        Vertex vStart = m_Vertexes[l.StartVertex];
        Vertex vEnd = m_Vertexes[l.EndVertex];

        SDL_RenderDrawLine(pRenderer,
            (vStart.XPosition + iXShift) / m_iAutoMapScaleFactor,
            iRenderYSize - (vStart.YPosition + iYShift) / m_iAutoMapScaleFactor,
            (vEnd.XPosition + iXShift) / m_iAutoMapScaleFactor,
            iRenderYSize - (vEnd.YPosition + iYShift) / m_iAutoMapScaleFactor);
    }
}
```

Okay! Now let's render the player. Initially, I wanted to draw the player as a filled circle but there was no function to do so in SDL! So, I tried the second-best thing to draw a cluster of dots of size  3 x 3 to show the player. You could have done a 2 for-loops that draw the same, your free to do so it makes things easier.

``` cpp
void Map::RenderAutoMapPlayer(SDL_Renderer *pRenderer, int iXShift, int iYShift)
{
    int iRenderXSize;
    int iRenderYSize;

    SDL_RenderGetLogicalSize(pRenderer, &iRenderXSize, &iRenderYSize);
    --iRenderXSize;
    --iRenderYSize;
    // Let's draw player in Red
    SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    
    // Just draw on the 
    pair<int, int> Direction[] = {
        make_pair(-1, -1), make_pair(0, -1), make_pair(+1, -1), // Above row
        make_pair(-1, 0), make_pair(0, 0), make_pair(+1, 0),    // Center Row
        make_pair(-1, +1), make_pair(0, +1), make_pair(+1, +1)  // Bottom Row
    };

    for (int i = 0; i < 9; ++i)
    {
        SDL_RenderDrawPoint(pRenderer,
            (m_pPlayer->GetXPosition() + iXShift) / m_iAutoMapScaleFactor + Direction[i].first,
            iRenderYSize - (m_pPlayer->GetYPosition() + iYShift) / m_iAutoMapScaleFactor + Direction[i].second);
    }
}
```

Let's look at the cute player now!
![Player](./img/player.PNG)  
  
Map class also had a small optimization. If the map index is previously looked up from the lump, it should be cached.
Just add an integer to store the lump index and initialize it with -1. Before any lookup just check if the index is valid (m_iLumpIndex > -1), if so, just use that, if not look it up and update it. 
  
Map class 

``` cpp
void SetLumpIndex(int iIndex);
...
int m_iLumpIndex;
```

Now let's Update WADLoader to utilize that  

``` cpp
int WADLoader::FindMapIndex(Map *pMap)
{
    // Is the index previously found?
    if (pMap->GetLumpIndex() > -1)
    {
        // Just return it and don’t search
        return pMap->GetLumpIndex();
    }
        
    // Not found, we need to look it up
    for (int i = 0; i < m_WADDirectories.size(); ++i)
    {
        if (m_WADDirectories[i].LumpName == pMap->GetName())
        {
            pMap->SetLumpIndex(i);
            return i;
        }
    }

    return -1;
}
```

Other minor changes I did was add more logging, so it is clear what is happening in the console window.  

## Other Notes
In Chocolate DOOM the function P_LoadThings in p_setup.c does load the things for a map. One interesting thing was the following  
``` cpp 
	// Do not spawn cool, new monsters if !commercial
	if (gamemode != commercial)
	{
	    switch (SHORT(mt->type))
	    {
	      case 68:	// Arachnotron
	      case 64:	// Archvile
	      case 88:	// Boss Brain
	      case 89:	// Boss Shooter
	      case 69:	// Hell Knight
	      case 67:	// Mancubus
	      case 71:	// Pain Elemental
	      case 65:	// Former Human Commando
	      case 66:	// Revenant
	      case 84:	// Wolf SS
		spawn = false;
		break;
	    }
	}
```

This code disables monsters to spawn from non-commercial version. So, if you’re running a shareware version you will not have the full experience as retail/commercial release.

## Source code
[Source code](../src)  

## Reference
[Doom Wiki](https://doomwiki.org/wiki/Thing)  
[ZDoom Wiki](https://zdoom.org/wiki/Thing)   
