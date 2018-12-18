/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#ifndef TRIGGER_H
#define TRIGGER_H

// An enum for type of triggers
enum TriggerTypes { Trigger_Sphere = 0, Trigger_Box,Trigger_Cylinder,   Trigger_Triangle };          
                    
typedef struct sTrigger 
{
    long Type;         // Sphere, Box, etc
    long ID;           // Trigger ID
    BOOL Enabled;      // Flag if enabled

    float x1, y1, z1;  // Coord 1
    float x2, y2, z2;  // Coord 2
    float x3, z3;      // Coord 3
    float Radius;      // Radius of bounds
  
    sTrigger *Prev, *Next; // Linked list of triggers

    sTrigger()  { Prev = Next = NULL; }
    ~sTrigger() { delete Next; Next = NULL; }
} sTrigger;

class CTrigger
{ 
private:
    long      m_NumTriggers;    //# triggers in linked list
    sTrigger *m_TriggerParent;  // Linked list parent

    long      GetNextLong(FILE *fp);  // Get next long
    float     GetNextFloat(FILE *fp); // Get next float

    // Function that adds a trigger to linked list
    sTrigger *AddTrigger(long Type, long ID, BOOL Enabled);

public:
    CTrigger();
    ~CTrigger();

    // Functions to load/save a trigger list
    BOOL Load(char *Filename);
    BOOL Save(char *Filename);

    // Functions to add a specific trigger to list
    BOOL AddSphere(long ID, BOOL Enabled,float XPos, float YPos, float ZPos,float Radius);                                                                
    BOOL AddBox(long ID, BOOL Enabled,float XMin, float YMin, float ZMin,float XMax, float YMax, float ZMax);                                                                 
    BOOL AddCylinder(long ID, BOOL Enabled,float XPos, float YPos, float ZPos,float Radius, float Height);                                                              
    BOOL AddTriangle(long ID, BOOL Enabled,float x1, float z1,
		float x2, float z2,float x3, float z3,float YPos, float Height);		                                                                                                                                                                        
    BOOL Remove(long ID);// Remove a specific trigger by ID   
    BOOL Free();// Free all triggers
    // Find first trigger at location (returns 0 if none)
    long GetTrigger(float XPos, float YPos, float ZPos);  
    BOOL GetEnableState(long ID); // Get state of trigger by ID  
    BOOL Enable(long ID, BOOL Enable);// Enabled/Disable a trigger by ID
    // Return the # of triggers and parent of linked list
    long GetNumTriggers();
    sTrigger *GetParentTrigger();
};

#endif
