/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#include "../Core/Core_Global.h"
#include "Trigger.h"

CTrigger::CTrigger()
{
    m_NumTriggers = 0;
    m_TriggerParent = NULL;
}

CTrigger::~CTrigger()
{
    Free();
}

long CTrigger::GetNextLong(FILE *fp)
{
    char Buf[1024];
    long Pos = 0;
    int c;
    while(1)// Read until EOF or EOL
    {
        if((c = fgetc(fp)) == EOF)
            break;
        if(c == 0x0a || (c == ' ' && Pos))
            break;
        if((c >= '0' && c <= '9') || c == '.' || c == '-')
            Buf[Pos++] = c;
    }
    if(!Pos)
        return -1;
    Buf[Pos] = 0;
    return atol(Buf);
}

float CTrigger::GetNextFloat(FILE *fp)
{
    char Buf[1024];
    long Pos = 0;
    int c; 
    while(1)// Read until EOF or EOL 
    {
        if((c = fgetc(fp)) == EOF)
            break;
        if(c == 0x0a || (c == ' ' && Pos))
            break;
        if((c >= '0' && c <= '9') || c == '.' || c == '-')
            Buf[Pos++] = c;
    }
    Buf[Pos] = 0;
    return (float)atof(Buf);
}

BOOL CTrigger::Load(char *Filename)
{
    FILE *fp;
    long Type, ID;
    BOOL Enabled;
    float x1, y1, z1, x2, y2, z2, x3, z3, Radius;    
    Free();// Remove all current triggers  
    if((fp=fopen(Filename, "rb"))==NULL)// Open file
        return FALSE;   
    while(1)// Start looping, reading in until EOF reached 
    {        
        if((ID = GetNextLong(fp)) == -1)// Get ID of trigger
            break; 
        Type = GetNextLong(fp); // Get type    
        Enabled = (GetNextLong(fp)) ? TRUE : FALSE;// Get enabled status        
        switch(Type)// Read in rest depending on type
	    {     
		case Trigger_Sphere:    
			x1 = GetNextFloat(fp);    
			y1 = GetNextFloat(fp);    
			z1 = GetNextFloat(fp);      		
			Radius = GetNextFloat(fp);     
			AddSphere(ID, Enabled, x1, y1, z1, Radius);     
			break;     
		case Trigger_Box:      
			x1 = GetNextFloat(fp);     
			y1 = GetNextFloat(fp);     
			z1 = GetNextFloat(fp);      
			x2 = GetNextFloat(fp);     
			y2 = GetNextFloat(fp);   
			z2 = GetNextFloat(fp);    
			AddBox(ID, Enabled, x1, y1, z1, x2, y2, z2);   
			break;     
		case Trigger_Cylinder:  
			x1 = GetNextFloat(fp);    
			y1 = GetNextFloat(fp);      
			z1 = GetNextFloat(fp);    
			Radius = GetNextFloat(fp);  
			y2 = GetNextFloat(fp);      
			AddCylinder(ID, Enabled, x1, y1, z1, Radius, y2);   
			break;        
		case Trigger_Triangle:     
			x1 = GetNextFloat(fp);    
			z1 = GetNextFloat(fp);      
			x2 = GetNextFloat(fp);    
			z2 = GetNextFloat(fp);    
			x3 = GetNextFloat(fp);  
			z3 = GetNextFloat(fp);      
			y1 = GetNextFloat(fp);   
			y2 = GetNextFloat(fp);   
			AddTriangle(ID, Enabled, x1, z1, x2, z2, x3, z3, y1, y2);      
			break;    
		default: 		  
			fclose(fp);    // Some error occurred         
			Free();        
			return FALSE; 
		}  
	} 
	fclose(fp);
	return (m_NumTriggers) ? TRUE : FALSE;
}

BOOL CTrigger::Save(char *Filename)
{
	FILE *fp; 
	sTrigger *TriggerPtr;
	if(!m_NumTriggers)
		return FALSE;
	if((TriggerPtr = m_TriggerParent) == NULL)   
		return FALSE; 
	if((fp=fopen(Filename, "wb"))==NULL)// Open file  
		return FALSE;
	while(TriggerPtr != NULL)// Write out all triggers
	{
		// Write out ID and Type of trigger   
		fprintf(fp, "%lu ", TriggerPtr->ID);   
		fprintf(fp, "%lu ", TriggerPtr->Type); 
		// Write out Enabled flag 
		fprintf(fp, "%lu ", (TriggerPtr->Enabled) ? 1 : 0); 
		// Write out remaining data depending on type
		switch(TriggerPtr->Type) 
		{  
		case Trigger_Sphere:     
			fprintf(fp, "%lf %lf %lf %lf\r\n",TriggerPtr->x1, TriggerPtr->y1, TriggerPtr->z1,TriggerPtr->Radius);                                  
			break;      
		case Trigger_Box:     
			fprintf(fp, "%lf %lf %lf %lf %lf %lf\r\n", TriggerPtr->x1, 
			TriggerPtr->y1, TriggerPtr->z1,TriggerPtr->x2, TriggerPtr->y2, TriggerPtr->z2);                                    
			break;    
		case Trigger_Cylinder:   
			fprintf(fp, "%lf %lf %lf %lf %lf\r\n",TriggerPtr->x1, 
				TriggerPtr->y1, TriggerPtr->z1,TriggerPtr->Radius, TriggerPtr->y2);                                   
			break;   
		case Trigger_Triangle:     
			fprintf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf\r\n", TriggerPtr->x1,TriggerPtr->z1, 
				TriggerPtr->x2, TriggerPtr->z2,TriggerPtr->x3, TriggerPtr->z3,TriggerPtr->y1, TriggerPtr->y2);                                                                
			break;  
		}
	}  
	fclose(fp); 
	return TRUE;
}

sTrigger *CTrigger::AddTrigger(long Type, long ID, BOOL Enabled)
{	
	sTrigger *Trigger = new sTrigger();// Allocate a new trigger structure and link in
	Trigger->Prev = NULL;
	if((Trigger->Next = m_TriggerParent) != NULL)  
		m_TriggerParent->Prev = Trigger;
	m_TriggerParent = Trigger;
	// Set trigger Type, ID, and Enabled flag
	Trigger->Type = Type;
	Trigger->ID = ID;
	Trigger->Enabled = Enabled;
	m_NumTriggers++;// Increase trigger count
	return Trigger;
}

BOOL CTrigger::AddSphere(long ID, BOOL Enabled,float XPos, float YPos, float ZPos, float Radius)                                                                
{
	// Create a new trigger structure and link in 
	sTrigger *Trigger = AddTrigger(Trigger_Sphere, ID, Enabled);
	// Setup trigger data
	Trigger->x1 = XPos;
	Trigger->y1 = YPos;
	Trigger->z1 = ZPos;
	Trigger->Radius = Radius * Radius;
	return TRUE;
}

BOOL CTrigger::AddBox(long ID, BOOL Enabled,float XMin, float YMin, float ZMin,float XMax, float YMax, float ZMax)                                                                 
{
	// Create a new trigger structure and link in 
    sTrigger *Trigger = AddTrigger(Trigger_Box, ID, Enabled);
    // Setup trigger data (fix for min/max values)
    Trigger->x1 = min(XMin, XMax);
    Trigger->y1 = min(YMin, YMax);
    Trigger->z1 = min(ZMin, ZMax);
    Trigger->x2 = max(XMin, XMax);
    Trigger->y2 = max(YMin, YMax);
    Trigger->z2 = max(ZMin, ZMax);
    return TRUE;
}

BOOL CTrigger::AddCylinder(long ID, BOOL Enabled,float XPos, float YPos, float ZPos,float Radius, float Height)                                                             
{
	// Create a new trigger structure and link in 
    sTrigger *Trigger = AddTrigger(Trigger_Cylinder, ID, Enabled);
    // Setup trigger data
    Trigger->x1     = XPos;
    Trigger->y1     = YPos;
    Trigger->z1     = ZPos;
    Trigger->Radius = Radius * Radius;
    Trigger->y2     = Height;
    return TRUE;
}

BOOL CTrigger::AddTriangle(long ID, BOOL Enabled,float x1, float z1,
						   float x2, float z2,float x3, float z3,float YPos, float Height)                                                                                                                                                                    
{
	// Create a new trigger structure and link in 
    sTrigger *Trigger = AddTrigger(Trigger_Triangle, ID, Enabled);
    // Setup trigger data
    Trigger->x1 = x1;
    Trigger->z1 = z1;
    Trigger->x2 = x2;
    Trigger->z2 = z2;
    Trigger->x3 = x3;
    Trigger->z3 = z3;
    Trigger->y1 = YPos;
    Trigger->y2 = Height;
    return TRUE;
}

BOOL CTrigger::Remove(long ID)
{
	sTrigger *TriggerPtr, *NextTrigger;
    long Count = 0; 
    if((TriggerPtr = m_TriggerParent) != NULL) 
	{  
		while(TriggerPtr != NULL)// Scan through list of triggers 
		{    			     
			NextTrigger = TriggerPtr->Next;// Remember which is next		  
			if(TriggerPtr->ID == ID)// Matched? 
			{   
				// Remove from list     
				if(TriggerPtr->Prev != NULL)        
					TriggerPtr->Prev->Next = TriggerPtr->Next;        
				else    
					m_TriggerParent = TriggerPtr->Next;      
				if(TriggerPtr->Next != NULL)       
					TriggerPtr->Next->Prev = TriggerPtr->Prev;       				        
				TriggerPtr->Next = NULL;// Release memory     
				delete TriggerPtr;   		      
				m_NumTriggers--;// Decrease number of triggers and increase # removed   
				Count++;  
			}    			    
			TriggerPtr = NextTrigger;// Go to next trigger   
		}
	}
	return (Count) ? TRUE : FALSE;// Return TRUE if any found and removed
}

BOOL CTrigger::Free()
{ 
	delete m_TriggerParent;
    m_TriggerParent = NULL;
    m_NumTriggers = 0;
    return TRUE;
}

long CTrigger::GetTrigger(float XPos, float YPos, float ZPos)
{
    float XDiff, YDiff, ZDiff, Dist;
    D3DXVECTOR2 vecNorm;
    sTrigger *Trigger;   
    if((Trigger = m_TriggerParent) != NULL)
	{
		while(Trigger != NULL)// Scan through list of triggers
		{     
			if(Trigger->Enabled == TRUE)// Only bother if enabled 
			{    			   
				switch(Trigger->Type)// Check based on type 
				{       
				case Trigger_Sphere:          
					// Check distance from sphere (using radius)           
					XDiff = (float)fabs(Trigger->x1 - XPos);          
					YDiff = (float)fabs(Trigger->y1 - YPos);           
					ZDiff = (float)fabs(Trigger->z1 - ZPos);         
					Dist = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;            
					if(Dist <= Trigger->Radius)          
						return Trigger->ID;          
					break;           
				case Trigger_Box:     
					// Check if inside box          
					if(XPos >= Trigger->x1 && XPos <= Trigger->x2) 
					{            
						if(YPos >= Trigger->y1 && YPos <= Trigger->y2)
						{             
							if(ZPos >= Trigger->z1 && ZPos <= Trigger->z2)                
								return Trigger->ID;              
						}           
					}         
					break;      
				case Trigger_Cylinder:           
					// First make sure within height bounds          
					if(YPos >= Trigger->y1 && YPos <= Trigger->y1 + Trigger->y2)                                      
					{            
						// Check distance from cylinder            
						XDiff = (float)fabs(Trigger->x1 - XPos);            
						YDiff = (float)fabs(Trigger->y1 - YPos);            
						ZDiff = (float)fabs(Trigger->z1 - ZPos);            
						Dist = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;          
						if(Dist <= Trigger->Radius)              
							return Trigger->ID;          
					}         
					break;         
				case Trigger_Triangle:           
					// First make sure within height bounds           
					if(YPos >= Trigger->y1 &&  YPos <= Trigger->y1 + Trigger->y2)                                      
					{                      
						// Check if point in front of all lines           
						// x1,z1 to x2,z2           
						D3DXVec2Normalize(&vecNorm,&D3DXVECTOR2(Trigger->z2 - Trigger->z1,Trigger->x1 - Trigger->x2));                                                                                     
						if(D3DXVec2Dot(&D3DXVECTOR2(XPos-Trigger->x1, ZPos-Trigger->z1),&vecNorm) < 0)                                                                                                   
							break;           
						// x2,z2 to x3,z3            
						D3DXVec2Normalize(&vecNorm, &D3DXVECTOR2(Trigger->z3 - Trigger->z2, Trigger->x2 - Trigger->x3));                                                                                       
						if(D3DXVec2Dot(&D3DXVECTOR2(XPos-Trigger->x2,ZPos-Trigger->z2),&vecNorm) < 0)                                                                                                      
							break;            
						// x3,z3 to x1,z1           
						D3DXVec2Normalize(&vecNorm,&D3DXVECTOR2(Trigger->z1 - Trigger->z3,Trigger->x3 - Trigger->x1));                                                                                      
						if(D3DXVec2Dot(&D3DXVECTOR2(XPos-Trigger->x3,ZPos-Trigger->z3),&vecNorm) < 0)                                                                                                  
							break;           
						return Trigger->ID;          
					}          
					break;       
				}
			}  			   
			Trigger = Trigger->Next;// Go to next trigger    
		} 
	}
	return 0; // 0 means no trigger found
}

BOOL CTrigger::GetEnableState(long ID)
{ 
	sTrigger *TriggerPtr;
	if((TriggerPtr = m_TriggerParent) != NULL)
	{  
		while(TriggerPtr != NULL)// Loop through all triggers looking for ID 
		{  			    
			if(TriggerPtr->ID == ID)// If matched ID then set return state     
				return TriggerPtr->Enabled;      
			TriggerPtr = TriggerPtr->Next;// Go to next flag  
		}
	}
	return FALSE;// Return FALSE for none found
}

BOOL CTrigger::Enable(long ID, BOOL Enable)
{
	sTrigger *TriggerPtr;
	long Count = 0;
	if((TriggerPtr = m_TriggerParent) != NULL) 
	{  
		while(TriggerPtr != NULL)// Loop through all triggers looking for ID 
		{     	 
			if(TriggerPtr->ID == ID)// If matched ID then set flag and increase count
			{      
				TriggerPtr->Enabled = Enable;       
				Count++;   
			}   		   
			TriggerPtr = TriggerPtr->Next;// Go to next flag 
		}
	}
	return (Count) ? TRUE : FALSE;// Return TRUE if any triggers changed
}

long CTrigger::GetNumTriggers()
{
    return m_NumTriggers;
}

sTrigger *CTrigger::GetParentTrigger()
{
    return m_TriggerParent;
}
