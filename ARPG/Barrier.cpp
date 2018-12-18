/*--------------------------------------------
Jim Adams 版权所有
2008-5-1 SCY 修改
--------------------------------------------*/
#include "../Core/Core_Global.h"
#include "Barrier.h"

CBarrier::CBarrier()
{
	m_NumBarriers = 0;
    m_BarrierParent = NULL;
}

CBarrier::~CBarrier()
{
    Free();
}

long CBarrier::GetNextLong(FILE *fp)
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

float CBarrier::GetNextFloat(FILE *fp)
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

BOOL CBarrier::SetMesh(long ID,CGraphics *Graphics, CMesh *Mesh)                      
{
    sBarrier *BarrierPtr;
    long Count = 0; 
    if((BarrierPtr = m_BarrierParent) != NULL)// Loop through all Barriers looking for ID 
	{
        while(BarrierPtr != NULL) 
		{   		      
			if(BarrierPtr->ID == ID)// If matched ID then set mesh    
		    {
                BarrierPtr->Object.Create(Graphics, Mesh);
                Count++;     
			}  		  
			BarrierPtr = BarrierPtr->Next;// Go to next flag  
		} 
	}
	return (Count) ? TRUE : FALSE;// Return TRUE if any meshes set
}

BOOL CBarrier::SetAnim(long ID, CAnimation *Anim,char *Name, long Time)                             
{
    sBarrier *BarrierPtr;
    long Count = 0;  
    if((BarrierPtr = m_BarrierParent) != NULL)// Loop through all Barriers looking for ID
	{   
		while(BarrierPtr != NULL) 
		{     		   
			if(BarrierPtr->ID == ID)// If matched ID then set animation 
			{        
				BarrierPtr->Object.SetAnimation(Anim, Name, Time);      
				Count++;    
			}			   
			BarrierPtr = BarrierPtr->Next;// Go to next flag  
		}
	}	
	return (Count) ? TRUE : FALSE;// Return TRUE if any animations set
}

BOOL CBarrier::Render(unsigned long Time, CFrustum *Frustum)
{
	sBarrier *BarrierPtr;
    float Radius;   
    if(Frustum == NULL)// Error checking
        return FALSE;   
    if((BarrierPtr = m_BarrierParent) != NULL)// Loop through all Barriers
	{
		while(BarrierPtr != NULL)
		{ 		    
			BarrierPtr->Object.GetBounds(NULL,NULL,NULL,NULL,NULL,NULL,&Radius);// Get radius                                              
			if(Frustum->CheckSphere(BarrierPtr->XPos,BarrierPtr->YPos,BarrierPtr->ZPos, Radius))//perform frustum check                                                             
			{						 			     
				BarrierPtr->Object.Move(BarrierPtr->XPos,BarrierPtr->YPos,BarrierPtr->ZPos);// Position object                                                                                    
				BarrierPtr->Object.Rotate(BarrierPtr->XRot,BarrierPtr->YRot,BarrierPtr->ZRot);                                                                                         				   
				BarrierPtr->Object.UpdateAnimation(Time, TRUE);// Update animation    				
				BarrierPtr->Object.Render();// Render object
			} 
			BarrierPtr = BarrierPtr->Next;
		}
	}
	return TRUE;
}

BOOL CBarrier::Load(char *Filename)
{ 
	FILE *fp;
	long Type, ID;
    BOOL Enabled;
    float x1, y1, z1, x2, y2, z2, x3, z3, Radius;
    float XPos, YPos, ZPos, XRot, YRot, ZRot; 
    Free(); // Remove all current Barriers
    if((fp=fopen(Filename, "rb"))==NULL) // Open file
        return FALSE;   
    while(1)// Start looping, reading in until EOF reached
	{   		  
		if((ID = GetNextLong(fp)) == -1)// Get ID of Barrier    
			break;		  
		Type = GetNextLong(fp);// Get type		
		Enabled = (GetNextLong(fp)) ? TRUE : FALSE;// Get enabled status
		// Get coordinates and rotation
		XPos = GetNextFloat(fp);
        YPos = GetNextFloat(fp);
        ZPos = GetNextFloat(fp);
        XRot = GetNextFloat(fp);
        YRot = GetNextFloat(fp);
        ZRot = GetNextFloat(fp); 
        switch(Type)// Read in rest depending on type 
		{
		case Barrier_Sphere:       
			x1 = GetNextFloat(fp); 
			y1 = GetNextFloat(fp); 
			z1 = GetNextFloat(fp);      
			Radius = GetNextFloat(fp);  
			AddSphere(ID, Enabled, XPos, YPos, ZPos,XRot, YRot, ZRot, x1, y1, z1, Radius);                         
			break;      
		case Barrier_Box:      
			x1 = GetNextFloat(fp);
            y1 = GetNextFloat(fp);
            z1 = GetNextFloat(fp);
            x2 = GetNextFloat(fp);
            y2 = GetNextFloat(fp);
            z2 = GetNextFloat(fp);
            AddBox(ID, Enabled, XPos, YPos, ZPos,XRot, YRot, ZRot, x1, y1, z1, x2, y2, z2);                                      
			break;  
		case Barrier_Cylinder:
            x1 = GetNextFloat(fp);
            y1 = GetNextFloat(fp);
            z1 = GetNextFloat(fp);
            Radius = GetNextFloat(fp);
            y2 = GetNextFloat(fp);
            AddCylinder(ID, Enabled, XPos, YPos, ZPos,XRot, YRot, ZRot, x1, y1, z1, Radius, y2);                                
			break;       
		case Barrier_Triangle:
            x1 = GetNextFloat(fp);
            z1 = GetNextFloat(fp);
            x2 = GetNextFloat(fp);
            z2 = GetNextFloat(fp);
            x3 = GetNextFloat(fp);
            z3 = GetNextFloat(fp);
            y1 = GetNextFloat(fp);
            y2 = GetNextFloat(fp);
            AddTriangle(ID, Enabled, XPos, YPos, ZPos,XRot, YRot, ZRot,x1, z1, x2, z2, x3, z3, y1, y2);                                                                   
            break;    
		default: 
			fclose(fp); // Some error occurred              
			Free();           
			return FALSE;  
		} 
	}
	fclose(fp);
	return (m_NumBarriers) ? TRUE : FALSE;
}

BOOL CBarrier::Save(char *Filename)
{ 
	FILE *fp;
    sBarrier *BarrierPtr;
    if(!m_NumBarriers)
        return FALSE;
    if((BarrierPtr = m_BarrierParent) == NULL)
        return FALSE;
	if((fp=fopen(Filename, "wb"))==NULL)// Open file
		return FALSE;
	while(BarrierPtr != NULL)// Write out all Barriers 
	{    				
		fprintf(fp, "%lu ", BarrierPtr->ID);// Write out ID and Type of Barrier
		fprintf(fp, "%lu ", BarrierPtr->Type); 
		fprintf(fp, "%lu ", (BarrierPtr->Enabled) ? 1 : 0);// Write out Enabled flag
        // Write out coordinates and rotation
        fprintf(fp, "%lf %lf %lf %lf %lf %lf ",BarrierPtr->XPos, BarrierPtr->YPos, BarrierPtr->ZPos,BarrierPtr->XRot, BarrierPtr->YRot, BarrierPtr->ZRot);                                    		
		switch(BarrierPtr->Type)// Write out remaining data depending on type
		{   
		case Barrier_Sphere:     
			fprintf(fp, "%lf %lf %lf %lf\r\n",
				BarrierPtr->x1, BarrierPtr->y1,BarrierPtr->z1, BarrierPtr->Radius);                                                                  
			break;     
		case Barrier_Box:       
			fprintf(fp, "%lf %lf %lf %lf %lf %lf\r\n",
				BarrierPtr->x1, BarrierPtr->y1, BarrierPtr->z1,
				BarrierPtr->x2, BarrierPtr->y2, BarrierPtr->z2);                                    
			break;
        case Barrier_Cylinder:
            fprintf(fp, "%lf %lf %lf %lf %lf\r\n",                         
				BarrierPtr->x1, BarrierPtr->y1, BarrierPtr->z1,              
				BarrierPtr->Radius, BarrierPtr->y2);      
			break;    
        case Barrier_Triangle:
            fprintf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf\r\n",         
				BarrierPtr->x1, BarrierPtr->z1,               
                BarrierPtr->x2, BarrierPtr->z2,               
                BarrierPtr->x3, BarrierPtr->z3,               
                BarrierPtr->y1, BarrierPtr->y2);       
			break;
    
		}
 
	}
	fclose(fp);
	return TRUE;
}

sBarrier *CBarrier::AddBarrier(long Type, long ID,BOOL Enabled,
							   float XPos, float YPos, float ZPos, float XRot, float YRot, float ZRot)                                                                                                            
{
    sBarrier *Barrier = new sBarrier();// Allocate a new Barrier structure and link in
    Barrier->Prev = NULL;
    if((Barrier->Next = m_BarrierParent) != NULL)
        m_BarrierParent->Prev = Barrier;
    m_BarrierParent = Barrier;
    // Set Barrier Type, ID, and Enabled flag
    Barrier->Type = Type;
    Barrier->ID = ID;
    Barrier->Enabled = Enabled;
    Barrier->XPos = XPos;
    Barrier->YPos = YPos;
    Barrier->ZPos = ZPos;
    Barrier->XRot = XRot;
    Barrier->YRot = YRot;
    Barrier->ZRot = ZRot;
    m_NumBarriers++;// Increase Barrier count
    return Barrier;
}

BOOL CBarrier::AddSphere(long ID, BOOL Enabled,                             
						 float XPos,  float YPos,  float ZPos,                       
						 float XRot,  float YRot,  float ZRot,                        
						 float CXPos, float CYPos, float CZPos,                       
						 float Radius)
{
	// Create a new Barrier structure and link in 
	sBarrier *Barrier = AddBarrier(Barrier_Sphere, ID, Enabled,XPos, YPos, ZPos,XRot, YRot, ZRot);                                                                           	
	// Setup Barrier data
	Barrier->x1 = CXPos;
    Barrier->y1 = CYPos;
    Barrier->z1 = CZPos;
    Barrier->Radius = Radius * Radius;//
    return TRUE;
}

BOOL CBarrier::AddBox(long ID, BOOL Enabled,                  
                      float XPos,  float YPos,  float ZPos,   
                      float XRot,  float YRot,  float ZRot,   
                      float XMin, float YMin, float ZMin,     
                      float XMax, float YMax, float ZMax)
{
	// Create a new Barrier structure and link in 
    sBarrier *Barrier = AddBarrier(Barrier_Box, ID, Enabled,XPos, YPos, ZPos,XRot, YRot, ZRot);                                                                                
	// Setup Barrier data (fix for min/max values)
    Barrier->x1 = min(XMin, XMax);
    Barrier->y1 = min(YMin, YMax);
    Barrier->z1 = min(ZMin, ZMax);
    Barrier->x2 = max(XMin, XMax);
    Barrier->y2 = max(YMin, YMax);
    Barrier->z2 = max(ZMin, ZMax);
    return TRUE;
}

BOOL CBarrier::AddCylinder(long ID, BOOL Enabled,                                    
						   float XPos,  float YPos,  float ZPos,                        
						   float XRot,  float YRot,  float ZRot,                   
						   float CXPos, float CYPos, float CZPos,                     
						   float Radius, float Height)
{
	// Create a new Barrier structure and link in 
    sBarrier *Barrier = AddBarrier(Barrier_Cylinder,ID,Enabled,XPos, YPos, ZPos,XRot, YRot, ZRot);                                                                                      
	// Setup Barrier data
    Barrier->x1     = XPos;
    Barrier->y1     = YPos;
    Barrier->z1     = ZPos;
    Barrier->Radius = Radius * Radius;//
    Barrier->y2     = Height;
    return TRUE;
}

BOOL CBarrier::AddTriangle(long ID, BOOL Enabled,                                    
						   float XPos,  float YPos,  float ZPos,    
                           float XRot,  float YRot,  float ZRot,    
                           float x1, float z1,                      
                           float x2, float z2,                      
                           float x3, float z3,                      
                           float CYPos, float Height)
{
  
	// Create a new Barrier structure and link in 
	sBarrier *Barrier = AddBarrier(Barrier_Triangle,ID,Enabled,XPos, YPos, ZPos,XRot, YRot, ZRot);                                                                             
	// Setup Barrier data
    Barrier->x1 = x1;
    Barrier->z1 = z1;
    Barrier->x2 = x2;
    Barrier->z2 = z2;
    Barrier->x3 = x3;
    Barrier->z3 = z3;
    Barrier->y1 = YPos;
    Barrier->y2 = Height;
	return TRUE;
}

BOOL CBarrier::Remove(long ID)
{
	sBarrier *BarrierPtr, *NextBarrier;
	long Count = 0;
	if((BarrierPtr = m_BarrierParent) != NULL)// Scan through list of Barriers
	{
		while(BarrierPtr != NULL) 
		{		  
			NextBarrier = BarrierPtr->Next;// Remember which is next	
			if(BarrierPtr->ID == ID)// Matched? 
			{    			      
				if(BarrierPtr->Prev != NULL)// Remove from list Modify two pointers        
					BarrierPtr->Prev->Next = BarrierPtr->Next;
				else  
					m_BarrierParent = BarrierPtr->Next;
				if(BarrierPtr->Next != NULL)  
    				BarrierPtr->Next->Prev = BarrierPtr->Prev;	
				BarrierPtr->Next = NULL;// Release memory
				delete BarrierPtr;		
				m_NumBarriers--;// Decrease number of Barriers and increase # removed
				Count++;
			}			
			BarrierPtr = NextBarrier;// Go to next Barrier
		}
	}	
	return (Count) ? TRUE : FALSE;// Return TRUE if any found and removed
}

BOOL CBarrier::Free()
{ 
	delete m_BarrierParent;
    m_BarrierParent = NULL;
    m_NumBarriers = 0;
    return TRUE;
}

long CBarrier::GetBarrier(float XPos, float YPos, float ZPos)//Given a point and return which barrier it in 
{
	float XDiff, YDiff, ZDiff, Dist;
	D3DXVECTOR2 vecNorm;
	sBarrier *Barrier;
	if((Barrier = m_BarrierParent) != NULL) 
	{
		while(Barrier != NULL)
		{		 
			if(Barrier->Enabled == TRUE)// Only bother if enabled  
			{    			        
				switch(Barrier->Type)// Check based on type 
				{       
				case Barrier_Sphere:// Check distance from sphere (using radius)           					           
					XDiff = (float)fabs(Barrier->x1 - XPos);        
					YDiff = (float)fabs(Barrier->y1 - YPos);       
					ZDiff = (float)fabs(Barrier->z1 - ZPos);      
					Dist = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;         
					if(Dist <= Barrier->Radius)//Barrier->Radius is Radius*Radius           
						return Barrier->ID;          
					break;    
				case Barrier_Box:          			  
					if(XPos >= Barrier->x1 && XPos <= Barrier->x2)// Check if inside box 
					{          
						if(YPos >= Barrier->y1 && YPos <= Barrier->y2)
						{          
							if(ZPos >= Barrier->z1 && ZPos <= Barrier->z2)        
								return Barrier->ID;       
						}         
					}        
					break;       
				case Barrier_Cylinder:         					          
					if(YPos >= Barrier->y1 &&YPos <= Barrier->y1 + Barrier->y2)// First make sure within height bounds                                     					 
					{             
						// Check distance from cylinder            
						XDiff = (float)fabs(Barrier->x1 - XPos);          
						YDiff = (float)fabs(Barrier->y1 - YPos);          
						ZDiff = (float)fabs(Barrier->z1 - ZPos);           
						Dist = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;           
						if(Dist <= Barrier->Radius)              
							return Barrier->ID;        
					}        
					break;         
				case Barrier_Triangle:        					        
					if(YPos >= Barrier->y1 && YPos <= Barrier->y1 + Barrier->y2)// First make sure within height bounds                                  						
					{                     
						// Check if point in front of all lines      					            
						D3DXVec2Normalize(&vecNorm,// x1,z1 to x2,z2
							&D3DXVECTOR2(Barrier->z2 - Barrier->z1,Barrier->x1 - Barrier->x2));                                         							                                   							           
						if(D3DXVec2Dot(&D3DXVECTOR2(XPos-Barrier->x1,ZPos-Barrier->z1),&vecNorm) < 0)                                         							              							
							break;           						           
						D3DXVec2Normalize(&vecNorm,// x2,z2 to x3,z3 
							&D3DXVECTOR2(Barrier->z3 - Barrier->z2,Barrier->x2 - Barrier->x3));                                           							                                     							             
						if(D3DXVec2Dot(&D3DXVECTOR2(XPos-Barrier->x2,ZPos-Barrier->z2),&vecNorm) < 0)                                         							                                            							               
							break;             						             
						D3DXVec2Normalize(&vecNorm,// x3,z3 to x1,z1
							&D3DXVECTOR2(Barrier->z1 - Barrier->z3,Barrier->x3 - Barrier->x1));                                           							                                     							          
						if(D3DXVec2Dot(&D3DXVECTOR2(XPos-Barrier->x3,ZPos-Barrier->z3),&vecNorm) < 0) 
							break;                                            							                                             							              						            
						return Barrier->ID;          
					}        	
					break;
       
				}//end switch
			}//end if    			
			Barrier = Barrier->Next;// Go to next Barrier
		}//end while
	}
	return 0; // 0 means no Barrier found
}

BOOL CBarrier::GetEnableState(long ID)
{
	sBarrier *BarrierPtr;  	
	if((BarrierPtr = m_BarrierParent) != NULL)// Loop through all Barriers looking for ID 
	{  
		while(BarrierPtr != NULL) 
		{			     
			if(BarrierPtr->ID == ID)// If matched ID then set return state        
				return BarrierPtr->Enabled;     			  
			BarrierPtr = BarrierPtr->Next;// Go to next flag  
		}
	}	
	return FALSE;// Return FALSE for none found
}

BOOL CBarrier::Enable(long ID, BOOL Enable)
{
    sBarrier *BarrierPtr;
    long Count = 0;  
    if((BarrierPtr = m_BarrierParent) != NULL)// Loop through all Barriers looking for ID 
	{ 
		while(BarrierPtr != NULL) 
		{  			   
			if(BarrierPtr->ID == ID)// If matched ID then set flag and increase count 
			{       
				BarrierPtr->Enabled = Enable;     
				Count++;     
			} 		
			BarrierPtr = BarrierPtr->Next;// Go to next flag   
		}
	} 	  
	return (Count) ? TRUE : FALSE;// Return TRUE if any Barriers changed
}

long CBarrier::GetNumBarriers()
{
    return m_NumBarriers;
}

sBarrier *CBarrier::GetParentBarrier()
{
    return m_BarrierParent;
}
