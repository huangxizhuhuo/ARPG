/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include "CharICS.h"

CCharICS::CCharICS()
{
    m_NumItems = 0;
    m_ItemParent = NULL;
}

CCharICS::~CCharICS()
{
    Free();
}

BOOL CCharICS::Load(char *Filename)
{
    FILE *fp;
    long LongNum;
    sCharItem *Item, *ItemPtr = NULL;
    Free();  // Free a prior set  
    if((fp=fopen(Filename, "rb"))==NULL)// Open the file
        return FALSE; 
    while(1)// Loop forever reading in items 
	{
        // Get next item number (break if no more items,which is represented by a return value of -1).
        if((LongNum = GetNextLong(fp)) == -1)
            break;
        // Create a new item pointer and link it in
        Item = new sCharItem();
        if(ItemPtr == NULL)
            m_ItemParent = Item;
        else
		{
            Item->Prev = ItemPtr;
            ItemPtr->Next = Item;
        }
        ItemPtr = Item;		
		Item->ItemNum = LongNum;// Store MIL item number	
		Item->Quantity = GetNextLong(fp);// Get quantity	
		Item->Owner = GetNextLong(fp);// Get owner #				
		Item->Position=GetNextLong(fp);//得到位于框格的位置(0-29..)
		Item->UpDown=GetNextLong(fp);//该物品位于道具栏还是快捷栏(0-1)	   
		Item->Index = m_NumItems++;// Save index # and increase count
	} 
	fclose(fp);// Close the file

	ItemPtr = m_ItemParent;// Match objects that belong to others
	while(ItemPtr != NULL)
	{	
		if(ItemPtr->Owner != -1)// Check if this item belongs to another 
		{			     
			Item = m_ItemParent;// Find matching parent item   
			while(Item != NULL) 
			{      
				if(ItemPtr->Owner == Item->Index) 
				{         					        
					ItemPtr->Parent = Item;// A match, point to parent         
					break; // Stop scanning for parents      
				}      
				Item = Item->Next;     
			} 
		}
		ItemPtr = ItemPtr->Next;// Go to next item
	}
	return TRUE;
}
BOOL CCharICS::Save(char *Filename)
{
    FILE *fp;
    sCharItem *Item;
    long Index = 0;  
    if((fp=fopen(Filename, "wb"))==NULL)// Open the file
        return FALSE; 
    if((Item = m_ItemParent) == NULL)// Assign index numbers to items 
    {
        fclose(fp);
        return TRUE; // no items to save
    }
    while(Item != NULL)//索引（Index）是不保存到文件的，只是临时计算并使用之 
    {
        Item->Index = Index++;
        Item = Item->Next;
    }
    // Match child items to parents
    Item = m_ItemParent;
    while(Item != NULL) 
    {
        if(Item->Parent != NULL)
            Item->Owner = Item->Parent->Index;
        else
            Item->Owner = -1;
        Item = Item->Next;
    }
    // Save item out
    Item = m_ItemParent;
    while(Item != NULL) 
    {      
        fprintf(fp, "%lu\r\n", Item->ItemNum);// Item number   
        fprintf(fp, "%lu\r\n", Item->Quantity);// Quantity      
        fprintf(fp, "%ld\r\n", Item->Owner);// Owner #    
	    fprintf(fp, "%ld\r\n", Item->Position);//位置
	    fprintf(fp, "%ld\r\n", Item->UpDown);//上下    
        Item = Item->Next;// Next item
	}
	fclose(fp); // Close the file
	return TRUE; // Return success!
}
BOOL CCharICS::Free()
{
    m_NumItems = 0;
    delete m_ItemParent;
    m_ItemParent = NULL;
    return TRUE;
}
BOOL CCharICS::Add(long ItemNum, long Quantity,long Position,sCharItem *OwnerItem,long UpDown)                 
{
    sCharItem *Item;   
    Item = new sCharItem();// Create a new item structure  
    Item->Next = m_ItemParent;// Insert into top of list
    if(m_ItemParent != NULL)
        m_ItemParent->Prev = Item;   
	m_ItemParent = Item;   
	// Fill the item structure
    Item->ItemNum  = ItemNum;
    Item->Quantity = Quantity;
    Item->Parent   = OwnerItem;
    Item->Position = Position;
    Item->UpDown   = UpDown;  
    m_NumItems++;// Increate # of items 
    return TRUE;
}

BOOL CCharICS::Remove(sCharItem *Item)
{
    sCharItem *ItemPtr, *NextItem;  
    if((ItemPtr = m_ItemParent) != NULL)// Remove child objects first
	{ 
		while(ItemPtr != NULL)
		{ 
			NextItem = ItemPtr->Next;//a temp pointer for storing ItemPtr     
			if(ItemPtr->Parent == Item)       
				Remove(ItemPtr);//这是一个递归的过程     
			ItemPtr = NextItem; 
		}
	}
	// Remove from linked list and reset root if it's the current head of list.
	if(Item->Prev != NULL) 
		Item->Prev->Next = Item->Next;
	else 
		m_ItemParent = Item->Next;
	if(Item->Next != NULL) 
		Item->Next->Prev = Item->Prev;	
	Item->Prev = Item->Next = NULL;// Clear link list	
	m_NumItems--;// Remove an item
	delete Item;// Free memory
	return TRUE;
}

long CCharICS::GetNumItems()
{
    return m_NumItems;
}

sCharItem *CCharICS::GetParentItem()
{
    return m_ItemParent;
}

sCharItem *CCharICS::GetItem(long Num)
{
    sCharItem *Item;
    Item = m_ItemParent;
    while(Num--)// Loop until reached item num 
	{   
		if(Item == NULL)  
			return NULL;
		Item = Item->Next;
	}
	return Item;
}

BOOL CCharICS::Arrange()
{
	sCharItem *Item, *PrevItem;
	// Start at top of linked list and float each item up that has a lesser ItemNum.
	// Break if past bottom of list
	Item = m_ItemParent;
	while(Item != NULL) 
	{	
		if(Item->Prev != NULL)// Check previous item to float up 
		{
			// Keep floating up while prev item has a lesser ItemNum value 
			// or until top of list has been reached.   
			while(Item->Prev != NULL)
			{       
				PrevItem = Item->Prev;  // Get prev item pointer				       
				if(Item->ItemNum >= PrevItem->ItemNum)// Break if no more to float up         
					break;       				       
				if((PrevItem = Item->Prev) != NULL)// Swap Item and PrevItem, 6 pointers should be modified 
				{       
					if(PrevItem->Prev != NULL)          
						PrevItem->Prev->Next = Item;      
					if((PrevItem->Next = Item->Next) != NULL)
						Item->Next->Prev = PrevItem;     
					if((Item->Prev = PrevItem->Prev) == NULL)           
						m_ItemParent = Item;         
					PrevItem->Prev = Item;         
					Item->Next = PrevItem;       
				}      
			}//end while 
		}		
		Item = Item->Next;// Go to next object 
	}//end while
	return TRUE;
}

BOOL CCharICS::MoveUp(sCharItem *Item)
{
	sCharItem *PrevItem;
	if((PrevItem = Item->Prev) != NULL)// Swap item and item before it 
	{
		if(PrevItem->Prev != NULL) 
			PrevItem->Prev->Next = Item;
		if((PrevItem->Next = Item->Next) != NULL)    
			Item->Next->Prev = PrevItem;
		if((Item->Prev = PrevItem->Prev) == NULL)     
			m_ItemParent = Item;
		PrevItem->Prev = Item;  
		Item->Next = PrevItem;
	}
	return TRUE; // Return success
}

BOOL CCharICS::MoveDown(sCharItem *Item)
{
	sCharItem *NextItem;	
	if((NextItem = Item->Next) != NULL)// Swap item and item after it 
	{  
		if((Item->Next = NextItem->Next) != NULL)    
			NextItem->Next->Prev = Item;   
		if((NextItem->Prev = Item->Prev) != NULL)   
			Item->Prev->Next = NextItem;
		else  
			m_ItemParent = NextItem;
		NextItem->Next = Item;  
		Item->Prev = NextItem;
	}
	return TRUE; // Return success
}

long CCharICS::GetNextLong(FILE *fp)
{
    char Buf[1024];
    long Pos = 0;
    int c; 
    while(1)// Read until EOF or EOL 
	{  
		if((c = fgetc(fp)) == EOF)     
			break;  
		if(c == 0x0a)  
			break;   
		if((c >= '0' && c <= '9') || c == '.' || c == '-')      
			Buf[Pos++] = c; 
	}
	if(!Pos) 
		return -1;
	Buf[Pos] = 0;
	return atol(Buf);
}

float CCharICS::GetNextFloat(FILE *fp)
{
	char Buf[1024];
	long Pos = 0;
	int c;	
	while(1)// Read until EOF or EOL
	{
		if((c = fgetc(fp)) == EOF)    
			break;
		if(c == 0x0a)    
			break;  
		if((c >= '0' && c <= '9') || c == '.' || c == '-')  
			Buf[Pos++] = c;
	}
	Buf[Pos] = 0;
	return (float)atof(Buf);
}
