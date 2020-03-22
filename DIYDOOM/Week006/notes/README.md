# Week 006 - Intro to DOOM BSP Traversal
Probably you have heard that Doom rendering uses BSP (Binary Space Partition trees). Moving directly into BSP might be a big jump, so let's break things down a little.  

The core of BSP is a binary trees data structure, so let's start from there.  

## Goals
1. Understanding Binary Trees.  
2. Minor code cleanup and refactor.  
  
## Design
So today will be theory. 

A "data structure" is how you organize data in memory. Choosing how to organize data could make all the difference for your project. A good example is how DOOM would have not been possible to run back in 1993 without the BSP implementation to store the map data (we get into details later).  

So, what makes a tree data-structure special? Let's think a bit and clarify a few things with examples.  
So, a binary tree has a parent node and two branches, a left and right child. Every time you look at a node you get the option to either traverse the left or the right child. It is as simple as that.  

A simple tree would look like!  
![Simple Tree](./img/simpletree.PNG) 

and in code

``` cpp
class CTreeNode
{
public:
    CTreeNode(int iData);
    ~CTreeNode(void);

    int m_iData;
    CTreeNode* m_pLeft;
    CTreeNode* m_pRight;
};
```
_Not in source_

With no constraints at all, you can just stuff data anywhere in the tree, and you will struggle to retrieve it since we don't know where to look for the data. (I.e.., Should you look at the left children or right children?)
This is where the "Binary search tree" comes into play. (Not every binary tree is a binary search tree).

A binary search tree (AKA sorted binary tree) has its right child always less than its parent, and its left child equal or greater than its parent. With such a property, if you are looking up a number, then every time you make a choice at a sub-tree node you discard half of the tree. That makes trees powerful to look up values. Imagine a tree with one million nodes. You will need only twenty checks (in the worst-case scenario) to find your data (Log(1,000,000) log here is base 2 because every node has 2 children). Compare this to a linear search which could take approximately one million checks to find your data.

This is how we would implement recursive binary tree search algorithm:

``` cpp
void CBinarySearchTree:printFindNumber(CTreeNode* pNode, int iData)
{
    // Is this a valid node?
    if (!pNode)
        return;

    // Is this what I'm looking for?
    if (pNode->m_iData == iData)
    {
       PrintData(pNode->m_iData);
       return;
    }

    // I didn't find what I'm looking for :(, where should I look?
    if (pNode->m_iData < iData)
    {
        printFindNumber(pNode->m_pRight, rStream, iData);
    }
    else
    {
        printFindNumber(pNode->m_pLeft, rStream, iData);
    }
}
```
_Not in source_

So how does this algorithm work? It is simple, but let's break it down into steps.
We first pass the root of the tree (the root node) to the function and then do the following:
1. We validate we are on a valid node. If Yes, continue onward. If No, exit function as the element was not found in this subtree.
2. Does the current node contain the data value we are looking for? If Yes, we have found our node and are done.  If not...
3. Compare the element to the current node's data value. Go left or right and go back to step 1.  

Let's give a quick search example.  
Given this tree  

![BSP Sample](./img/bsptree.PNG) 
  
Searching for the value 30 using the above algorithm could be visualized like this:

![BSP Sample](./img/treesearch.gif) 

That was straight forward!  
Now let's get this a little complex.  
I need to change this algorithm to retrieve the closest number and not an exact number. With that given, we need to find other criteria to select a valid node.  One possible solution is to put my data leaf nodes and the parent's nodes just a guide for a range.  
The tree is a custom built tree, my actual data is 10, 20, 30, 40, 50, 60, 70, 80. The rest of the tree (any number with 5 in its ones digit) was just added to help me in my search and is not part of the data I care about.   

So, let's replace this piece of code

``` cpp
if (pNode->m_iData == iData)
```

with this

``` cpp
if (!pNode->m_pRight && !pNode->m_pLeft)
```

With this change to the search condition, we are not looking for an equal value anymore, we are looking for a leaf node. What does that mean? The input values (the value to search for) will be our guide until we find a leaf node.    

The tree hasn't changed, but now we have added a constraint that our data should be leaf nodes, and all other nodes are just a guide for us to go left or right.  
The code should look like this now  

``` cpp
void CBinarySearchTree::printCloseNumbers(CTreeNode* pNode, int iData)
{
    if (!pNode)
        return;

    if (!pNode->m_pRight && !pNode->m_pLeft)
    {
        PrintData(pNode->m_iData);
    }

    if (pNode->m_iData < iData)
    {
        printFindNumber(pNode->m_pRight, iData);
    }
    else
    {
        printFindNumber(pNode->m_pLeft, iData);
    }
}
```
_Not in source_

If you re-run the code, with 30 as input, it will find it. Not only that, but it will also output 30 if you look for 31, 32, 33, and 34 because they are the closes value to 30. So, now we know that you don’t have to know the exact value of what you're looking for, just a range will give you a correct value you're looking for.  

Note: You might think "How is this related to DOOM?". The map is broken down to subsector, you have no idea which subsector the player is in, but you know the player position, we can use that to find out which subsector the player is in (if this sounds too complicated don't worry we will go in details and visualize this later).

With this modified algorithm, we can do a nice trick, we can print (custom sort) the nodes. We can print the nodes from closer in range to further (or the other way around).  
With a little modification, we can retrieve the closest numbers to the one I'm looking for, it is more of a custom kind of sorting.  


``` cpp
void CBinarySearchTree::printCloseNumbers(CTreeNode* pNode, int iData)
{
    if (!pNode)
        return;

    if (!pNode->m_pRight && !pNode->m_pLeft)
    {
        PrintData(pNode->m_iData);
    }

    if (pNode->m_iData < iData)
    {
        printFindNumber(pNode->m_pRight, iData);
        printFindNumber(pNode->m_pLeft, iData);
    }
    else
    {
        printFindNumber(pNode->m_pLeft, iData);
        printFindNumber(pNode->m_pRight, iData);
    }
}
```
_Not in source_

List (sort) all the data that is close to the number 30.  
Running this algorithm will output the following:  
30 , 40 , 20 , 10 , 50 , 60 , 70 , 80  

Running with 70 as input:  
70 , 80 , 60 , 50 , 40 , 30 , 20 , 10  

Running with 10 as input:  
10 , 20 , 30 , 40 , 50 , 60 , 70 , 80  

Running with 80 as input:   
80 , 70 , 60 , 50 , 40 , 30 , 20 , 10  

Notice the result is just listing the neighbors from closest to furthest (relative to the number we are looking up). The Tree has not been modified in any way; it is only our input that changes.  

This is how BSP works, but instead of numbers it is splitting 2D or 3D space.  
But, why does this algorithm work?  What is the logic behind it? The simple answer is after the algorithm finds the leaf closest to our value, you force it to search in the "wrong" branch. When you apply the search on the "wrong" branch it will find the closest value to what you're looking for. Now just keep doing this for all the branches you took a decision at.
The advanced answer is, this is a recursive algorithm, the memory stack has the path how we got to that node, just move in the opposite direction and print those nodes.
You will find some books discussing this as a hybrid Pre-Post order tree traversal, I personally find it a modified binary search algorithm.

## Coding
There is no major code change, just some refactoring to make things a little easier for now.
Update DoomEngine to store a copy for the Renderer and remove player and map to Init function.

``` cpp
DoomEngine::DoomEngine(SDL_Renderer *pRenderer) : m_pRenderer(pRenderer), m_bIsOver(false), m_iRenderWidth(320), m_iRenderHeight(200)
{
}
```

Update Init to create the player and the map

``` cpp
bool DoomEngine::Init()
{
    // Delay object creation to this point so renderer is initialized
    m_pPlayer = new Player(1);
    m_pMap = new Map(m_pRenderer, "E1M1", m_pPlayer);

    m_WADLoader.SetWADFilePath(GetWADFileName());
    m_WADLoader.LoadWAD();

    m_WADLoader.LoadMapData(m_pMap);
    return true;
}
```

No need to pass "SDL_Renderer *pRenderer" anymore, the map has its own copy.  

``` cpp
void DoomEngine::Render()
{
    SDL_SetRenderDrawColor(m_pRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(m_pRenderer);
    m_pMap->RenderAutoMap();
}
```

Adding a helper function RemapXToScreen, it is just to make things look cleaner and easier to read.  

``` cpp
int Map::RemapXToScreen(int XMapPosition)
{
    return (XMapPosition + (-m_XMin)) / m_iAutoMapScaleFactor;
}
```

## Other Notes
Lots of things in computer science are just big words for small things ($25 term for a five-cent concept) like cloud computing or dependency injection.  Sadly, BSP is one of them. What is stated above is how BSP search algorithms work.  

## Source code
[Source code](../src)  

## Reference
[Binary tree](https://en.wikipedia.org/wiki/Binary_tree)  
[Binary tree (Stanford)](http://cslibrary.stanford.edu/110/BinaryTrees.html)  
[Binary tree traversal](https://en.wikipedia.org/wiki/Tree_traversal)  

