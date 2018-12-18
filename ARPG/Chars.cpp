/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/

#include "Global.h"

int curKillMonster;
long SkillLevelUp[8]={2,5,10,70,100,140,190,250}; 
static char *Motion[]=
{
	{"..\\Image\\emotion\\23-1.png"},//怒0
	{"..\\Image\\emotion\\23-2.png"},
	{"..\\Image\\emotion\\23-3.png"},
	{"..\\Image\\emotion\\23-4.png"},
	{"..\\Image\\emotion\\23-5.png"},
	{"..\\Image\\emotion\\23-6.png"},
	{"..\\Image\\emotion\\27-1.png"},//惊6
	{"..\\Image\\emotion\\27-2.png"},
	{"..\\Image\\emotion\\27-3.png"},
	{"..\\Image\\emotion\\27-4.png"},
	{"..\\Image\\emotion\\27-5.png"},
	{"..\\Image\\emotion\\27-6.png"},
	{"..\\Image\\emotion\\27-7.png"},
	{"..\\Image\\emotion\\27-8.png"},
	{"..\\Image\\emotion\\27-9.png"},
	{"..\\Image\\emotion\\27-10.png"},
	{"..\\Image\\emotion\\27-11.png"},
	{"..\\Image\\emotion\\27-12.png"},
	{"..\\Image\\emotion\\27-13.png"},
	{"..\\Image\\emotion\\27-14.png"},
	{"..\\Image\\emotion\\27-15.png"},
	{"..\\Image\\emotion\\35-1.png"},//疑21
	{"..\\Image\\emotion\\35-2.png"},
	{"..\\Image\\emotion\\35-3.png"},
	{"..\\Image\\emotion\\35-4.png"},
	{"..\\Image\\emotion\\35-5.png"},
	{"..\\Image\\emotion\\35-6.png"},
	{"..\\Image\\emotion\\35-7.png"},
	{"..\\Image\\emotion\\35-8.png"},
	{"..\\Image\\emotion\\35-9.png"},
	{"..\\Image\\emotion\\35-10.png"},
	{"..\\Image\\emotion\\35-11.png"},
	{"..\\Image\\emotion\\35-12.png"},
	{"..\\Image\\emotion\\35-13.png"},
	{"..\\Image\\emotion\\35-14.png"},
	{"..\\Image\\emotion\\35-15.png"},
	{"..\\Image\\emotion\\35-16.png"}

};
static int TotalFrame[100];//100角色
static int iNum[100]; 
static int ShowMessageTime[100];

CCharacterController::CCharacterController()
{
	m_Graphics          = NULL;  // Clear CGraphics pointer
    m_Font              = NULL;  // Clear CGameFont pointer
    m_Frustum           = NULL;  // Clear frustum pointer
    m_MIL               = NULL;  // Clear MIL pointer
    m_MSL               = NULL;  // Clear MSL pointer
    m_CharacterParent   = NULL;  // Clear character list
    m_NumCharacters     = 0;
    m_DefinitionFile[0] = 0;     // Clear definition filename
    m_NumMeshes         = 0;     // Clear mesh data
    m_Meshes            = NULL;
    m_TexturePath[0]    = 0;
    m_NumAnimations     = 0;     // Clear animation data
    m_Animations        = NULL;
    m_SpellController   = NULL;  // Clear spell controller
    for(int i=0;i<100;i++)//
    {
        TotalFrame[i]=0;//
	    iNum[i]=0;
    }
}

CCharacterController::~CCharacterController()
{
    Shutdown();
}

BOOL CCharacterController::Init(CGraphics *Graphics,CGameFont *Font, char *DefinitionFile,
								sItem *MIL, sSpell *MSL,long NumCharacterMeshes,
								char **MeshNames,char *MeshPath, char *TexturePath,
								long NumAnimations,sCharAnimationInfo *Anims,
								CSpellController *SpellController)								                                                                                                                                                                                                                                                                                                                 
{
    long i;
    Free();// Free prior init 
    if((m_Graphics = Graphics) == NULL || MeshNames == NULL ||DefinitionFile == NULL)// Get parent graphics object and error checking  
	    return FALSE;   
    m_Font = Font;// Get font object pointer  
    strcpy(m_DefinitionFile, DefinitionFile);// Copy definition file name   
    m_MIL = MIL;// Store MIL pointer
    m_MSL = MSL;// Store MSL pointer 
    if(MeshPath != NULL) // Copy over mesh path (or set default)
        strcpy(m_MeshPath, MeshPath);
    else//Default path is the current path
        strcpy(m_MeshPath, ".\\");  
    if(TexturePath != NULL)// Copy over texture path (or set default)
        strcpy(m_TexturePath, TexturePath);
    else
        strcpy(m_TexturePath, ".\\");  
    if((m_NumMeshes = NumCharacterMeshes))// Get mesh names 
    {
        m_Meshes = new sCharacterMeshList[NumCharacterMeshes]();
        for(i=0;i<m_NumMeshes;i++)
            strcpy(m_Meshes[i].Filename, MeshNames[i]);
    }  
    if((m_NumAnimations = NumAnimations))// Get animation data
    {
        m_Animations = new sCharAnimationInfo[m_NumAnimations]();
        for(i=0;i<m_NumAnimations;i++) 
	    {
            memcpy(&m_Animations[i], &Anims[i],sizeof(sCharAnimationInfo));                                 
        }
    } 
    m_SpellController = SpellController;// Store spell controller pointer
    return TRUE;
}

BOOL CCharacterController::Shutdown()
{
    Free();  
    delete [] m_Meshes;// Release mesh list
    m_Meshes = NULL;
    m_NumMeshes = 0;   
    m_NumAnimations = 0;// Release animation data
    delete [] m_Animations;
    m_Animations = NULL; 
    m_Graphics = NULL;// Clear graphics object  
    m_DefinitionFile[0] = 0;// Clear paths
    m_MeshPath[0] = 0;
    m_TexturePath[0] = 0; 
    m_SpellController = NULL;// Clear spell controller
    return TRUE;
}

BOOL CCharacterController::Free()
{
    // Release character structures
    delete m_CharacterParent;
    m_CharacterParent = NULL;
    m_NumCharacters = 0;
    return TRUE;
}

BOOL CCharacterController::Add(long IDNum, long Definition,long Type, long AI,
							   float XPos, float YPos, float ZPos,float Direction)                                                                                                                                                
{
    sCharacter *CharPtr;
    FILE *fp;
    char Path[MAX_PATH];
    long i;  
    if(m_Graphics==NULL || m_Meshes==NULL || !m_DefinitionFile[0])// Error checking
        return FALSE;   
    CharPtr = new sCharacter();// Allocate a new structure   
    CharPtr->Definition = Definition;// Assign data
    CharPtr->ID         = IDNum;
    CharPtr->Type       = Type;
    CharPtr->AI         = AI;
    CharPtr->XPos       = XPos;
    CharPtr->YPos       = YPos;
    CharPtr->ZPos       = ZPos;
    CharPtr->Direction  = Direction;
    CharPtr->Enabled    = TRUE;  
    CharPtr->Charge = (float)(rand() % 101);// Set a random charge amount   
    if((fp=fopen(m_DefinitionFile, "rb"))==NULL)// Load character definition 
    {
        delete CharPtr;
        return FALSE;
    }
    fseek(fp, sizeof(sCharacterDefinition)*Definition, SEEK_SET);
    fread(&CharPtr->Def, 1, sizeof(sCharacterDefinition), fp);
    fclose(fp);
    
    if(CharPtr->Def.ItemFilename[0])// Load character ICS
    {
        CharPtr->CharICS = new CCharICS();
	    CharPtr->CharSkill=new CCharskill();
    } 
    CharPtr->HealthPoints = CharPtr->Def.HealthPoints;// Set character stats(HP and MP)
    CharPtr->ManaPoints   = CharPtr->Def.ManaPoints; 
    if(!m_Meshes[CharPtr->Def.MeshNum].Count)// Load mesh and animation if needed
    {  
		m_Meshes[CharPtr->Def.MeshNum].Mesh.Load(m_Graphics,
			m_Meshes[CharPtr->Def.MeshNum].Filename,m_TexturePath);		                                           
		m_Meshes[CharPtr->Def.MeshNum].Animation.Load(
			m_Meshes[CharPtr->Def.MeshNum].Filename,&m_Meshes[CharPtr->Def.MeshNum].Mesh);                                                     		   
		if(m_NumAnimations)// Set animation loops
		{  
			for(i=0;i<m_NumAnimations;i++)       
				m_Meshes[CharPtr->Def.MeshNum].Animation.SetLoop(
				m_Animations[i].Loop,m_Animations[i].Name);                                                                                    
		} 
	}   
    CharPtr->Object.Create(m_Graphics,&m_Meshes[CharPtr->Def.MeshNum].Mesh);// Configure graphics object                                                 
    m_Meshes[CharPtr->Def.MeshNum].Count++;  
    if(m_MIL != NULL && CharPtr->Def.Weapon != -1 && m_MIL[CharPtr->Def.Weapon].MeshFilename[0])               
    {// Load and configure weapon (if any) 
		sprintf(Path, "%s%s", m_MeshPath,m_MIL[CharPtr->Def.Weapon].MeshFilename);// Build the mesh path                                      		  
		CharPtr->WeaponMesh.Load(m_Graphics, Path, m_TexturePath);// Load the weapon mesh   	
		CharPtr->WeaponObject.Create(m_Graphics,&CharPtr->WeaponMesh);// Create the weapon object                                                 		
		CharPtr->WeaponObject.Move(0.0f, 0.0f, 0.0f);// Orient and attach the weapon  
		CharPtr->WeaponObject.Rotate(0.0f, 0.0f, 0.0f); 
		CharPtr->WeaponObject.AttachToObject(&CharPtr->Object,"WeaponHand");                                            
	}
	if(m_CharacterParent != NULL)// Link in to head of list    
		m_CharacterParent->Prev = CharPtr; 
	CharPtr->Next = m_CharacterParent;
	m_CharacterParent = CharPtr;
	return TRUE;
}

BOOL CCharacterController::Remove(long IDNum)
{
    return Remove(GetCharacter(IDNum));
} 

BOOL CCharacterController::Remove(sCharacter *Character)
{   
    if(Character == NULL)// Error checking
        return FALSE;   
    m_Meshes[Character->Def.MeshNum].Count--;// Decrease mesh count
    if(!m_Meshes[Character->Def.MeshNum].Count)//release if no more
    {
        m_Meshes[Character->Def.MeshNum].Mesh.Free();
        m_Meshes[Character->Def.MeshNum].Animation.Free();
    }  
    if(Character->Prev != NULL)// Remove from list and free resource
        Character->Prev->Next = Character->Next;//two pointers will be modified and two will be deleted
    else
        m_CharacterParent = Character->Next;
    if(Character->Next != NULL)
        Character->Next->Prev = Character->Prev;
    if(Character->Prev == NULL && Character->Next == NULL)
        m_CharacterParent = NULL;
    Character->Prev = Character->Next = NULL;
    delete Character;
    return TRUE;
}

BOOL CCharacterController::Save(long IDNum, char *Filename)
{//保存角色信息,包含基本属性、能力值,当前HPMP、方位、状态,道具清单，法术清单？
    sCharacter *CharPtr;
    FILE *fp;  
    if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character in list
        return FALSE;  
    if((fp=fopen(Filename, "wb"))==NULL)// Open file
        return FALSE;
    fwrite(&CharPtr->Def, 1, sizeof(sCharacterDefinition), fp);// Output character data
    fwrite(&CharPtr->HealthPoints, 1, sizeof(long), fp);
    fwrite(&CharPtr->ManaPoints, 1, sizeof(long), fp);
    fwrite(&CharPtr->Ailments, 1, sizeof(long), fp);
    fwrite(&CharPtr->XPos, 1, sizeof(float), fp);
    fwrite(&CharPtr->YPos, 1, sizeof(float), fp);
    fwrite(&CharPtr->ZPos, 1, sizeof(float), fp);
    fwrite(&CharPtr->Direction, 1, sizeof(float), fp);
    fclose(fp);// Close file
    /*------------------------  
    if(CharPtr->CharICS != NULL)// Save inventory 
    {
        sprintf(ICSFilename, "Item%s", Filename);
        CharPtr->CharICS->Save(ICSFilename);
    }
    if(CharPtr->CharSkill!=NULL)
    {
	    sprintf(ICSFilename,"Skill%s",Filename);
	    CharPtr->CharSkill->Save(ICSFilename);
    }
    ------------------------*/
    return TRUE;
}

BOOL CCharacterController::Load(long IDNum, char *Filename)
{
	//char ICSFilename[MAX_PATH];
	sCharacter *CharPtr; 
	FILE *fp; 	
	if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character in list   
		return FALSE;	
	if((fp=fopen(Filename, "rb"))==NULL)// Open file 
		return FALSE;
	fread(&CharPtr->Def, 1, sizeof(sCharacterDefinition), fp);// Read in character data
    fread(&CharPtr->HealthPoints, 1, sizeof(long), fp);
    fread(&CharPtr->ManaPoints, 1, sizeof(long), fp);
    fread(&CharPtr->Ailments, 1, sizeof(long), fp);
    fread(&CharPtr->XPos, 1, sizeof(float), fp);
    fread(&CharPtr->YPos, 1, sizeof(float), fp);
    fread(&CharPtr->ZPos, 1, sizeof(float), fp);
    fread(&CharPtr->Direction, 1, sizeof(float), fp); 
    fclose(fp);// Close file
    /*------------------------   
    if(CharPtr->CharICS != NULL)// Load inventory
    {
        sprintf(ICSFilename, "Item%s", Filename);
        CharPtr->CharICS->Load(ICSFilename);
    }
    if(CharPtr->CharSkill!=NULL)
    {
	    sprintf(ICSFilename,"Skill%s",Filename);
	    CharPtr->CharSkill->Load(ICSFilename);
    }
    ------------------------*/
    return TRUE;
}

BOOL CCharacterController::Update(long Elapsed)
{
    sCharacter *CharPtr, *NextChar;
    float XMove, YMove, ZMove;//此帧移动的距离
    static long EffectCounter = 0;//效果计时器
    BOOL ToProcess, DeadChar; 
    if((CharPtr = m_CharacterParent) == NULL)// Return success if no characters to update
        return TRUE;
    EffectCounter += Elapsed;// Update effect counter  
    while(CharPtr != NULL)// Loop through all characters
    {   
        NextChar = CharPtr->Next;// Remember next character    
        if(CharPtr->Enabled == TRUE)// Only update if enabled, not asleep or paralyzed 
	    {              
			if(CharPtr->ActionTimer != 0)// Update action timer if in use	  
			{//动作持续时间,动作包含Attack、Spell、Death等       
				CharPtr->ActionTimer -= Elapsed;       
				if(CharPtr->ActionTimer < 0)      
					CharPtr->ActionTimer = 0;
			}     			
			if(CharPtr->MessageTimer > 0)// Update text message timer       
				CharPtr->MessageTimer -= Elapsed;     
			// Reset charge counter if attacking, spell, or item  
			if(CharPtr->Action == CHAR_ATTACK||CharPtr->Action==CHAR_SPELL||CharPtr->Action == CHAR_ITEM)                    	  
				CharPtr->Charge = 0.0f;  		  
			if(CharPtr->HealthPoints <= 0 &&CharPtr->Action != CHAR_DIE)                               	  
			{// Kill character if no health left			 
				CharPtr->HealthPoints=0;	 
				SetAction(CharPtr,CHAR_DIE);//尸体的做法：1.CHAR_DIE动画;2.尸体列表？ 
			}     		     
			ToProcess = TRUE;// Mark that processing can continue later on    			
			DeadChar = FALSE;// Mark character as still alive     		    
			if((CharPtr->Ailments & AILMENT_SLEEP) ||
				(CharPtr->Ailments & AILMENT_PARALYZE))// Don't allow an update if asleep or paralyzed                       
				ToProcess = FALSE;     			
			if(CharPtr->Action != CHAR_IDLE&&CharPtr->Action != CHAR_MOVE&&!CharPtr->ActionTimer)                               
			{// Process non-idle, non-walk actions              
				switch(CharPtr->Action) 		
				{          
				case CHAR_ATTACK:// Process attack           				         
					if(ToProcess == TRUE)             
						Attack(CharPtr, CharPtr->Victim);           
					break;    
				case CHAR_SPELL:// Manually cast a spell          					          
					if(m_SpellController != NULL && m_MSL != NULL&&ToProcess == TRUE)            			
					{             
						m_SpellController->Add(CharPtr->SpellNum,CharPtr, CharPtr->SpellTarget,
							CharPtr->XPos,CharPtr->YPos, CharPtr->ZPos,
							CharPtr->TargetX,CharPtr->TargetY,CharPtr->TargetZ);                                                                                                                                                                                                                                                                                                        
					}          
					break;       
				case CHAR_ITEM:            
					if(ToProcess == TRUE)             
						Item(CharPtr, CharPtr,CharPtr->ItemNum, CharPtr->CharItem);                                                     
					break;         
				case CHAR_DIE:           
					Death(CharPtr->Attacker, CharPtr);          
					DeadChar = TRUE;   // Mark character as dead     
					ToProcess = FALSE; // Don't allow updates            
					break;       
				}//end switch      
			}    		    
			XMove = YMove = ZMove = 0.0f;// Clear movement 			  
			if(ToProcess == TRUE)// Only continue if allowed (in case character died)
			{  				       
				if(CharPtr->Enabled==TRUE&&!CharPtr->ActionTimer&&CharPtr->Locked==FALSE)                                                                 		
				{// Only allow updates if lock/timer not in use         					         
					CharPtr->Action = CHAR_IDLE;// Reset action         					        
					if(CharPtr->Type == CHAR_PC)// Get movement           
						PCUpdate(CharPtr, Elapsed, &XMove, &YMove, &ZMove);       
					else        
						CharUpdate(CharPtr, Elapsed, &XMove,&YMove,&ZMove);      				        
					if(CheckMove(CharPtr,&XMove,&YMove,&ZMove)==FALSE) 		 
					{// Check for validity of movement (clear if invalid)           
						XMove = YMove = ZMove = 0.0f;          
						CharPtr->Action = CHAR_IDLE;        
					}      
				}      				     
				ProcessUpdate(CharPtr, XMove, YMove, ZMove);// Process movement of character      						
				CharPtr->Charge+=3;	
				if(CharPtr->Charge>100.0f)	
					CharPtr->Charge=100.0f;
			}     			   
			if(DeadChar == FALSE && CharPtr->Ailments)// Process timed ailments (only on live characters)  
			{    				       
				if(CharPtr->Ailments & AILMENT_SLEEP && (rand()%100)<4)// Sleeping characters have 4% to wake up        
					CharPtr->Ailments &= ~AILMENT_SLEEP;      				       
				if(CharPtr->Ailments & AILMENT_PARALYZE&&(rand() % 100) < 2)// Paralyzed character have 2% chance to recover                                                                 
					CharPtr->Ailments &= ~AILMENT_PARALYZE;				   
				if(CharPtr->Ailments & AILMENT_POISON &&EffectCounter >= 4000)// Posion removes 2 hp every 2 seconds                                        		
				{			
					if(CharPtr->HealthPoints>2)			
					{       				
						CharPtr->HealthPoints -= 2;    			
						SetMessage(CharPtr, "Poison -2 HP", 500,D3DCOLOR_RGBA(0,255,64,255)); 			
					}                       
				}     
			}
		}//End if character enabled   		  
		CharPtr = NextChar;// Go to next character
	} //End while	
	if(EffectCounter >= 4000)// Reset effect counter (after 4 seconds)    
		EffectCounter = 0;
	return TRUE;
}

BOOL CCharacterController::Render(long Elapsed,CFrustum *Frustum,float ZDistance)                                                                                                                               
{
    CFrustum     ViewFrustum;  // Local viewing frustum
    float        Radius;       // Bounding radius
    sCharacter  *CharPtr; 
    DWORD        Time;  
    BOOL         GotMatrices = FALSE;// Variables for printing messages
    D3DXMATRIX   matWorld, matView, matProj;
    D3DXVECTOR3  vecPos;
    D3DVIEWPORT9 vpScreen;
    float        MaxY; 
    if(m_Graphics == NULL)// Error checking
        return FALSE;   
    if((CharPtr = m_CharacterParent) == NULL)// Return success if no character to draw
        return TRUE;
	if((m_Frustum = Frustum) == NULL)// Construct the viewing frustum (if none passed) 
	{
		ViewFrustum.Construct(m_Graphics, ZDistance);   
		m_Frustum = &ViewFrustum;
	} 
    if(Elapsed == -1)// Get time to update animations (30fps) if elapsed value passed == -1
        Time = timeGetTime() / 30;	
	while(CharPtr != NULL)// Loop through each character and draw 
	{     
        if(Elapsed != -1)// Update animation based on elapsed time passed 
		{
            CharPtr->LastAnimTime += (Elapsed/30);
            Time = CharPtr->LastAnimTime;   
		}
		CharPtr->Object.GetBounds(NULL,NULL,NULL,NULL,&MaxY,NULL,&Radius);                                              
		// Draw character if in viewing frustum
		if(m_Frustum->CheckSphere(CharPtr->Object.GetXPos(),
			CharPtr->Object.GetYPos(),CharPtr->Object.GetZPos(),Radius) == TRUE)                                        
		{                                                                  
			CharPtr->Object.UpdateAnimation(Time, TRUE);   
			CharPtr->Object.Render();    			 
			if(CharPtr->Def.Weapon != -1)// Draw character's weapon       
				CharPtr->WeaponObject.Render(); 			    
			if(CharPtr->MessageTimer > 0)// Draw message if needed 
			{      				     
				if(GotMatrices == FALSE)// Get the matrices and viewport if not done already  
				{         
					GotMatrices = TRUE;       
					// Get the world, projection, and view transformations         
					D3DXMatrixIdentity(&matWorld);         
					m_Graphics->GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView);                                                    
					m_Graphics->GetDeviceCOM()->GetTransform(D3DTS_PROJECTION, &matProj);                                                   				         
					m_Graphics->GetDeviceCOM()->GetViewport(&vpScreen);// Get viewport      
				}      
				// Project coordinates to screen         
				D3DXVec3Project(&vecPos,&D3DXVECTOR3(CharPtr->XPos,CharPtr->YPos+(MaxY*1.3f),
					CharPtr->ZPos) ,&vpScreen, &matProj, &matView, &matWorld);                                                                                                                                                                         					
				if(!bMotion)// Print message		
				{		
					int iMove=CharPtr->MessageTimer;       
					m_Font->Print(CharPtr->Message,(long)vecPos.x, 
						(long)vecPos.y+iMove/10,0, 0, CharPtr->MessageColor);						                                                                 		
				}		
				else//只在第一次进行某个表情渲染时将iNum[i]赋值,以下当一定的帧数之后,将其值加一渲染第二幅图,										
				{//如此循环,当下一个MessageTimer到来启用新的iNum[i],这样就防止了调用图片的错乱			
					int i=CharPtr->ID;			
					static CTexture m_Texture;										
					if(CharPtr->MessageTimer==(ShowMessageTime[CharPtr->ID]-Elapsed))			
						iNum[i]=CharPtr->MotionNum;			
					m_Texture.Load(m_Graphics,Motion[iNum[i]]);		
					TotalFrame[i]++;		
					if(TotalFrame[i]%(CharPtr->FrameNum)==0)		
						iNum[i]++;			
					if(iNum[i]>(CharPtr->AniNum-1+CharPtr->MotionNum))			
						iNum[i]=CharPtr->MotionNum;
					m_Graphics->BeginSprite();	
					m_Texture.Blit((long)vecPos.x, (long)vecPos.y);		
					m_Graphics->EndSprite();
					m_Texture.Free();		
				}   
			}
		}   		    
		CharPtr = CharPtr->Next;// go to next character  
	}//End while
	return TRUE;
}

float CCharacterController::GetXZRadius(sCharacter *Character)
{
    float MinX, MaxX, MinZ, MaxZ;
    float x, z;
    if(Character == NULL)// Error checking
        return 0.0f;
    Character->Object.GetBounds(&MinX, NULL, &MinZ,&MaxX, NULL, &MaxZ, NULL);                                        
    x = (float)max(fabs(MinX), fabs(MaxX));
    z = (float)max(fabs(MinZ), fabs(MaxZ));
    return max(x, z);
}

/*------------------------ 
  Set/Get Functions
------------------------*/ 
sCharacter *CCharacterController::GetParentCharacter()
{
    return m_CharacterParent;
}

sCharacter *CCharacterController::GetCharacter(long IDNum)
{
    sCharacter *CharPtr;   
    if((CharPtr = m_CharacterParent) != NULL)// Scan through all characters 
	{ 
		while(CharPtr != NULL) 
		{   			      
			if(IDNum == CharPtr->ID)// Return character      
				return CharPtr;    		    
			CharPtr = CharPtr->Next;// Go to next character  
		}
	}
	return NULL;
}

float CCharacterController::GetSpeed(sCharacter *Character)
{
    float Speed;  
    if(Character == NULL)// Error checking
        return 0.0f;   
    Speed = Character->Def.Speed;// Calculate adjusted speed
    if(Character->Ailments & AILMENT_SLOW)
        Speed *= 0.5f;
    if(Character->Ailments & AILMENT_FAST)
        Speed *= 1.5f;
    if(Speed < 1.0f)//Bounds check value
        Speed = 1.0f;
    return Speed;
}

long CCharacterController::GetAttack(sCharacter *Character)
{
    long STR;  
    if(Character == NULL)// Error checking
        return 0;  
    STR = Character->Def.STR;// Calculate adjusted attack  
    if(Character->Def.Weapon != -1 && m_MIL != NULL) // Adjust attack based on item value (in %(Value/100)+1)
    {   	  
		STR+=m_MIL[Character->Def.Weapon].ModifyStr;
	}
    if(Character->Def.Armor!=-1&&m_MIL!=NULL)
	    STR+=m_MIL[Character->Def.Armor].ModifyStr;
    if(Character->Def.Helmet!=-1&&m_MIL!=NULL)
	    STR+=m_MIL[Character->Def.Helmet].ModifyStr;
    if(Character->Def.Boots!=-1&&m_MIL!=NULL)
	    STR+=m_MIL[Character->Def.Boots].ModifyStr;
    if(Character->Def.Adorn!=-1&&m_MIL!=NULL)
	    STR+=m_MIL[Character->Def.Adorn].ModifyStr;
    if(Character->Ailments & AILMENT_WEAK)
        STR = (long)((float)STR * 0.5f);
    if(Character->Ailments & AILMENT_STRONG)
        STR = (long)((float)STR * 1.5f);
    return STR;
}

long CCharacterController::GetDefense(sCharacter *Character)
{
    long DEF;  
    if(Character == NULL)// Error checking
        return 0;   
    DEF = Character->Def.DEF;// Calculate adjusted defense
    if(Character->Def.Armor != -1 && m_MIL != NULL)
        DEF +=m_MIL[Character->Def.Armor].ModifyDef;                                         
    if(Character->Def.Helmet != -1 && m_MIL != NULL)
        DEF +=m_MIL[Character->Def.Helmet].ModifyDef;                                                  
    if(Character->Def.Boots != -1 && m_MIL != NULL)
        DEF +=m_MIL[Character->Def.Boots].ModifyDef;
    if(Character->Def.Weapon!=-1&&m_MIL!=NULL)
	    DEF+=m_MIL[Character->Def.Weapon].ModifyDef;
    if(Character->Def.Adorn!=-1&&m_MIL!=NULL)
	    DEF+=m_MIL[Character->Def.Adorn].ModifyDef;
    if(Character->Ailments & AILMENT_WEAK)
        DEF = (long)((float)DEF * 0.5f);
    if(Character->Ailments & AILMENT_STRONG)
        DEF = (long)((float)DEF * 1.5f);
    if(DEF < 0)
	    DEF = 0;
    return DEF;
}

long CCharacterController::GetAgility(sCharacter *Character)
{
    long DEX; 
    if(Character == NULL)// Error checking
        return 0; 
    DEX = Character->Def.DEX;// Calculate adjusted agility
    if(Character->Def.Weapon!=-1&&m_MIL!=NULL)
	    DEX+=m_MIL[Character->Def.Weapon].ModifyDex;
    if(Character->Def.Armor!=-1&&m_MIL!=NULL)
	    DEX+=m_MIL[Character->Def.Armor].ModifyDex;
    if(Character->Def.Helmet!=-1&&m_MIL!=NULL)
	    DEX+=m_MIL[Character->Def.Helmet].ModifyDex;
    if(Character->Def.Boots!=-1&&m_MIL!=NULL)
	    DEX+=m_MIL[Character->Def.Boots].ModifyDex;
    if(Character->Def.Adorn!=-1&&m_MIL!=NULL)
	    DEX+=m_MIL[Character->Def.Adorn].ModifyDex;
    if(Character->Ailments & AILMENT_CLUMSY)
        DEX = (long)((float)DEX * 0.75f);
    if(Character->Ailments & AILMENT_SUREFOOTED)
        DEX = (long)((float)DEX * 1.5f);
    return DEX;
}

long CCharacterController::GetResistance(sCharacter *Character)
{
    long RES;  
    if(Character == NULL)// Error checking
        return 0;
    RES = Character->Def.RES;// Calculate adjusted resistance
    if(Character->Def.Weapon!=-1&&m_MIL!=NULL)
	    RES+=m_MIL[Character->Def.Weapon].ModifyRes;
    if(Character->Def.Armor!=-1&&m_MIL!=NULL)
	    RES+=m_MIL[Character->Def.Armor].ModifyRes;
    if(Character->Def.Helmet!=-1&&m_MIL!=NULL)
	    RES+=m_MIL[Character->Def.Helmet].ModifyRes;
    if(Character->Def.Boots!=-1&&m_MIL!=NULL)
	    RES+=m_MIL[Character->Def.Boots].ModifyRes;
    if(Character->Def.Adorn!=-1&&m_MIL!=NULL)
	    RES+=m_MIL[Character->Def.Adorn].ModifyRes;
    if(Character->Ailments & AILMENT_ENCHANTED)
        RES = (long)((float)RES * 0.5f);
    if(Character->Ailments & AILMENT_BARRIER)
        RES = (long)((float)RES * 1.5f);
    return RES;
}

long CCharacterController::GetMental(sCharacter *Character)
{
    long ENE; 
    if(Character == NULL)// Error checking
        return 0;  
    ENE = Character->Def.ENE;// Calculate adjusted mental
    if(Character->Def.Weapon!=-1&&m_MIL!=NULL)
	    ENE+=m_MIL[Character->Def.Weapon].ModifyEne;
    if(Character->Def.Armor!=-1&&m_MIL!=NULL)
	    ENE+=m_MIL[Character->Def.Armor].ModifyEne;
    if(Character->Def.Helmet!=-1&&m_MIL!=NULL)
	    ENE+=m_MIL[Character->Def.Helmet].ModifyEne;
    if(Character->Def.Boots!=-1&&m_MIL!=NULL)
	    ENE+=m_MIL[Character->Def.Boots].ModifyEne;
    if(Character->Def.Adorn!=-1&&m_MIL!=NULL)
	    ENE+=m_MIL[Character->Def.Adorn].ModifyEne;
    if(Character->Ailments & AILMENT_DUMBFOUNDED)
        ENE = (long)((float)ENE * 0.5f);
    return ENE;
}

long CCharacterController::GetToHit(sCharacter *Character)
{
    long HIT; 
    if(Character == NULL)// Error checking
        return 0;  
    HIT = Character->Def.HIT;// Calculate adjusted to hit
    if(Character->Def.Weapon!=-1&&m_MIL!=NULL)
	    HIT+=m_MIL[Character->Def.Weapon].ModifyHit;
    if(Character->Def.Armor!=-1&&m_MIL!=NULL)
	    HIT+=m_MIL[Character->Def.Armor].ModifyHit;
    if(Character->Def.Helmet!=-1&&m_MIL!=NULL)
	    HIT+=m_MIL[Character->Def.Helmet].ModifyHit;
    if(Character->Def.Boots!=-1&&m_MIL!=NULL)
	    HIT+=m_MIL[Character->Def.Boots].ModifyHit;
    if(Character->Def.Adorn!=-1&&m_MIL!=NULL)
	    HIT+=m_MIL[Character->Def.Adorn].ModifyHit;
    if(Character->Ailments & AILMENT_BLIND)
        HIT = (long)((float)HIT * 0.75f);
    if(Character->Ailments & AILMENT_HAWKEYE)
        HIT = (long)((float)HIT * 1.5f);
    return HIT;
}

long CCharacterController::GetHP(sCharacter *Character)
{
	long HP;
	if(Character==NULL)
		return 0;
	HP=Character->Def.HealthPoints;
	if(Character->Def.Weapon!=-1&&m_MIL!=NULL)  
		HP+=m_MIL[Character->Def.Weapon].ModifyHP; 
	if(Character->Def.Armor!=-1&&m_MIL!=NULL)  
		HP+=m_MIL[Character->Def.Armor].ModifyHP;
    if(Character->Def.Helmet!=-1&&m_MIL!=NULL)
	    HP+=m_MIL[Character->Def.Helmet].ModifyHP;
    if(Character->Def.Boots!=-1&&m_MIL!=NULL)
	    HP+=m_MIL[Character->Def.Boots].ModifyHP;
    if(Character->Def.Adorn!=-1&&m_MIL!=NULL)
	    HP+=m_MIL[Character->Def.Adorn].ModifyHP;
    return HP;
}

long CCharacterController::GetMP(sCharacter *Character)
{		
	long MP;
	if(Character==NULL)
		return 0;
	MP=Character->Def.ManaPoints; 
	if(Character->Def.Weapon!=-1&&m_MIL!=NULL)  
		MP+=m_MIL[Character->Def.Weapon].ModifyMP;
    if(Character->Def.Armor!=-1&&m_MIL!=NULL)
	    MP+=m_MIL[Character->Def.Armor].ModifyMP;
    if(Character->Def.Helmet!=-1&&m_MIL!=NULL)
	    MP+=m_MIL[Character->Def.Helmet].ModifyMP;
    if(Character->Def.Boots!=-1&&m_MIL!=NULL)
	    MP+=m_MIL[Character->Def.Boots].ModifyMP;
    if(Character->Def.Adorn!=-1&&m_MIL!=NULL)
	    MP+=m_MIL[Character->Def.Adorn].ModifyMP;
    return MP;
}

float CCharacterController::GetCharge(sCharacter *Character)
{
	float Charge;
	if(Character == NULL)// Error checking  
		return 0;	
	Charge = Character->Def.ChargeRate;// Calculate adjusted charge
	if(Character->Ailments & AILMENT_SLOW) 
		Charge = Charge * 0.75f;
	if(Character->Ailments & AILMENT_FAST) 
		Charge = Charge * 1.5f;
	return Charge;
}

CCharICS *CCharacterController::GetICS(long IDNum)
{
    sCharacter *CharPtr;
    if((CharPtr = GetCharacter(IDNum)) == NULL)
        return NULL;
    return CharPtr->CharICS;
}

BOOL CCharacterController::SetLock(long IDNum, BOOL State)
{
    sCharacter *CharPtr; 
    if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character
        return FALSE;  
    CharPtr->Locked = State;// Set new value
    return TRUE;
}

BOOL CCharacterController::SetActionTimer(long IDNum, long Timer)
{
    sCharacter *CharPtr; 
    if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character
        return FALSE;
    CharPtr->ActionTimer = Timer; // Set new value
    return TRUE;
}

BOOL CCharacterController::SetAction(sCharacter *Character,long Action,long AddTime)                                                                                    
{
	long MeshNum; 
    if(Character == NULL)// Error checking
        return FALSE; 
    if(Action == CHAR_ATTACK || Action == CHAR_SPELL ||Action == CHAR_ITEM)
    {// Make sure attack, spell, and item has supporting charge 
		if(Character->Charge < 100.0f)    
			return FALSE;
	}	
	Character->Action = Action;// Set action
	ActionSound(Character);// Play sound effect	
	MeshNum = Character->Def.MeshNum;// Get mesh number
	if(AddTime == -1)// Set action time (or set to 1 is addtime = -1)
		Character->ActionTimer = 1;
	else  
		Character->ActionTimer = AddTime + 
		m_Meshes[MeshNum].Animation.GetLength(m_Animations[Action].Name) * 30;                                                           
	return TRUE;
}

BOOL CCharacterController::SetDistance(long IDNum, float Distance)                                     
{
	sCharacter *CharPtr;	 
	if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character 
		return FALSE;	
	CharPtr->Distance = Distance;// Set new value
	return TRUE;
}

float CCharacterController::GetDistance(long IDNum)
{
    sCharacter *CharPtr;
    if((CharPtr = GetCharacter(IDNum)) == NULL)
        return 0.0f;
    return CharPtr->Distance;
}

BOOL CCharacterController::SetRoute(long IDNum,long NumPoints,sRoutePoint *Route)                                                                                                
{
    sCharacter *CharPtr;
    if((CharPtr = GetCharacter(IDNum)) == NULL) // Get pointer to character
        return FALSE;  
    delete [] CharPtr->Route;// Free old route
    CharPtr->Route = NULL; 
    if((CharPtr->NumPoints = NumPoints) != NULL)// Set new route 
	{ 
		CharPtr->Route = new sRoutePoint[NumPoints];  
		memcpy(CharPtr->Route,Route,NumPoints*sizeof(sRoutePoint));  
		CharPtr->CurrentPoint = 0;
	}
	return TRUE;
}

BOOL CCharacterController::SetScript(long IDNum,char *ScriptFilename)                                                  
{
	sCharacter *CharPtr;
	if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character   
		return FALSE;	
	strcpy(CharPtr->ScriptFilename, ScriptFilename);// Set new script
	return TRUE;
}

char *CCharacterController::GetScript(long IDNum)
{
    sCharacter *CharPtr;
    if((CharPtr = GetCharacter(IDNum)) == NULL)
        return NULL;
    return CharPtr->ScriptFilename;
}

BOOL CCharacterController::SetEnable(long IDNum, BOOL Enabled)
{
    sCharacter *CharPtr; 
    if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character
        return FALSE;  
    CharPtr->Enabled = Enabled;// Set new value
    return TRUE;
}

BOOL CCharacterController::GetEnable(long IDNum)
{
    sCharacter *CharPtr;
    if((CharPtr = GetCharacter(IDNum)) == NULL)
        return FALSE;
    return CharPtr->Enabled;
}

BOOL CCharacterController::SetMessage(sCharacter *Character,char *Text, long Timer,D3DCOLOR Color)                                                                          
{
    strcpy(Character->Message, Text);
    Character->MessageTimer = Timer;
    Character->MessageColor = Color;
    bMotion=false;
    return TRUE;
}
bool CCharacterController::SetEmotion(sCharacter *Character,int StartNum,int PicNum,long Timer,int FrameNum)
{
	bMotion=true;
	Character->MessageTimer=Timer;
	Character->MotionNum=StartNum;
	Character->AniNum=PicNum;
	Character->FrameNum=FrameNum;
	ShowMessageTime[Character->ID]=Timer;
	return true;
}

BOOL CCharacterController::Move(long IDNum,float XPos, float YPos, float ZPos)                                                                                              
{
    sCharacter *CharPtr; 
    if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character
        return FALSE;
    // Set new values
    CharPtr->XPos = XPos;
    CharPtr->YPos = YPos;
    CharPtr->ZPos = ZPos;
    return TRUE;
}

BOOL CCharacterController::GetPosition(long IDNum,float *XPos, float *YPos, float *ZPos)                                                                                       
{
    sCharacter *CharPtr;
    if((CharPtr = GetCharacter(IDNum)) == NULL)
        return FALSE;
    if(XPos != NULL)
        *XPos = CharPtr->XPos;
    if(YPos != NULL)
        *YPos = CharPtr->YPos;
    if(ZPos != NULL)
        *ZPos = CharPtr->ZPos;
    return TRUE;
}

BOOL CCharacterController::SetBounds(long IDNum,float MinX, float MinY, float MinZ,
									 float MaxX, float MaxY, float MaxZ)                                                                                                                  
{
    sCharacter *CharPtr;  
    if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character
        return FALSE;
    // Set new values
    CharPtr->MinX = min(MinX, MaxX);
    CharPtr->MinY = min(MinY, MaxY);
    CharPtr->MinZ = min(MinZ, MaxZ);
    CharPtr->MaxX = max(MinX, MaxX);
    CharPtr->MaxY = max(MinY, MaxY);
    CharPtr->MaxZ = max(MinZ, MaxZ);
    return TRUE;
}   

BOOL CCharacterController::GetBounds(long IDNum,float *MinX, float *MinY, float *MinZ,
									 float *MaxX, float *MaxY, float *MaxZ)                                                                                                                
{
    sCharacter *CharPtr;
    if((CharPtr = GetCharacter(IDNum)) == NULL)
        return FALSE;
    if(MinX != NULL)
        *MinX = CharPtr->MinX;
    if(MinY != NULL)
        *MinY = CharPtr->MinY;
    if(MinZ != NULL)
        *MinZ = CharPtr->MinZ;
    if(MaxX != NULL)
        *MaxX = CharPtr->MaxX;
    if(MaxY != NULL)
        *MaxY = CharPtr->MaxY;
    if(MaxZ != NULL)
        *MaxZ = CharPtr->MaxZ;
    return TRUE;
}

BOOL CCharacterController::SetType(long IDNum, long Type)
{
    sCharacter *CharPtr; 
    if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character
        return FALSE;
    CharPtr->Type = Type;// Set new value
    return TRUE;
}

long CCharacterController::GetType(long IDNum)
{
    sCharacter *CharPtr;
    if((CharPtr = GetCharacter(IDNum)) == NULL)
        return 0;
    return CharPtr->Type;
}

BOOL CCharacterController::SetAI(long IDNum, long Type)
{
    sCharacter *CharPtr; 
    if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character
        return FALSE; 
    CharPtr->AI = Type;// Set new value
    return TRUE;
}

long CCharacterController::GetAI(long IDNum)
{
    sCharacter *CharPtr;
    if((CharPtr = GetCharacter(IDNum)) == NULL)
        return 0;
    return CharPtr->AI;
}

BOOL CCharacterController::SetTargetCharacter(long IDNum,long TargetNum)                                              
{
    sCharacter *CharPtr, *CharTarget; 
    if((CharPtr = GetCharacter(IDNum)) == NULL)// Get pointer to character
        return FALSE;
    if(TargetNum == -1)// Clear if TargetNum == -1
        CharPtr->TargetChar = NULL;
    else
	{ 
		CharTarget = m_CharacterParent;// Scan through list and target 1st TargetNum found 
		while(CharTarget != NULL) 
		{   
			if(CharTarget->ID == TargetNum)
			{
				CharPtr->TargetChar = CharTarget; 
				break;   
			}   
			CharTarget = CharTarget->Next; 
		}  		 
		if(CharTarget == NULL)// Clear target if not found in list  
			CharPtr->TargetChar = NULL;
	}
	return TRUE;
}

BOOL CCharacterController::CharUpdate(sCharacter *Character, long Elapsed,
									  float *XMove, float *YMove, float *ZMove)                                                               
{
    float MoveX, MoveY, MoveZ, Speed;
    float XDiff, YDiff, ZDiff, Dist, Radius;
    float y1, y2;
    long  i, SpellNum;
    BOOL SpellCast;
    sCharacter *CharPtr;
    if(Character == NULL)// Error checking
        return FALSE;  
    MoveX = MoveY = MoveZ = 0.0f;// Clear movements and action
    Speed = (float)Elapsed / 1000.0f * GetSpeed(Character);// Calculate movement speed
    switch(Character->AI)// Move character based on their type 
	{  
	case CHAR_STAND:     
		break;   
    case CHAR_WANDER: 	
        Character->Distance -= Elapsed;// Calculate new distance and direction if needed
        if(Character->Distance <= 0.0f) 
		{
			// Calculate a new distance to travel      
			Character->Distance = (float)(rand() % 2000) + 2000.0f;     
			// Calculate a new direction  
			Character->Direction = (float)(rand()%360)*0.01744444f;
		}   	   
		if(Character->Distance > 1000.0f)// Process walk or stand still 
		{     
			MoveX = (float)sin(Character->Direction) * Speed;       
			MoveZ = (float)cos(Character->Direction) * Speed; 
			Character->Action = CHAR_MOVE;  
		} 
		else  		     
			Character->Action = CHAR_IDLE;// Stand still for one second  
		break;
    case CHAR_ROUTE:     
		// Determine if character has reached point    
		XDiff = (float)fabs(Character->XPos -Character->Route[Character->CurrentPoint].XPos);                                 
		YDiff = (float)fabs(Character->YPos -Character->Route[Character->CurrentPoint].YPos);                                    
		ZDiff = (float)fabs(Character->ZPos -Character->Route[Character->CurrentPoint].ZPos);                                  
		Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;
		Radius = GetXZRadius(Character) * 0.25f;//        	
		if(Dist < (Radius*Radius))// Go to next point if reached 
		{     
			Character->CurrentPoint++;     
			if(Character->CurrentPoint >= Character->NumPoints)       
				Character->CurrentPoint = 0;    
			// Calculate new differences and distance  
			XDiff = (float)fabs(Character->XPos -Character->Route[Character->CurrentPoint].XPos);                                     
			YDiff = (float)fabs(Character->YPos -Character->Route[Character->CurrentPoint].YPos);                                
			ZDiff = (float)fabs(Character->ZPos -Character->Route[Character->CurrentPoint].ZPos);                             
			Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;      
		}   	 
		Dist = (float)sqrt(Dist);// Setup movement towards target 
		if(Speed > Dist)      
			Speed = Dist;
		MoveX=(Character->Route[Character->CurrentPoint].XPos -Character->XPos) / Dist * Speed;           
		MoveZ=(Character->Route[Character->CurrentPoint].ZPos -Character->ZPos) / Dist * Speed;                	 
		Character->Direction = (float)atan2(MoveX, MoveZ);// Set new direction    	
		Character->Action = CHAR_MOVE;// Set new action
		break;
    case CHAR_FOLLOW: 
		if(Character->TargetChar != NULL) 
		{     
			// Check distance between characters     
			XDiff = (float)fabs(Character->XPos -Character->TargetChar->XPos);                                              
			YDiff = (float)fabs(Character->YPos -Character->TargetChar->YPos);                                          
			ZDiff = (float)fabs(Character->ZPos -Character->TargetChar->ZPos);                                              
			Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;   
			// Update if further then distance    
			if(Dist > (Character->Distance*Character->Distance)) 
			{       
				// Setup movement towards target        
				Dist = (float)sqrt(Dist);   
				if(Speed > Dist)        
					Speed = Dist;       
				MoveX = (Character->TargetChar->XPos -Character->XPos) / Dist * Speed;                                     
				MoveZ = (Character->TargetChar->ZPos -Character->ZPos) / Dist * Speed;                                 			       
				Character->Direction = (float)atan2(MoveX, MoveZ);// Set new direction      			        
				Character->Action = CHAR_MOVE;// Set new action     
			}   
		}
		break;
    case CHAR_EVADE:     
		if(Character->TargetChar != NULL) 
		{   
			// Check distance between characters     
			XDiff = (float)fabs(Character->XPos -Character->TargetChar->XPos);                                                 
			YDiff = (float)fabs(Character->YPos -Character->TargetChar->YPos);                                            
			ZDiff = (float)fabs(Character->ZPos -Character->TargetChar->ZPos);                                            
			Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;
			// Update if closer then distance     
			if(Dist < (Character->Distance*Character->Distance)) 
			{    
				// Setup movement away from target       
				Dist = (float)sqrt(Dist);        
				if(Speed > Dist)         
					Speed = Dist;  
				MoveX = -(Character->TargetChar->XPos -Character->XPos) / Dist * Speed;                             				      
				MoveZ = -(Character->TargetChar->ZPos -Character->ZPos) / Dist * Speed;                           				         			        
				Character->Direction = (float)atan2(MoveX, MoveZ);// Set new direction     				      
				Character->Action = CHAR_MOVE;// Set new action    
			}  
		}
		break;
	}//End switch
	if(Character->Type == CHAR_MONSTER &&Character->Charge >= 100.0f)// Process monster actions if at full charge                                                    
	{ 	
		if((rand() % 100) <= Character->Def.ToAttack)// Determine chance of attacking     
		{  		
			CharPtr = m_CharacterParent; 
			while(CharPtr != NULL)// Scan through list and pick a character
			{    
				// Randomly pick enabled target (a PC),and make sure the target is not hurt or dead.      
				if(CharPtr != Character && CharPtr->Type==CHAR_PC &&(rand() % 100) < 50 
					&&CharPtr->Enabled == TRUE &&CharPtr->Action != CHAR_DIE &&CharPtr->Action != CHAR_HURT)                                                                                                           
				{        
					// Get distance to target    
					XDiff = (float)fabs(Character->XPos - CharPtr->XPos);        
					YDiff = (float)fabs(Character->YPos - CharPtr->YPos);   
					ZDiff = (float)fabs(Character->ZPos - CharPtr->ZPos);       
					Dist  = XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff;      
					// Make sure in range to attack      
					Radius = GetXZRadius(Character);       
					Radius += Character->Def.Range;      				         
					if((Radius*Radius) >= Dist)// Attack if in range 
					{           					     
						Character->Victim = CharPtr;// Set attack data               
						CharPtr->Attacker = Character;       					        
						MoveX = MoveY = MoveZ = 0.0f;// Clear movement       
						// Point towards target character          
						XDiff = CharPtr->XPos - Character->XPos;         
						ZDiff = CharPtr->ZPos - Character->ZPos;     
						Character->Direction = (float)atan2(XDiff, ZDiff);           						        
						SetAction(Character, CHAR_ATTACK);// Perform attack action   
						break;         
					}     
				}      			      
				CharPtr = CharPtr->Next;// Go to next character     
			}//End while
		} 
		else if((rand() % 100) <= Character->Def.ToMagic // Determine chance of spell casting
			&&m_MSL != NULL && m_SpellController != NULL)  			                      
		{    		      
			SpellCast = FALSE;// Flag no spells cast  
			// If health is less then half, then there's a 50% chance of healing (if the monster knows any heal spells)
			if(Character->HealthPoints <=(Character->Def.HealthPoints / 2))                                                     
			{			    
				for(i=0;i<64;i++)// Search for a known heal spell 
				{
         
					if(m_MSL[i].Name[0] &&m_MSL[i].Effect == ALTER_HEALTH 
						&&m_MSL[i].Value[0] > 0.0f&&Character->ManaPoints >= m_MSL[i].Cost  					
						&&Character->Def.MagiCSpells[i/32] & (1<<(i&31)))                                                                                                                       
					{           								         				
						if((rand() % 100) < 50)// This is the spell, determine chance to heal 				 
						{             					 
							// Set spell data           					
							Character->Victim      = Character;          					 
							Character->Attacker    = Character;
   							Character->SpellNum    = i;            				
							Character->SpellTarget = CHAR_MONSTER;//             	 
							// Store target coordinates           					
							Character->TargetX = Character->XPos;         				
							Character->TargetY = Character->YPos;           					
							Character->TargetZ = Character->ZPos;         												            					
							MoveX = MoveY = MoveZ = 0.0f;// Clear movement        										           				
							SetAction(Character, CHAR_SPELL);// Perform spell action       										           					
							SpellCast = TRUE;// Flag spell as cast          				
							break;      				
						}        			 
					}   
				}//End for
			}   
			// If there are bad status ailments, then there's a 50% chance of dispeling magic.    
			if(Character->Ailments & AILMENT_POISON ||Character->Ailments & AILMENT_SLEEP ||                    			                        
				Character->Ailments & AILMENT_PARALYZE ||Character->Ailments & AILMENT_ENCHANTED ||                  		             
				Character->Ailments & AILMENT_DUMBFOUNDED ||Character->Ailments & AILMENT_SLOW ||                 			                 
				Character->Ailments & AILMENT_BLIND ||Character->Ailments & AILMENT_SILENCED &&                   
				SpellCast == FALSE)				                 		 
			{             				      
				for(i=0;i<64;i++)// Search for a known dispel spell 
				{       
					if(m_MSL[i].Name[0] && m_MSL[i].Effect == DISPEL_MAGIC&&
						 Character->ManaPoints >= m_MSL[i].Cost &&Character->Def.MagiCSpells[i/32] & (1<<(i&31)))                                                                                     
					{          										          				
						if((rand() % 100) < 50)// This is the spell, determine chance to dispel 				
						{             					
							// Set spell data      				
							Character->Victim      = Character;           			
							Character->Attacker    = Character;            				
							Character->SpellNum    = i;             				
							Character->SpellTarget = CHAR_MONSTER;                  				 
							// Store target coordinates           					
							Character->TargetX = Character->XPos;          				
							Character->TargetY = Character->YPos;         			
							Character->TargetZ = Character->ZPos;            												             					
							MoveX = MoveY = MoveZ = 0.0f;// Clear movement             					 						            				
							SetAction(Character, CHAR_SPELL);// Perform spell action            												         				
							SpellCast = TRUE;// Flag spell as cast             					
							break;           				 
						}         			 
					}       
				}//End for 
			}      			  
			if(SpellCast == FALSE)// If now spells already cast, then pick a random one 
			{   			     
				SpellNum = rand() % 64;// Pick a random spell to attack with       			       
				for(i=0;i<64;i++)// Scan through list until a spell is found the monster can cast. 
				{   
					if(m_MSL[SpellNum].Name[0] &&
						Character->Def.MagiCSpells[SpellNum / 32] &(1 << (SpellNum & 31))&&                                    					                                               							
						Character->ManaPoints >= m_MSL[SpellNum].Cost &&(rand() % 100) < 50)             								   
					{          						           
						CharPtr = m_CharacterParent;        
						while(CharPtr != NULL)// Scan through list and pick a character 
						{             
							// Randomly pick an enabled target (a PC),               
							// and make sure the target is not hurt or dead.              
							// Also, don't cast self-targeting spells here.            
							if(CharPtr != Character &&  CharPtr->Type == CHAR_PC && 
								m_MSL[SpellNum].Target != TARGET_SELF&&(rand() % 100) < 50&& 
								CharPtr->Enabled == TRUE &&CharPtr->Action != CHAR_DIE &&CharPtr->Action != CHAR_HURT)                                                                                                                                                                                               
							{              
								// Get heights of attacker and target for line of sight checking              
								Character->Object.GetBounds(NULL,NULL,NULL,NULL,&y1,NULL,NULL);                                                       
								y1 = (y1 * 0.5f) + Character->YPos;              
								CharPtr->Object.GetBounds(NULL,NULL,NULL,NULL,&y2,NULL,NULL);                                                              
								y2 = (y2 * 0.5f) + CharPtr->YPos;           
								// Get distance to target             
								XDiff = (float)fabs(Character->XPos -CharPtr->XPos);                                                          
								YDiff = (float)fabs(Character->YPos -CharPtr->YPos);                                                        
								ZDiff = (float)fabs(Character->ZPos -CharPtr->ZPos);                                                      
								Dist  = XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff;         								            
								Radius = GetXZRadius(CharPtr);          
								Dist -= (Radius*Radius);// Reduce distance by character's radius                								      
								Radius = m_MSL[SpellNum].Distance;// Get spell radius                   
								// Make sure target is in range and in sight
                
								if(LineOfSight(Character, CharPtr,Character->XPos, y1,Character->ZPos,
									CharPtr->XPos, y2,CharPtr->ZPos) &&Dist <= (Radius * Radius))                                                                                                                                                                                                            
								{                					  
									// Set the spell data            
									Character->Victim      = CharPtr;                
									CharPtr->Attacker      = Character;               
									Character->SpellNum    = SpellNum;               
									Character->SpellTarget = CHAR_PC;               
									// Store target coordinates                 
									Character->TargetX = CharPtr->XPos;               
									Character->TargetY = CharPtr->YPos;                 
									Character->TargetZ = CharPtr->ZPos;                 
									// Face toward target (only if not self)                 
									if(m_MSL[SpellNum].Target != TARGET_SELF)
									{                   
										XDiff = CharPtr->XPos - Character->XPos;                    
										ZDiff = CharPtr->ZPos - Character->ZPos;                   
										Character->Direction =(float)atan2(XDiff, ZDiff);                                                                      
									}                								                
									MoveX = MoveY = MoveZ = 0.0f;// Clear movement              								                
									SetAction(Character, CHAR_SPELL);// Set the spell action                 								                 
									SpellCast = TRUE;// Flag spell as cast                  
									break;               
								}            
							}          							              
							CharPtr = CharPtr->Next;// Go to next character          
						}//End while            
						break;          
					}       					          
					SpellNum = (SpellNum + 1) % 64;// Go to next spell        
				}//End for
			}//End if  		
			if(SpellCast == FALSE) 
			{// If still no spell cast, try casting a known self-enhancing ailment-effecting spell.       
				for(i=0;i<64;i++) 
				{       
					if(m_MSL[i].Name[0] &&m_MSL[i].Effect == CAUSE_AILMENT &&
						Character->ManaPoints >= m_MSL[i].Cost && 
						Character->Def.MagiCSpells[i/32]&(1<<(i&31)) &&(rand()%100) < 10)
					{                                                                                                              						        
						if((long)m_MSL[i].Value[0]&AILMENT_STRONG ||(long)m_MSL[i].Value[0]&AILMENT_BARRIER ||
							(long)m_MSL[i].Value[0]&AILMENT_SUREFOOTED ||(long)m_MSL[i].Value[0]&AILMENT_FAST ||
							(long)m_MSL[i].Value[0]&AILMENT_HAWKEYE)                                                                              
						{// Make sure it's self-enhancing               							         
							if(!(Character->Ailments & (long)m_MSL[i].Value[0]))// Make sure ailment not already set                                                         
							{              
								// Set spell data               
								Character->Victim      = Character;                
								Character->Attacker    = Character;               
								Character->SpellNum    = i;              
								Character->SpellTarget = CHAR_MONSTER;                          
								// Store target coordinates               
								Character->TargetX = Character->XPos;             
								Character->TargetY = Character->YPos;               
								Character->TargetZ = Character->ZPos;               								               
								MoveX = MoveY = MoveZ = 0.0f;// Clear movement              								               
								SetAction(Character, CHAR_SPELL);// Perform spell action                
								break;             
							}            
						}          
					}        
				}//End for     
			}//End if
        }//End else if
    }//End if    
    *XMove = MoveX;// Store movement and return
    *YMove = MoveY;
    *ZMove = MoveZ;
    return TRUE;
}

BOOL CCharacterController::CheckMove(sCharacter *Character,float *XMove, float *YMove, float *ZMove)                  
{ 
	sCharacter *CharPtr;
    float XDiff, YDiff, ZDiff, Dist;
    float Radius1, Radius2;
    float XPos, YPos, ZPos;
    float MinX, MaxX, MinZ, MaxZ;
    if(Character == NULL)// Error checking
        return FALSE;  // Don't allow movement
    XPos = Character->XPos + (*XMove);
    YPos = Character->YPos + (*YMove);
    ZPos = Character->ZPos + (*ZMove); 
    Character->Object.GetBounds(&MinX, NULL, &MinZ,&MaxX, NULL, &MaxZ, NULL);                                         
    Radius1 = max(MaxX-MinX, MaxZ-MinZ) * 0.5f; // Get character's X/Z radius 
    if((CharPtr = m_CharacterParent) != NULL)// Check movement against other characters
    {
        while(CharPtr != NULL) 
	    {          
            if(Character != CharPtr && CharPtr->Enabled == TRUE)// Don't check against self or disabled characters   
			{  				    
				if(Character->Type == CHAR_PC &&CharPtr->Type == CHAR_PC)// Don't check against other PC characters                      			
					break;           
				// Get distance between characters     
				XDiff = (float)fabs(XPos - CharPtr->XPos);    
				YDiff = (float)fabs(YPos - CharPtr->YPos);     
				ZDiff = (float)fabs(ZPos - CharPtr->ZPos);      
				Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;  
				// Get checking character's X/Z bounding radius    
				CharPtr->Object.GetBounds(&MinX, NULL, &MinZ,&MaxX, NULL, &MaxZ, NULL);                                       
				Radius2 = max(MaxX-MinX, MaxZ-MinZ) * 0.5f;     			    
				if(Dist <= (Radius1*Radius1 + Radius2*Radius2))// Don't allow movement if intersecting         
					return FALSE;  
			}    
			CharPtr = CharPtr->Next;  
		}
	}
	if(Character->MinX != Character->MaxX)// Bounds check movement if MinX != MaxX
	{   
		if(XPos < Character->MinX || XPos > Character->MaxX)  
			*XMove = 0.0f; 
		if(YPos < Character->MinY || YPos > Character->MaxY)  
			*YMove = 0.0f;
		if(ZPos < Character->MinZ || ZPos > Character->MaxZ) 
			*ZMove = 0.0f;  		  
		if(!(*XMove) && !(*YMove) && !(*ZMove))// Return no movement at all   
			return FALSE; 
	}	
	if(ValidateMove(Character, XMove, YMove, ZMove) == FALSE)// Call overloaded check custom collisions (maps, etc) 
		return FALSE;  // Don't allow movement
	return TRUE;
}

BOOL CCharacterController::ProcessUpdate(sCharacter *Character,float XMove, float YMove, float ZMove)                                                               
{//3d模型的更新(移动旋转设置新动画) 
	// Move character
	Character->XPos += XMove;
	Character->YPos += YMove;
	Character->ZPos += ZMove;
	// Move object and point in direction
	Character->Object.Move(Character->XPos,Character->YPos,Character->ZPos);                                                                                                              					
	Character->Object.Rotate(0.0f, Character->Direction, 0.0f);  	
	if(Character->LastAnim != Character->Action)// Set new animation 
	{   
		Character->LastAnim = Character->Action; 
		if(m_NumAnimations && m_Animations != NULL)
		{  
			Character->LastAnimTime = timeGetTime() / 30;    
			Character->Object.SetAnimation(&m_Meshes[Character->Def.MeshNum].Animation ,
				m_Animations[Character->Action].Name,Character->LastAnimTime);                                                                                      
		} 
	}  
	return TRUE;
}

BOOL CCharacterController::Attack(sCharacter *Attacker,sCharacter *Victim)                                         
{ 
	if(Attacker == NULL || Victim == NULL)// Error checking    
		return FALSE;
	if(Victim->Action == CHAR_DIE || Victim->Action == CHAR_HURT)// Don't attack dead or hurt people  
		return FALSE;
	Victim->Attacker = Attacker;// Set attacker and victim
	Attacker->Victim = Victim;
	//判定物理攻击是否Miss 1:rand()%100>=攻击者命中 rand()%100<=被攻击者身法; 2:rand()%100>=(攻击者命中-被攻击者身法)
	long value=GetToHit(Attacker)-GetAgility(Victim);
	if(rand()%100>(value+50)) 
	{	 
		SetMessage(Victim,"Miss",500);	 
		return FALSE;
	}
	if(Victim->Ailments & AILMENT_SLEEP)// If character is asleep, randomly wake them up (50% chance)
	{ 
		if((rand() % 100) < 50)  
			Victim->Ailments &= ~AILMENT_SLEEP;
	}
	Damage(Victim, TRUE, GetAttack(Attacker), -1, -1);// Attack landed, apply damage
	return TRUE;
}

BOOL CCharacterController::Damage(sCharacter *Victim,BOOL PhysicalAttack,long Amount,long DmgClass,long CureClass)                                                                                                                                                                                     
{ 
	char Text[128];
    float Range;
    long  DmgAmount;
    if(Victim == NULL)// Error checking
        return FALSE;
    // Can't attack if already dead or being hurt (or not enabled)
    if(Victim->Enabled == FALSE ||Victim->Action==CHAR_DIE ||Victim->Action==CHAR_HURT)                                                                  
        return FALSE; 
    if(PhysicalAttack == TRUE)// Adjust for defense if physical attack 
    {
        //攻击波动(-20%--+10%)
        Range     = (float)((rand() % 40) + 80) / 100.0f;
        DmgAmount = (long)((float)Amount * Range);
        // Subtract for defense of victim
        Range     = (float)((rand() % 20) + 80) / 100.0f;
        DmgAmount -= (long)((float)GetDefense(Victim) * Range);
    } 
    else
    {
        // Adjust for magical attack
	    Range=(float)((rand()%40)+80)/100.0f;
	    DmgAmount=(long)((float)Amount*Range);
	    Range=(float)((rand()%20)+80)/100.0f;
	    DmgAmount-=(long)((float)GetResistance(Victim)*Range); 
    } 
    if(DmgAmount < 0)// Bounds check value    
		DmgAmount = 0;
    if(Victim->Def.Class == DmgClass)// Check for double damage
        DmgAmount *= 2;
    if(Victim->Def.Class == CureClass)// Check for cure damage
        DmgAmount = -(labs(DmgAmount)/2);
    // If no physical damage is dealt then randomly deal 10-20% of damage from the original amount.
    if(!DmgAmount && PhysicalAttack == TRUE) 
    {  
		Range     = (float)((rand() % 10) + 10) / 100.0f; 
		DmgAmount = (long)((float)Amount * Range);
    }
	Victim->HealthPoints -= DmgAmount;// Subtract damage amount	
	if(DmgAmount >= 0)// Set hurt status and display message
	{
		sprintf(Text, "-%lu HP", DmgAmount); 
		SetMessage(Victim, Text, 500, D3DCOLOR_RGBA(255,64,0,255));  	
		if(DmgAmount)// Only set hurt if any damage (and idle or moving) 
		{   
			if(Victim->Action==CHAR_MOVE || Victim->Action==CHAR_IDLE)     
				SetAction(Victim, CHAR_HURT);  
		}
	} 
	if(DmgAmount < 0)// Display cure amount 
	{
		sprintf(Text, "+%lu HP", -DmgAmount);   
		SetMessage(Victim, Text, 500, D3DCOLOR_RGBA(0,64,255,255));
	}
	return TRUE;
}

BOOL CCharacterController::Death(sCharacter *Attacker,sCharacter *Victim)                                         
{
	char Text[128];
	if(Victim->Type == CHAR_PC)// If a PC or NPC dies, then don't remove from list  
	{
		Victim->Enabled = FALSE;// Mark character as disabled so no updates
		if(Victim->Type == CHAR_PC)// Call outside death function for PC's   
			PCDeath(Victim);
		else	   
			NPCDeath(Victim);		
	} 
	else
	{
	    bKillMonster=true;
		curKillMonster=Victim->ID;   	
		if(Attacker != NULL)// Give attacker the victim's experience(固定) 
		{     
			if(Experience(Attacker, Victim->Def.Experience) == TRUE)  
			{  
				sprintf(Text, "+%lu exp.", Victim->Def.Experience);      
				SetMessage(Attacker, Text, 500);    
			}
		}  	
		if(m_MIL != NULL && Victim->Def.Money)// Drop character's money    
			DropMoney(Victim->XPos, Victim->YPos, Victim->ZPos,Victim->Def.Money);                    	
		if((rand() % 100) < Victim->Def.DropChance)// Randomly drop an item (as specified in definition) 
			DropItem(Victim->XPos, Victim->YPos, Victim->ZPos,Victim->Def.DropItem,1);                      	
		m_Meshes[Victim->Def.MeshNum].Count--;// Decrease mesh count and release if no more
		if(!m_Meshes[Victim->Def.MeshNum].Count)
		{   
			m_Meshes[Victim->Def.MeshNum].Mesh.Free();    
			m_Meshes[Victim->Def.MeshNum].Animation.Free();   
		}
		// Remove dead character from list,two pointers should be modified and two deleted 	 
		if(Victim->Prev != NULL)   
			Victim->Prev->Next = Victim->Next;
		else
			m_CharacterParent = Victim->Next;
		if(Victim->Next != NULL)  
			Victim->Next->Prev = Victim->Prev;
		if(Victim->Prev == NULL && Victim->Next == NULL)
			m_CharacterParent = NULL;
		Victim->Prev = Victim->Next = NULL;
		delete Victim;
	} 
	return TRUE;
}

BOOL CCharacterController::Spell(sCharacter *Caster,sSpellTracker *SpellTracker,sSpell *Spells)                                                                            
{//自动找一个最近的目标施展Spell或施展范围Spell
	float XDiff, YDiff, ZDiff, Dist, YDist, XZDist;
    float MinX, MaxX, MinY, MaxY, MinZ, MaxZ;
    float SpellRadius, XZRadius, YRadius;
    sSpell *SpellPtr;
    sCharacter *CharPtr, *ClosestChar;
    float Closest;
    BOOL Allow;
    if(Caster == NULL || SpellTracker == NULL || Spells == NULL)
        return TRUE;
    SpellPtr = &Spells[SpellTracker->SpellNum];
    if(Caster->Ailments & AILMENT_SILENCED)// Can't cast if silenced
    {
        SetMessage(Caster, "Silenced!", 500);
        return FALSE;
    }  
    if(Caster->Definition!=CHAR_PC)//每个NPC/Monster也应对应一个CharSkill,此处简化问题,仅PC有
    {
		if(Caster->ManaPoints>=SpellPtr->Cost)
			Caster->ManaPoints -= SpellPtr->Cost;
		if(Caster->ManaPoints < 0)
			Caster->ManaPoints = 0;
	}
	else
	{  
		long value=(long)((float)SpellPtr->Cost*((float)(g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Level-1)*0.1+1.0f));
	    if(Caster->ManaPoints>=value)
	    {
		    Caster->ManaPoints-=value;
	    }
	    else
	    {
		    Caster->ManaPoints=0;
	    }
  
	}
	SpellRadius = SpellPtr->Range * SpellPtr->Range;// Get radius of spell
	if(SpellPtr->Target == TARGET_SELF)// Handle self-targeting spells instantly
	{
		SpellEffect(Caster, Caster, SpellPtr); 
		return TRUE;
	}
	// Reset closest character pointer
	ClosestChar = NULL;
	Closest = 0.0f;
	if((CharPtr = m_CharacterParent) == NULL)
		return FALSE;
	while(CharPtr != NULL)// Scan through all characters and look for hits 
	{
		// Only bother with characters of allowed types as well as not targeting self.    
		// Also, allow a RAISE_DEAD PC spell to affect any character.
		Allow = FALSE;   
		if(CharPtr!=Caster && SpellTracker->Type==CharPtr->Type)    
			Allow = TRUE;  
		if(CharPtr->Type==CHAR_PC && SpellPtr->Effect==RAISE_DEAD)    
			Allow = TRUE;  	
		if(Allow == TRUE)// Find target(s) if allowed 
		{  
			// Get distance from target to character   
			XDiff = (float)fabs(CharPtr->XPos - SpellTracker->TargetX);    
			YDiff = (float)fabs(CharPtr->YPos - SpellTracker->TargetY);    
			ZDiff = (float)fabs(CharPtr->ZPos - SpellTracker->TargetZ);  
			// Get X/Z and Y distances  
			XZDist = (XDiff * XDiff + ZDiff * ZDiff) - SpellRadius;    
			YDist  = (YDiff * YDiff) - SpellRadius;  
			// Get target X/Z and Y radius   
			CharPtr->Object.GetBounds(&MinX,&MinY,&MinZ,&MaxX,&MaxY,&MaxZ,NULL);                                
			XZRadius = max(MaxX-MinX, MaxZ-MinZ) * 0.5f;   
			YRadius = (MaxY-MinY) * 0.5f;
			
			if(XZDist-(XZRadius * XZRadius)<=0.0f&&YDist-(YRadius*YRadius)<=0.0f)// Check if character in range           	 
			{         				     
				if(SpellPtr->Target == TARGET_SINGLE)// Determine what to do if in range  		
				{     
					// Record closest character in range         
					Dist = XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff;        
					if(ClosestChar == NULL) 		 
					{         
						ClosestChar = CharPtr;          
						Closest = Dist;         
					} 		 
					else 	 
					{        
						if(Dist < Closest) 			
						{            
							ClosestChar = CharPtr;           
							Closest = Dist;            
						}         
					}      
				}	
				else 		
				{        				         
					SpellEffect(Caster, CharPtr, SpellPtr);//攻击范围内的所有角色       
				} 
			}
		} 		
		CharPtr = CharPtr->Next;// Go to next character
	}   
	if(SpellPtr->Target==TARGET_SINGLE && ClosestChar!=NULL)// Process spell on closest character if needed   
		SpellEffect(Caster, ClosestChar, SpellPtr);
	return TRUE;
}

BOOL CCharacterController::SpellEffect(sCharacter *Caster,sCharacter *Target,sSpell *Spell)                                                                                     
{//技能效果 Caster施展者 Target目标 Spell指向技能结构体的指针
 
	BOOL CanHit;
    long Chance;
    char Text[64];
    if(Target == NULL || Spell == NULL)
        return FALSE;
    if(Caster != NULL) //计算命中几率 
    { 
		if(Caster == Target)//针对自身的技能不会Miss  
			Chance = 1000;
		else//施展技能的威力取决于使用者的内力和被攻者的气御,内力深厚者即使一招长拳也可致命,善御气者,纵然千斤之力,亦能化之于无形    
			Chance=Spell->Chance-(long)((float)GetAgility(Target)/100.0f+1.0f)*60;                       
	} 
	else
	{ 
		Chance = Spell->Chance;
	}                          
	if(Chance != 1000 && (rand() % 1000) >= Chance)  
	{
		SetMessage(Target, "Failed", 500);  
		return FALSE; 
	} 
	CanHit = TRUE;// Flag character to allow effect
	if(Target->Action==CHAR_HURT || Target->Action==CHAR_DIE)
		CanHit = FALSE;
	if((Target->Attacker = Caster) != NULL)// Store attacker and victim
   		Caster->Victim = Target;  
	switch(Spell->Effect)// Process spell effect
	{  
	case ALTER_HEALTH://改变HP,Spell的伤害基值乘以施展者的(内力/100+1),限攻击性的    
		if(CanHit == FALSE)   
			break;       
		if(Spell->Value[0] < 0.0f)	  
		{       
			float Mental=(float)GetMental(Caster)/100.0f+1;		
			if(Caster->Definition!=CHAR_PC)     
				Damage(Target,FALSE,-(long)(Spell->Value[0]*Mental),Spell->DmgClass, Spell->CureClass);  	
			else	
			{			
				long value;			
				value=(long)((float)Spell->Value[0]*(1.0f+0.2*(float)(g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Level-1)));			
				Damage(Target,FALSE,-(long)(value*Mental),Spell->DmgClass,Spell->CureClass);	
			}  
		}  
		if(Spell->Value[0] > 0.0f)   
		{	
			long value;	
			if(Caster->Definition!=CHAR_PC)	
			{	
				value=(long)Spell->Value[0];      
				Target->HealthPoints +=value;		
			}		
			else	
			{						
				value=(long)((float)Spell->Value[0]*(1.0f+0.2*(float)(g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Level-1)));		
				Target->HealthPoints+=value;	
			}     
			if(Target->HealthPoints>GetHP(Target))        
				Target->HealthPoints=GetHP(Target);    
			sprintf(Text, "+%lu HP", value);     
			SetMessage(Target,Text,500,D3DCOLOR_RGBA(0,64,255,255)); 
		}  
		if(Caster->Definition==CHAR_PC)  		 
			g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Exp+=1;	  		
		break;  
	case ALTER_MANA:// Alter mana    
		if(CanHit == FALSE)     
			break;    
		Target->ManaPoints += (long)Spell->Value[0];    
		if(Target->ManaPoints < 0)       
			Target->ManaPoints = 0;   
		if(Target->ManaPoints>GetMP(Target))       
			Target->ManaPoints=GetMP(Target);   
		if(Spell->Value[0] < 0.0f) 
		{      
			sprintf(Text, "%ld MP", (long)Spell->Value[0]);       
			SetMessage(Target,Text,500,D3DCOLOR_RGBA(0,128,64,255));  
		}     
		if(Spell->Value[0] > 0.0f)	 
		{      
			sprintf(Text, "+%lu MP", (long)Spell->Value[0]);      
			SetMessage(Target,Text,500,D3DCOLOR_RGBA(0,255,0,255));    
		}	  
		if(Caster->Definition==CHAR_PC)	  
		{		 
			g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Exp+=1;	 
		}   
		break;  
	case CURE_AILMENT:// Clear ailment flag      
		if(CanHit == FALSE)      
			break;    
		Target->Ailments |= ~(long)Spell->Value[0];     
		SetMessage(Target, "Cure", 500);  
		if(Caster->Definition==CHAR_PC)
		{		 
			g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Exp+=1;	 
		}    
		break; 
	case CAUSE_AILMENT:// Set ailment flag     
		if(CanHit == FALSE)      
			break;   
		Target->Ailments |= (long)Spell->Value[0];    
		SetMessage(Target, "Ailment", 500);  
		if(Caster->Definition==CHAR_PC) 
		{		 
			g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Exp+=1;	 
		}    
		break;
    case RAISE_DEAD://复活     
		if(Target->Action == CHAR_DIE) 	  
		{      
			Target->HealthPoints = 1;      
			Target->ManaPoints   = 0;      
			Target->Action       = CHAR_IDLE;      
			Target->Locked       = FALSE;      
			Target->ActionTimer  = 0;   
			Target->Ailments     = 0;       
			Target->Enabled      = TRUE;     
		}   
		if(Caster->Definition==CHAR_PC)	 
		{		 
			g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Exp+=1;	  
		}     
		break;
    case INSTANT_KILL://瞬杀    
		if(CanHit == FALSE)      
			break;    
		SetAction(Target, CHAR_DIE);  
		if(Caster->Definition==CHAR_PC)	 
		{		  
			g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Exp+=1;	 
		}   
		break;
    case DISPEL_MAGIC://驱散一切负面状态    
		if(CanHit == FALSE)      
			break;     
		Target->Ailments = 0;  
		if(Caster->Definition==CHAR_PC)	 
		{		  
			g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Exp+=1;	 
		}   
		break;
    case TELEPORT://传送    
		if(CanHit == FALSE)    
			break;   
		if(Target->Type == CHAR_PC)       
			PCTeleport(Caster, Spell);    
		else 	 
		{      
			Target->XPos = Spell->Value[0];       
			Target->YPos = Spell->Value[1];       
			Target->ZPos = Spell->Value[2];    
		} 
		if(Caster->Definition==CHAR_PC)	  
		{		 
			g_PCChar->CharSkill->GetSkill(curUseSpellIDInCharSkill)->Exp+=1;	 
		}     
		break;
    }//End switch
	return TRUE;
}

BOOL CCharacterController::Equip(sCharacter *Character,long ItemNum,long Type,BOOL Equip)                                                                            
{//角色Character装备/卸下Type类型的MIL中编号为ItemNum的道具  
	char Path[MAX_PATH];  
	if(m_MIL == NULL || Character == NULL)  
		return FALSE; 	
	if(!CheckUsageBit(m_MIL[ItemNum].Usage,Character->Def.Class))// Make sure restritions allow equiping of item   
		return FALSE;	
	switch(Type)// Remove current item first and equip new one
	{ 
	case WEAPON:   
		Character->Def.Weapon = -1;   
		Character->WeaponObject.Free();  
		Character->WeaponMesh.Free();   
		if(Equip==TRUE && m_MIL[ItemNum].Category == WEAPON) 
		{      
			Character->Def.Weapon = ItemNum;     		    
			if(m_MIL[ItemNum].MeshFilename[0])// Setup new weapon mesh and object		
			{        
				// Build the mesh path        
				sprintf(Path, "%s%s", m_MeshPath,  m_MIL[Character->Def.Weapon].MeshFilename);                        
				// Load the new mesh         
				Character->WeaponMesh.Load(m_Graphics, Path,   m_TexturePath);                                                  
				// Create the weapon object        
				Character->WeaponObject.Create(m_Graphics,  &Character->WeaponMesh);                                                 
				// Orient and attach the weapon         
				Character->WeaponObject.Move(0.0f, 0.0f, 0.0f);        
				Character->WeaponObject.Rotate(0.0f, 0.0f, 0.0f);        
				Character->WeaponObject.AttachToObject(&Character->Object, "WeaponHand");                                         
			}     
		}     
		break;
    case ARMOR:     
		Character->Def.Armor = -1;      
		if(Equip==TRUE && m_MIL[ItemNum].Category == ARMOR)       
			Character->Def.Armor = ItemNum;  
		break;   
    case HELMET://头戴     
		Character->Def.Helmet = -1;     
		if(Equip==TRUE && m_MIL[ItemNum].Category == HELMET)       
			Character->Def.Helmet = ItemNum;	 
		break;	
	case BOOTS://脚踏		
		Character->Def.Boots=-1;
		if(Equip==TRUE&&m_MIL[ItemNum].Category==BOOTS)
			Character->Def.Boots=ItemNum;
		break;
    case ADORN:     
		Character->Def.Adorn = -1;     
		if(Equip==TRUE && m_MIL[ItemNum].Category == ADORN)      
			Character->Def.Adorn = ItemNum;     
		break;
    default: 	
		return FALSE;
	} 
	return TRUE;
}

BOOL CCharacterController::Item(sCharacter *Owner,sCharacter *Target,long ItemNum,sCharItem *CharItem)                                                                                                                                      
{ 
	sItem *Item; 
	if(Owner == NULL || Target == NULL || m_MIL == NULL) 
		return FALSE;
	if(!CheckUsageBit(m_MIL[ItemNum].Usage, Target->Def.Class))// Make sure restritions allow equiping of item 
		return FALSE;
	Item = (sItem*)&m_MIL[ItemNum]; 
	switch(Item->Category)  
	{   
	case FOOD:
    case HEAL:     
		Owner->HealthPoints += Item->ModifyHP;	 
		Owner->ManaPoints+=Item->ModifyMP;  
		break;
	}
	if(CheckItemFlag(Item->Flags,USEONCE) && CharItem != NULL)
	{
		CharItem->Quantity--;  
		if(CharItem->Quantity <= 0 && Owner->CharICS != NULL)    
			Owner->CharICS->Remove(CharItem);
	}
	return TRUE;
}

BOOL CCharacterController::Drop(sCharacter *Character,sCharItem *Item, long Quantity)               
{//丢弃Quantity个道具 
	if(Item == NULL || m_MIL == NULL || Character == NULL) 
		return FALSE;
	if(!CheckItemFlag(m_MIL[Item->ItemNum].Flags,CANDROP))  
		return FALSE;
	Item->Quantity -= Quantity;  
	if(Item->Quantity <= 0 && Character->CharICS != NULL)  
		Character->CharICS->Remove(Item);
	return TRUE;
}

BOOL CCharacterController::PCDeath(sCharacter *Character)
{
	GameIsOver=true;
    return TRUE;	
}
