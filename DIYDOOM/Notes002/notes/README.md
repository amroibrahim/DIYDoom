Reviewers:
*  Marl [@DOOMReboot](https://twitter.com/DOOMReboot)  

# Notes 002 - Basic Map Data
Now that we can read a WAD file, let’s make use of the data we read. It would be nice to read some level (“mission”) data and put it to use. The level-related lumps can be complex, so we’ll go step by step. As a first small step, let’s build something like the automap. If you’re not aware of what an automap is, it’s a bird’s-eye, 2D view of the map. For now, let’s see what’s inside the level lump group.  
  
## Map Anatomy  
First things first: DOOM levels are represented much like a 2D blueprint, with lines representing walls. Each wall gets floor/ceiling heights to give the world its vertical structure (XY is the horizontal plane; Z is height). These components are used to render the level as a 3D world, with constraints such as no room-over-room and (in the original renderer) no free look up/down.  

These curiosities have fueled endless debates over whether DOOM is a 2D or 3D engine. A common compromise is to call it “2.5D”.  

To simplify our task and to bring us back on topic, let’s try to just read that 2D data and see if we could put it to use. We will soon attempt to render it in 3D when we become a bit more familiar with it and how the individual pieces fit together.
  
With some investigation, each level is composed of a set of lumps that (in the original DOOM WADs) appear in a consistent order.  
1. **VERTEXES:** Endpoints of walls in 2D. Two vertices form one linedef. Three vertices can form two linedefs, and so on. Think of a vertex as a meeting point of two or more walls. (Most people prefer “vertices”; “vertexes” is also acceptable per [Merriam-Webster](https://www.merriam-webster.com/dictionary/vertexes).)  
2. **LINEDEFS:** These are the lines forming the connections between VERTEXES, forming walls. Not all lines (walls) behave the same, there are flags that define the behavior of those lines.  
3. **SIDEDEFS:** In real life there are two sides of a wall, the one you are facing, and the one on the other side. Those two sides of the wall could have different textures, SIDEDEFS is a lump which holds the texture information for a wall (LINEDEF).  
4. **SECTORS:** SECTORs are the "rooms" created by connecting LINEDEFs together.  Each sector contains information such as the floor and ceiling heights and textures, and light value, special actions such as moving floors/platforms/elevators.  Some of these settings will also affect how walls are rendered such as the light level and calculation of texture mapping coordinates.
5. **SSECTORS:** SSECTORS (subsectors) form convex areas within a sector which are used to aid in rendering and in tandem with traversal of the BSP which also aids in determining exactly where the player is within a level.  This is quite useful and a very common usage of this is to determine the vertical position of the player.  Each SSECTOR consists of connected fractions of a sector, such as the walls that make up a corner.   
6. **SEGS:** Segs are fractions of a wall/LINEDEF; to put it another way, they are "segments" of a wall/LINEDEF.  The world is rendered by traversing a BSP tree in order to determine which walls to draw first, closest first. These SEGs are then used to render walls rather than the LINEDEFs themselves.  Each SSECTOR's geometry is defined by the segs which it contains.  
7. **NODES:** BSP nodes. This is the BSP tree used for spatial partitioning and rendering traversal. It helps determine which subsector the player is in and supports culling/traversal so the renderer can focus on potentially visible geometry.  
8. **THINGS:** The THINGS Lump is a list of the Mission's decorations and actors (enemies, weapons, etc.).  Each entry in the lump provides information for one instance of an actor/decoration and provides information such as type of object, spawn point, direction they are to face and so forth.
9. **REJECT:** A precomputed table describing (potential) sector-to-sector visibility. It’s primarily used to speed up sight checks for monsters. (Sound propagation is handled via other data structures; REJECT can affect some checks but it’s not the main “sound travel distance” mechanism.)  
10. **BLOCKMAP:** Collision-detection information for player and THING movement.  It consists of a grid which encompasses the entire Mission geometry.  Each cell within the grid contains a list of LINEDEFs that are inside or cross through it. This is used to dramatically speed up collision detection since collision tests will only need to be run against a few LINEDEFs per player/THING which produces a significant computation savings.
  
What we will focus on to generate our 2D map would be VERTEXES and LINEDEFS. If we can plot the vertexes and connect those vertexes with the lines defined by linedefs, then we should be able to generate a 2D representation of the map.  

![Demo Map](./img/map.PNG)  

The above demo map has the following characteristics 
* 4 Vertexes
  * vertex 1 at (10,10)
  * vertex 2 at (10,100)
  * vertex 3 at (100, 10)
  * vertex 4 at (100,100)
  
* 4 lines
  * line from vertex 1 to 2
  * line from vertex 1 to 3
  * line from vertex 2 to 4
  * line from vertex 3 to 4

### Vertex Format
The vertexes data is simple as you would expect, just the x, and y (a point), on some coordinates.
  
| Field Size | Data Type    | Content    |  
|------------|--------------|------------| 
|  0x00-0x01 | Signed short | X Position |
|  0x02-0x03 | Signed short | Y Position |
  
### Linedef Format
Linedef seems to have more information in it, it shows a line that connect two vertexes, and properties to that line (which later will be a wall).
  
| Field Size | Data Type      | Content                                   |  
|------------|----------------|-------------------------------------------| 
|  0x00-0x01 | Unsigned short | Start vertex                              |
|  0x02-0x03 | Unsigned short | End vertex                                |
|  0x04-0x05 | Unsigned short | Flags (details below)                     |
|  0x06-0x07 | Unsigned short | Line type / Action                        |
|  0x08-0x09 | Unsigned short | Sector tag                                |
|  0x0A-0x0B | Unsigned short | Right sidedef ( 0xFFFF side not present ) |
|  0x0C-0x0D | Unsigned short | Left sidedef  ( 0xFFFF side not present ) |
  
### Linedef Flags Values
Note: not all linedefs are meant to be drawn on the automap. Some have special behaviors and automap flags.  
  
| Bit | Description                                          |
|-----|------------------------------------------------------|
|  0  | Blocks players and monsters                          |
|  1  | Blocks monsters                                      |
|  2  | Two sided                                            |
|  3  | Upper texture is unpegged (will research this later) |
|  4  | Lower texture is unpegged (will research this later) |
|  5  | Secret (shows as one-sided on automap)               |
|  6  | Blocks sound                                         |
|  7  | Never shows on automap                               |
|  8  | Always shows on automap                              |
  
## Goals
1. Create a Map class.  
2. Read vertex data.  
3. Read linedef data.  
  
## Design
First, let’s create a class and call it `Map`, where we will store all the map-related data.  
For now, I have plans to just store the vertexes and linedefs in a vector, so we would try to put them to some use later.  
Let’s also update WADLoader and WADReader to help us achieve reading this two new information.
  
## Coding
Coding this should be like what we did in Notes 001: add a few more structures, then populate them with data from the WAD.  
Starting by adding a new class and will be passing the name of the map.  
  
``` cpp
class Map
{
public:
    Map(std::string sName);
    ~Map();

    std::string GetName(); // In case someone needs to know the map name
    void AddVertex(Vertex &v); // Wrapper class to append to the vertexes vector
    void AddLinedef(Linedef &l); // Wrapper class to append to the linedef vector

protected:
    std::string m_sName;
    std::vector<Vertex> m_Vertexes;
    std::vector<Linedef> m_Linedef;
};
```
  
Now let’s add structs so we can read those new fields. Since we have already done this a few times, let’s add both at once this time.  
  
``` cpp
struct Vertex
{
    int16_t XPosition;
    int16_t YPosition;
};
  
struct Linedef
{
    uint16_t StartVertex;
    uint16_t EndVertex;
    uint16_t Flags;
    uint16_t LineType;
    uint16_t SectorTag;
    uint16_t RightSidedef;
    uint16_t LeftSidedef;
};
```
  
Now we will need a function to read those from the WADReader, this is close to what we did the other day.  
  
``` cpp
void WADReader::ReadVertexData(const uint8_t *pWADData, int offset, Vertex &vertex)
{
    vertex.XPosition = Read2Bytes(pWADData, offset);
    vertex.YPosition = Read2Bytes(pWADData, offset + 2);
}
  
void WADReader::ReadLinedefData(const uint8_t *pWADData, int offset, Linedef &linedef)
{
    linedef.StartVertex = Read2Bytes(pWADData, offset); 
    linedef.EndVertex = Read2Bytes(pWADData, offset + 2);
    linedef.Flags = Read2Bytes(pWADData, offset + 4);
    linedef.LineType = Read2Bytes(pWADData, offset + 6);
    linedef.SectorTag = Read2Bytes(pWADData, offset + 8);
    linedef.RightSidedef = Read2Bytes(pWADData, offset + 10);
    linedef.LeftSidedef = Read2Bytes(pWADData, offset + 12);
}
```
  
I don't think there is anything new up to this point, and as you guessed by now, we need to call those functions from the `WADLoader` class.  

Let me state some facts, the sequence of the lumps does matter, we will find the map name in the directory lump, then all map-related lumps will follow in the given order. To make things easier for us and not to keep track of lumps index individually I have added an enum to avoid magic numbers.
  
``` cpp
enum EMAPLUMPSINDEX
{
    eTHINGS = 1,
    eLINEDEFS,
    eSIDEDDEFS,
    eVERTEXES,
    eSEGS,
    eSSECTORS,
    eNODES,
    eSECTORS,
    eREJECT,
    eBLOCKMAP,
    eCOUNT
};
```
  
Also, I will be adding a function to find the map by name in the directory list. We will likely improve performance for this later by using a map data structure instead as there are a significant number of entries and we will have to iterate through them quite frequently, particularly when we start loading media such as textures, sprites, sounds, and so forth.
  
``` cpp
int WADLoader::FindMapIndex(Map &map)
{
    for (int i = 0; i < m_WADDirectories.size(); ++i)
    {
        if (m_WADDirectories[i].LumpName == map.GetName())
        {
            return i;
        }
    }

    return -1;
}
```
  
We’re almost done! Let’s read `VERTEXES` now. Again, we’ve done the same kind of thing before you’re a pro by now.  
  
``` cpp
bool WADLoader::ReadMapVertex(Map &map)
{
    int iMapIndex = FindMapIndex(map);

    if (iMapIndex == -1)
    {
        return false;
    }

    iMapIndex += EMAPLUMPSINDEX::eVERTEXES;

    if (strcmp(m_WADDirectories[iMapIndex].LumpName, "VERTEXES") != 0)
    {
        return false;
    }

    int iVertexSizeInBytes = sizeof(Vertex);
    int iVertexesCount = m_WADDirectories[iMapIndex].LumpSize / iVertexSizeInBytes;

    

    Vertex vertex;
    for (int i = 0; i < iVertexesCount; ++i)
    {
        m_Reader.ReadVertexData(m_WADData, m_WADDirectories[iMapIndex].LumpOffset + i * iVertexSizeInBytes, vertex);

        map.AddVertex(vertex);

        cout << vertex.XPosition << endl;
        cout << vertex.YPosition << endl;
        std::cout << std::endl;
    }

    return true;
}
``` 
  
Hmm, this feels like copy/pasting code over and over. We’ll refactor later, but for now you can implement `ReadMapLinedefs` yourself (or look at the provided source code).  
  
Final touches, we need to call this function passing a map object to it.
  
``` cpp
bool WADLoader::LoadMapData(Map &map)
{
    if (!ReadMapVertex(map))
    {
        cout << "Error: Failed to load map vertex data MAP: " << map.GetName() << endl;
        return false;
    }

        if (!ReadMapLinedef(map))
    {
        cout << "Error: Failed to load map linedef data MAP: " << map.GetName() << endl;
        return false;
    }
    return true;
}
```
  
Now let’s update `main` and see if things run. I want to load the map "E1M1", which I’ll pass to the `Map` object.  
  
``` cpp
    Map map("E1M1");
    wadloader.LoadMapData(map);
```
  
Now let’s run this! Wow! lots of cool numbers, but are they correct? Let’s check!  
let’s see if slade can help us here again.
  
We can locate the map in the slade menu and look at the lump’s details, let’s compare numbers.
![Vertex](./img/vertex.PNG)  
  
Looks good!
What about the Linedef?
![Linedef](./img/linedef.PNG)  
  
I have also added this enum mostly we will try to utilize this when we try to draw the map.

``` cpp
enum ELINEDEFFLAGS
{
    eBLOCKING      = 0,
    eBLOCKMONSTERS = 1,
    eTWOSIDED      = 2,
    eDONTPEGTOP    = 4,
    eDONTPEGBOTTOM = 8,
    eSECRET        = 16,
    eSOUNDBLOCK    = 32,
    eDONTDRAW      = 64,
    eDRAW          = 128
};
```

## Other Notes
While I was writing this code, I mistakenly was reading more bytes than I should have, and I was getting wrong values. To debug this, I started looking at the WAD offset in memory to see if I'm at the correct offset. This can be accomplished by visual studio memory windows which I find a very handy tool when it comes to byte or memory tracing (you can also place break point in that window).  
  
If you don’t see the Memory window, go to Debug > Windows > Memory.  
  
![Memory](./img/memory.PNG)  
  
Now you can see the memory values in hexadecimal.  
Those values can be compared by the hex view in slade, where you can right click any lump and view it as hex.  
  
![Slade](./img/hex.PNG)  
  
Comparing both with the address of the WAD loaded in memory.  
  
![Debug](./img/debug.png)  

One final thing for today, we have seen all those vertexes numbers, is there an easy way to plot those without writing the code, I don’t want to invest time then find out we were not working in the right direction.  
For sure someone out there implemented a graph plotter. Surely, I googled "draw points on a graph" and the first result was a website called [Plot Points - Desmos](https://www.desmos.com/calculator), you can paste from your clipboard with multiple points and it should draw them for you. We will need to format as following "(x, y)". That is easy just let’s change our print function to do that for us.  

``` cpp
cout << "("  << vertex.XPosition << "," << vertex.YPosition << ")" << endl;
```
  
Wow! That is a resemblance of E1M1! We are up to something!  
  
![E1M1 Plot Points](./img/plot.PNG) 

If you're too lazy to do that here is a link with the vertex being populated  

[Plot Vertex](https://www.desmos.com/calculator/jatccysan2)

But let’s step it up, with a little bit of work we can connect those points given the linedefs.  

![E1M1 Plot Vertex](./img/plotvertex.PNG) 

Here is a link

[E1M1 Plot Vertex](https://www.desmos.com/calculator/3mle5kuzsi)  

## Source code
[Source code](../src)    

## Reference
[Doom Wiki](https://doomwiki.org/wiki/WAD)  
[ZDoom Wiki](https://zdoom.org/wiki/WAD)  
 
