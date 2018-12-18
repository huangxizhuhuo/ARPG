/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#ifndef CHARICS_H
#define CHARICS_H

typedef struct sCharItem
{ 
	long      ItemNum;          // MIL item number
    long      Quantity;         // Quantity of item (ie coins)
    sCharItem  *Prev, *Next;    // linked list pointers
    long       Index;           // This items index #
    long       Owner;           // Owner index #
    sCharItem  *Parent;         // Parent of a contained item
    long       Position;        //该物品位于框格的位置
    long       UpDown;          //该物品位于道具栏还是快捷栏
    sCharItem()
    { 
        Prev = Next = Parent = NULL;
        Index = 0; Owner = -1;   
	}   
    ~sCharItem() { delete Next; } 
} sCharItem;
class CCharICS
{ 
private:
    long      m_NumItems;     // # items in inventory
    sCharItem *m_ItemParent;  // Linked list parent item  
    long  GetNextLong(FILE *fp);// Functions to read in next long or float # in file
    float GetNextFloat(FILE *fp);
public:
    CCharICS();   // Constructor
    ~CCharICS();  // Destructor   
    BOOL Load(char *Filename);// Load, save, and free a list of items
    BOOL Save(char *Filename);
    BOOL Free();
    // Add and remove an item
    BOOL Add(long ItemNum, long Quantity, long Position,sCharItem *OwnerItem = NULL,long UpDown=0);             
    BOOL Remove(sCharItem *Item);  
    long      GetNumItems();// Retrieve # items or parent linked list object
    sCharItem *GetParentItem();
    sCharItem *GetItem(long Num);  
    BOOL Arrange();// Re-ordering functions
    BOOL MoveUp(sCharItem *Item);
    BOOL MoveDown(sCharItem *Item);
};
#endif
