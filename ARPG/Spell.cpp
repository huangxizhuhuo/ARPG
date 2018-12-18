/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#include "../Core/Core_Global.h"
#include "Frustum.h"
#include "MCL.h"
#include "MSL.h"
#include "Chars.h"
#include "Spell.h"

CSpellController::CSpellController()
{
    m_Graphics       = NULL;  // Clear CGraphics pointer
    m_Frustum        = NULL;  // Clear frustum pointer
    m_Chars          = NULL;  // Clear character controller 
    m_SpellParent    = NULL;
    m_NumMeshes      = 0;     // Clear mesh data
    m_Meshes         = NULL;
    m_TexturePath[0] = 0;
}

CSpellController::~CSpellController()
{
    Shutdown();
}

BOOL CSpellController::Init(CGraphics *Graphics,char *DefinitionFile,long NumSpellMeshes, 
							char **MeshNames,char *TexturePath,CCharacterController *Controller)                                                                                                                                  
{
	FILE *fp;
	long i;
	Free();  // Free prior init
	// Get parent graphics object
	if((m_Graphics = Graphics) == NULL || MeshNames == NULL || DefinitionFile == NULL)   
		return FALSE;
	if((fp=fopen(DefinitionFile, "rb"))==NULL)// Load the spells  
		return FALSE;
	for(i=0;i<NUM_SPELL_DEFINITIONS;i++)  
		fread(&m_Spells[i], 1, sizeof(sSpell), fp);
	fclose(fp);
	m_Chars = Controller;  // Store controller pointer
	if(TexturePath != NULL)// Copy over texture name 
		strcpy(m_TexturePath, TexturePath);	
	if((m_NumMeshes = NumSpellMeshes))// Get mesh names 
	{  
		m_Meshes = new sSpellMeshList[NumSpellMeshes]();  
		for(i=0;i<m_NumMeshes;i++)   
			strcpy(m_Meshes[i].Filename, MeshNames[i]);
	}
	return TRUE;
}

BOOL CSpellController::Shutdown()
{
	long i;
	Free();  // Free class data
	if(m_NumMeshes && m_Meshes != NULL)// Release spell meshes
	{  
		for(i=0;i<m_NumMeshes;i++) 
		{   
			m_Meshes[i].Animation.Free(); 
			m_Meshes[i].Mesh.Free();     
			m_Meshes[i].Count = 0;  
		}
	}	
	m_Graphics = NULL;// Clear graphics object
	delete [] m_Meshes;// Release mesh list  
	m_Meshes = NULL;
	m_NumMeshes = 0; 
	return TRUE;
}

BOOL CSpellController::Free()
{
	delete m_SpellParent;// Release spell list
	m_SpellParent = NULL;
	return TRUE;
}

sSpell *CSpellController::GetSpell(long SpellNum)
{
	return &m_Spells[SpellNum];
}

BOOL CSpellController::Add(long SpellNum,sCharacter *Caster, long TargetType,float SourceX,
						   float SourceY, float SourceZ,float TargetX, float TargetY, float TargetZ)                                                                            
{ 
	sSpellTracker *SpellPtr;
	if(m_Graphics == NULL || m_Meshes == NULL) // Error checking  
		return FALSE;
	// Make sure character is allowed to cast spell 
	if(Caster != NULL) 
	{ 
		if(!(Caster->Def.MagiCSpells[SpellNum/32] & (1 << (SpellNum & 31))))                                                   							
			return FALSE;		
		if(Caster != NULL)// Make sure caster has enough MP to cast and reduce MP  
		{    		     
			if(Caster->ManaPoints < m_Spells[SpellNum].Cost)// Return if not enough mana        
				return FALSE;    
		}
	}
	// Allocate a new structure and link in to head of list
	SpellPtr = new sSpellTracker(); 
	if(m_SpellParent != NULL)
		m_SpellParent->Prev = SpellPtr;
	SpellPtr->Next = m_SpellParent;
	m_SpellParent = SpellPtr;
    // Set structure data
    SpellPtr->SpellNum = SpellNum;
    SpellPtr->Caster   = Caster;
    SpellPtr->Type     = TargetType;
    SpellPtr->SourceX  = SourceX;
    SpellPtr->SourceY  = SourceY;
    SpellPtr->SourceZ  = SourceZ;
    SpellPtr->TargetX  = TargetX;
    SpellPtr->TargetY  = TargetY;
    SpellPtr->TargetZ  = TargetZ;  
    SetAnimData(SpellPtr, 0);// Setup the mesh and movement data
    return TRUE;
}

BOOL CSpellController::Update(long Elapsed)
{
    sSpellTracker *SpellPtr, *NextSpell;
    long SpellNum, Anim;
    BOOL GotoNextAnim;
    float Speed;
    if((SpellPtr = m_SpellParent) == NULL)
        return FALSE;   
    while(SpellPtr != NULL)// Scan through all spells in use
	{  		  
		NextSpell = SpellPtr->Next;// Remember next spell in list 
		// Get local data 
		SpellNum = SpellPtr->SpellNum; 
		Anim     = SpellPtr->CurrentAnimation;
		// Update/move/countdown spell object  
		GotoNextAnim = FALSE;
		switch(m_Spells[SpellNum].MeshPos[Anim])
		{     
		case POSITION_NONE:      		     
			GotoNextAnim = TRUE;// Go to next animation   
			break;    
		case POSITION_CASTER:   
		case POSITION_TARGET:    
		case POSITION_SCALE:      
			SpellPtr->Time -= Elapsed;     
			if(SpellPtr->Time <= 0)      
				GotoNextAnim = TRUE;      
			break;  
		case POSITION_TOTARGET:  
		case POSITION_TOCASTER:    
			Speed = (float)Elapsed / 1000.0f * SpellPtr->Speed;   			    
			SpellPtr->Distance -= Speed;// Decrease distance amount    
			if(SpellPtr->Distance <= 0.0f)       
				GotoNextAnim = TRUE;  
			else
			{      
				SpellPtr->XPos += (SpellPtr->XAdd * Speed);    
				SpellPtr->YPos += (SpellPtr->YAdd * Speed);       
				SpellPtr->ZPos += (SpellPtr->ZAdd * Speed);     
			}    
			break;  
		}	  
		if(GotoNextAnim == TRUE)// Update to next animation    
			SetAnimData(SpellPtr, Anim+1);   	 
		SpellPtr = NextSpell;// Go to next spell
	}
	return TRUE;
}

BOOL CSpellController::SetAnimData(sSpellTracker *SpellPtr,long Num)                                    
{
	float XDiff, YDiff, ZDiff, Dist, Length;
	float XAngle = 0.0f, YAngle = 0.0f;
	float Scale = 1.0f;
	long  i;
	long  SpellNum, MeshNum;
	float SourceX, SourceY, SourceZ;
	float TargetX, TargetY, TargetZ;
	// Process spell effect if no more animations left while storing the current animation number.
	if((SpellPtr->CurrentAnimation = Num) >= 3)
	{   
		if(m_Chars != NULL)   
			m_Chars->Spell(SpellPtr->Caster, SpellPtr, m_Spells); 		
		for(i=0;i<3;i++)// Remove any mesh references 
		{  
			if(m_Spells[SpellPtr->SpellNum].MeshPos[i] != POSITION_NONE)                                                     
			{    
				MeshNum = m_Spells[SpellPtr->SpellNum].MeshNum[i];      				   
				m_Meshes[MeshNum].Count--;// Decrease count and remove if needed     
				if(!m_Meshes[MeshNum].Count) 
				{       
					m_Meshes[MeshNum].Mesh.Free();          
					m_Meshes[MeshNum].Animation.Free();     
				}    
			}   
		}
		// Remove spell from list 
		if(SpellPtr->Prev != NULL)    
			SpellPtr->Prev->Next = SpellPtr->Next; 
		else   
			m_SpellParent = SpellPtr->Next;  
		if(SpellPtr->Next != NULL) 
			SpellPtr->Next->Prev = SpellPtr->Prev; 
		SpellPtr->Prev = SpellPtr->Next = NULL; 
		delete SpellPtr;
		return TRUE;
	}
	// Setup local data 
	SourceX  = SpellPtr->SourceX;
	SourceY  = SpellPtr->SourceY;
	SourceZ  = SpellPtr->SourceZ;
    TargetX  = SpellPtr->TargetX;
    TargetY  = SpellPtr->TargetY;
    TargetZ  = SpellPtr->TargetZ;
    SpellNum = SpellPtr->SpellNum;
    // Go to next animation if no mesh to use
    if(m_Spells[SpellNum].MeshPos[Num] == POSITION_NONE)
        return SetAnimData(SpellPtr,Num+1);
    // Get mesh # to use
    MeshNum = m_Spells[SpellNum].MeshNum[Num];    
    if(!m_Meshes[MeshNum].Count)// Load mesh and animation if needed 
	{  
		m_Meshes[MeshNum].Mesh.Load(m_Graphics,m_Meshes[MeshNum].Filename,m_TexturePath);                                                                    
		m_Meshes[MeshNum].Animation.Load(m_Meshes[MeshNum].Filename, &m_Meshes[MeshNum].Mesh);                                                                            
		// Set animation loop
		m_Meshes[MeshNum].Animation.SetLoop(m_Spells[SpellNum].MeshLoop[Num], "Anim");                                 
	}
	// Configure graphics object
	SpellPtr->Object.Create(m_Graphics,&m_Meshes[MeshNum].Mesh);
    m_Meshes[MeshNum].Count++;  
    switch(m_Spells[SpellNum].MeshPos[Num])// Setup mesh movements
	{  
	case POSITION_CASTER:     
		SpellPtr->XPos = SourceX;
        SpellPtr->YPos = SourceY;
        SpellPtr->ZPos = SourceZ;
        SpellPtr->Time = (long)m_Spells[SpellNum].MeshSpeed[Num];
        if(SpellPtr->Caster != NULL)
            YAngle = SpellPtr->Caster->Direction;
        break;
    case POSITION_TOTARGET:
        // Store position and speed info
        SpellPtr->XPos  = SourceX;
        SpellPtr->YPos  = SourceY;
        SpellPtr->ZPos  = SourceZ;
        SpellPtr->Speed = m_Spells[SpellNum].MeshSpeed[Num];
        // Calculate movement
        XDiff = (float)fabs(TargetX - SourceX);
        YDiff = (float)fabs(TargetY - SourceY);
        ZDiff = (float)fabs(TargetZ - SourceZ);
        Dist  = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);
        if((SpellPtr->Distance = Dist) != 0.0f) 
		{          
			SpellPtr->XAdd = (TargetX - SourceX) / Dist;
            SpellPtr->YAdd = (TargetY - SourceY) / Dist;
            SpellPtr->ZAdd = (TargetZ - SourceZ) / Dist;
            // Calculate angles
            XAngle = -(float)atan(SpellPtr->YAdd);
            YAngle =  (float)atan2(SpellPtr->XAdd, SpellPtr->ZAdd);  
		}    
		break;
    case POSITION_TARGET:      
		SpellPtr->XPos = TargetX;   
		SpellPtr->YPos = TargetY;
        SpellPtr->ZPos = TargetZ;
        SpellPtr->Time = (long)m_Spells[SpellNum].MeshSpeed[Num];
        // Calculate distance from source to target
        XDiff = (float)fabs(TargetX - SourceX);
        ZDiff = (float)fabs(TargetZ - SourceZ);
        Dist  = (float)sqrt(XDiff*XDiff+ZDiff*ZDiff);
        SpellPtr->XAdd = (TargetX - SourceX) / Dist;
        SpellPtr->ZAdd = (TargetZ - SourceZ) / Dist;
        // Calculate angle
        YAngle =  (float)atan2(SpellPtr->XAdd, SpellPtr->ZAdd);
        break;
    case POSITION_TOCASTER:
        // Store position and speed info
        SpellPtr->XPos  = TargetX;
        SpellPtr->YPos  = TargetY;
        SpellPtr->ZPos  = TargetZ;
        SpellPtr->Speed = m_Spells[SpellNum].MeshSpeed[Num];
        // Calculate movement
        XDiff = (float)fabs(SourceX - TargetX);
        YDiff = (float)fabs(SourceY - TargetY);
        ZDiff = (float)fabs(SourceZ - TargetZ);
        Dist  = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);
        if((SpellPtr->Distance = Dist) != 0.0f) 
		{    
			SpellPtr->XAdd = (SourceX - TargetX) / Dist;        
			SpellPtr->YAdd = (SourceY - TargetY) / Dist;      
			SpellPtr->ZAdd = (SourceZ - TargetZ) / Dist;
			// Calculate angles      
			XAngle = -(float)atan(SpellPtr->YAdd);     
			YAngle =  (float)atan2(SpellPtr->XAdd, SpellPtr->ZAdd);    
		}   
		break;
    case POSITION_SCALE:    
		// Store position and speed info  
		SpellPtr->XPos  = SourceX;
        SpellPtr->YPos  = SourceY;
        SpellPtr->ZPos  = SourceZ;
        SpellPtr->Time = (long)m_Spells[SpellNum].MeshSpeed[Num];
        // Get distance from source to target and size of mesh
        XDiff = (float)fabs(TargetX - SourceX);
        YDiff = (float)fabs(TargetY - SourceY);
        ZDiff = (float)fabs(TargetZ - SourceZ);
        Dist  = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);
        SpellPtr->Object.GetBounds(NULL,NULL,NULL, NULL,NULL,&Length,NULL);                                                 
        Scale = Dist / Length;// Calculate scale    
        // Calculate angles
        SpellPtr->XAdd = (TargetX - SourceX) / Dist;
        SpellPtr->YAdd = (TargetY - SourceY) / Dist;
        SpellPtr->ZAdd = (TargetZ - SourceZ) / Dist;
        XAngle = -(float)atan(SpellPtr->YAdd);
        YAngle =  (float)atan2(SpellPtr->XAdd, SpellPtr->ZAdd);
        break;
	}   
    SpellPtr->Object.Rotate(XAngle, YAngle, 0.0f);// Rotate object to points towards target  
    SpellPtr->Object.Scale(1.0f, 1.0f, Scale); // Scale object
    // Set the animation
    SpellPtr->Object.SetAnimation(&m_Meshes[MeshNum].Animation, "Anim", timeGetTime()/30);                                      
    if(m_Spells[SpellNum].MeshSound[Num] != -1)// Play the sound
        SpellSound(m_Spells[SpellNum].MeshSound[Num]);
    return TRUE;
}

BOOL CSpellController::Render(CFrustum *Frustum,float ZDistance)                                          
{
    CFrustum    ViewFrustum;  // Local viewing frustum
    float       Radius;       // Bounding radius
    sSpellTracker *SpellPtr;
    DWORD       Time;  
    if(m_Graphics == NULL) // Error checking
        return FALSE; 
    if((SpellPtr = m_SpellParent) == NULL)// Return success if no character to draw
        return TRUE;
    // Construct the viewing frustum (if none passed)
    if((m_Frustum = Frustum) == NULL)
	{ 
		ViewFrustum.Construct(m_Graphics, ZDistance);  
		m_Frustum = &ViewFrustum;
	} 
	Time = timeGetTime() / 30;// Get time to update animations (30fps)
	while(SpellPtr != NULL)// Loop through each spell and draw 
	{   
		// Draw spell if in viewing frustum  
		SpellPtr->Object.GetBounds(NULL,NULL,NULL,NULL,NULL,NULL,&Radius);                                       
		if(m_Frustum->CheckSphere(SpellPtr->XPos,SpellPtr->YPos,SpellPtr->ZPos,Radius) == TRUE)                                                                                                                                          
		{							 
			// Position object 
			SpellPtr->Object.Move(SpellPtr->XPos,SpellPtr->YPos,SpellPtr->ZPos);                                                                                       
			SpellPtr->Object.UpdateAnimation(Time, TRUE);    
			SpellPtr->Object.Render();
		}  
		SpellPtr = SpellPtr->Next;// Go to next spell
	}
	return TRUE;
}
