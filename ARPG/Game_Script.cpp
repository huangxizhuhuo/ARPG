/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#include "Global.h"

CGameScript::CGameScript()
{ 
    m_App   = NULL;// Clear script data pointers
    m_Route = NULL;  
    for(long i=0;i<256;i++)// Clear flags and variables
    {
        m_Flags[i] = FALSE;
        m_Vars[i] = 0;
    }
}

CGameScript::~CGameScript()
{ 
	delete [] m_Route;  // Only need to free the internal route
}

BOOL CGameScript::SetData(CApp *App)
{ 	
    if((m_App = App) == NULL)// Remember the application class pointer
        return FALSE;
	m_Talk.Create(&m_App->m_Graphics, &m_App->m_Font,&m_App->m_TalkPic,&m_App->m_Mouse);
    m_BlackScreen.Create(&m_App->m_Graphics,&m_App->m_Font,&m_App->m_Black,&m_App->m_Mouse);
    return TRUE; 
}

BOOL CGameScript::Reset()
{ 
	for(long i=0;i<256;i++)// Clear flags and variables 
	{  
		m_Flags[i] = FALSE;  
		m_Vars[i] = 0;
	}  
	return TRUE;
}

BOOL CGameScript::Save(char *Filename)
{
	FILE *fp;
    if((fp=fopen(Filename, "wb"))==NULL)
        return FALSE;
    fwrite(&m_Flags, 1, sizeof(m_Flags), fp);
    fwrite(&m_Vars, 1, sizeof(m_Vars), fp);
    fclose(fp);
    return TRUE;
}

BOOL CGameScript::Load(char *Filename)
{
    FILE *fp;
    if((fp=fopen(Filename, "rb"))==NULL)
        return FALSE;
    fread(&m_Flags, 1, sizeof(m_Flags), fp);
    fread(&m_Vars, 1, sizeof(m_Vars), fp);
    fclose(fp);
    return TRUE;
}

BOOL CGameScript::Release()
{
    delete [] m_Route;
    m_Route          = NULL;
    m_NumRoutePoints = 0;
    return TRUE;
}

sScript *CGameScript::Process(sScript *Script)
{
	switch(Script->Type)
	{ 
	case  0: return Script_End(Script);
    case  1: return Script_Else(Script);
    case  2: return Script_EndIf(Script);
    case  3: return Script_IfFlagThen(Script);
    case  4: return Script_IfVarThen(Script);
    case  5: return Script_SetFlag(Script);
    case  6: return Script_SetVar(Script);
    case  7: return Script_Label(Script);
    case  8: return Script_Goto(Script);
    case  9: return Script_Message(Script);
    case 10: return Script_Add(Script);
    case 11: return Script_Remove(Script);
    case 12: return Script_Move(Script);
    case 13: return Script_Direction(Script);
    case 14: return Script_Type(Script);
    case 15: return Script_AI(Script);
    case 16: return Script_Target(Script);
    case 17: return Script_NoTarget(Script);
    case 18: return Script_Bounds(Script);
    case 19: return Script_Distance(Script);
    case 20: return Script_Script(Script);
    case 21: return Script_CharMessage(Script);
    case 22: return Script_Enable(Script);
    case 23: return Script_CreateRoute(Script);
    case 24: return Script_AddPoint(Script);
    case 25: return Script_AssignRoute(Script);
    case 26: return Script_AlterHPMP(Script);
    case 27: return Script_Ailment(Script);
    case 28: return Script_AlterSpell(Script);
    case 29: return Script_Teleport(Script);
    case 30: return Script_ShortMessage(Script);
    case 31: return Script_Action(Script);
    case 32: return Script_IfExpLevel(Script);
    case 33: return Script_Barter(Script);
    case 34: return Script_IfItem(Script);
    case 35: return Script_AddItem(Script);
    case 36: return Script_RemoveItem(Script);
    case 37: return Script_AddBarrier(Script);
    case 38: return Script_EnableBarrier(Script);
    case 39: return Script_RemoveBarrier(Script);
    case 40: return Script_AddTrigger(Script);
    case 41: return Script_EnableTrigger(Script);
    case 42: return Script_RemoveTrigger(Script);
    case 43: return Script_Sound(Script);
    case 44: return Script_Music(Script);
    case 45: return Script_StopMusic(Script);
    case 46: return Script_WinGame(Script);
    case 47: return Script_CommentOrSeparator(Script);
    case 48: return Script_CommentOrSeparator(Script);
    case 49: return Script_Wait(Script);
    case 50: return Script_IfRandThen(Script);
    case 51: return Script_Render(Script);
	case 52: return Script_Motion(Script);
	case 53: return Script_SetFaceToFace(Script);
	case 54: return Script_LetNPCAttack(Script);
	case 55: return Script_IfKillThen(Script);
	case 56: return Script_IfAllWasKilled(Script);
	case 57: return Script_MoveCamera(Script);
	case 58: return Script_MoveCameraToChar(Script);
	case 59: return Script_SetPCControlCamera(Script);
	case 60: return Script_BlackScreenMessage(Script);
	case 61: return Script_PlayMP3(Script);
	case 62: return Script_StopMP3(Script);
	case 63: return Script_PlayAVI(Script);
	}
	return NULL; // Error executing
}

sScript *CGameScript::Script_End(sScript *ScriptPtr)
{
    return NULL;  // Force end of processing
}

sScript *CGameScript::Script_Else(sScript *ScriptPtr)
{
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_EndIf(sScript *ScriptPtr)
{
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_IfFlagThen(sScript *ScriptPtr)
{
    BOOL Skipping;
    // See if a flag matches second entry
    if(m_Flags[ScriptPtr->Entries[0].lValue] == ScriptPtr->Entries[1].bValue)                         
        Skipping = FALSE;
    else 
        Skipping = TRUE;  
    return Script_IfThen(ScriptPtr->Next, Skipping);// Let if/then processor handle the rest
}

sScript *CGameScript::Script_IfVarThen(sScript *ScriptPtr)
{
	BOOL Skipping;
	// See if var matches second entry
	if(m_Vars[ScriptPtr->Entries[0].lValue] == ScriptPtr->Entries[1].lValue)                  	 
		Skipping = FALSE;
	else     
		Skipping = TRUE;
	return Script_IfThen(ScriptPtr->Next, Skipping);// Let if/then processor handle the rest
}

sScript *CGameScript::Script_SetFlag(sScript *ScriptPtr)
{
    // Set boolean value
    m_Flags[ScriptPtr->Entries[0].lValue] = ScriptPtr->Entries[1].bValue;                             
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_SetVar(sScript *ScriptPtr)
{
    // Set long value
    m_Vars[ScriptPtr->Entries[0].lValue] = ScriptPtr->Entries[1].lValue;                             
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_Label(sScript *ScriptPtr)//脚本标签?用于GOTO跳转
{
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_Goto(sScript *ScriptPtr)
{
    sScript *Script;
    // Search entire script for label
    Script = GetParentScript();
    while(Script != NULL) 
    {       
        if(Script->Entries[0].lValue==ScriptPtr->Entries[0].lValue)// Return next entry if a match     
			return Script->Next; 	
		Script = Script->Next;// Go to next entry
	}
	return NULL;  // End of script, return completion
}

sScript *CGameScript::Script_Message(sScript *ScriptPtr)//显示一条信息,采用m_Window,按下鼠标左键或空格键Break
{
	/*m_Talk.SetText((char*)ScriptPtr->Entries[0].strText.c_str());// Create the text   
	// Lock the keyboard and mouse
	m_App->m_Keyboard.SetLock(KEY_SPACE, TRUE);
	m_App->m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
	m_App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
	m_App->m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);	
	while(1)// Render the scene while waiting for keypress/buttonpress 
	{
		// Break when space pressed  
		m_App->m_Keyboard.Acquire(TRUE); 
		m_App->m_Keyboard.Read();
		if(m_App->m_Keyboard.GetKeyState(KEY_SPACE) == TRUE)	
		{
			if(m_Talk.bPrintAll)    
				break;		
			else	
				m_Talk.PrintSpeed=100;
		}
		// Break when left mouse button pressed 
		m_App->m_Mouse.Acquire(TRUE);
		m_App->m_Mouse.Read();  
		if(m_App->m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE)
		{
			if(m_Talk.bPrintAll)      
				break;	
			else
				m_Talk.PrintSpeed=100;
		}
		// Render the scene w/window
		m_App->m_Graphics.ClearZBuffer();
		m_App->GetCharController()->Update(33);
		m_App->GetSpellController()->Update(33);   
		if(m_App->m_Graphics.BeginScene() == TRUE) 
		{
			m_App->RenderFrame(33);//渲染场景、角色、法术	  
			m_Talk.Render();
			m_App->m_Graphics.EndScene();
		}
		m_App->m_Graphics.Display();
	}
	// Relock keyboard and mouse
	m_App->m_Keyboard.SetLock(KEY_SPACE, TRUE);
	m_App->m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
	m_App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
	m_App->m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);   
	return ScriptPtr->Next;*/
	return NULL;
}

sScript *CGameScript::Script_CharMessage(sScript *ScriptPtr)
{
	sCharacter *CharPtr;
	D3DXMATRIX matWorld, matView, matProj;
	D3DXVECTOR3 vecTarget;
	D3DVIEWPORT9 vpScreen;
	float MaxY;
	// Get the transformation matrices
	D3DXMatrixIdentity(&matWorld);
	m_App->m_Graphics.GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView);
	m_App->m_Graphics.GetDeviceCOM()->GetTransform( D3DTS_PROJECTION, &matProj);
	m_App->m_Graphics.GetDeviceCOM()->GetViewport(&vpScreen);// Get the viewport
	// Get the character's height
	CharPtr=m_App->m_CharController.GetCharacter( ScriptPtr->Entries[1].lValue);
	CharPtr->Object.GetBounds(NULL,NULL,NULL,NULL,&MaxY,NULL,NULL);
	// Project the 3-D coordinates in 2-D coordinates
	D3DXVec3Project(&vecTarget,&D3DXVECTOR3(CharPtr->XPos,CharPtr->YPos+MaxY,CharPtr->ZPos),
	&vpScreen, &matProj, &matView, &matWorld);
	// Create the text
	m_Talk.SetText((char*)ScriptPtr->Entries[0].strText.c_str(), D3DCOLOR_RGBA(255,255,0,255));
	// Lock the keyboard and mouse
	m_App->m_Keyboard.SetLock(KEY_SPACE, TRUE);
	m_App->m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
	m_App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
	m_App->m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);
	// Render the scene while waiting for keypress/buttonpress
	char Text[100];
	sprintf(Text,"..\\Image\\face\\%ld.png",ScriptPtr->Entries[3].lValue);
	m_Head.Load(&m_App->m_Graphics,Text);//D3DCOLOR_RGBA(255,255,255,0));//////////////////);
	while(true)
	{
		static DWORD UpdateTimer = timeGetTime();
		// Limit all frame updates to 30 fps
		if (timeGetTime() < UpdateTimer + 33)
			continue;
		UpdateTimer = timeGetTime();

		CanGetInput=false;
		// Break when space pressed
		m_App->m_Keyboard.Acquire(TRUE);
		m_App->m_Keyboard.Read();
		if(m_App->m_Keyboard.GetKeyState(KEY_SPACE) == TRUE)
		{
			if(m_Talk.bPrintAll)
				break;
			else
				m_Talk.PrintSpeed=100;
		}
		// Break when left mouse button pressed
		m_App->m_Mouse.Acquire(TRUE);
		m_App->m_Mouse.Read();
		if(m_App->m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE)
		{
			if(m_Talk.bPrintAll)
				break;
			else
				m_Talk.PrintSpeed=100;
		}
		// Render the scene w/window
		m_App->m_Graphics.Clear();
		m_App->GetCharController()->Update(33);
		m_App->GetSpellController()->Update(33);
		if(m_App->m_Graphics.BeginScene() == TRUE)
		{
			m_App->RenderFrame(33);
			if(ScriptPtr->Entries[2].bValue==TRUE)//头像放在左边
			{
				m_Talk.Render(0,-1,0);
				m_App->m_Graphics.BeginSprite();
				m_Head.Blit(0,460);
				m_App->m_Graphics.EndSprite();
				sCharacter * CharCur=m_App->GetCharController()->GetCharacter(ScriptPtr->Entries[1].lValue);
				char Text[30];
				sprintf(Text,"%s：",CharCur->Def.Name);
				m_App->m_Font.Print(Text,120,512);
			}
			else
			{
				m_Talk.Render(0,-1,1);
				m_App->m_Graphics.BeginSprite();
				m_Head.Blit(650,460);
				m_App->m_Graphics.EndSprite();
				sCharacter * CharCur=m_App->GetCharController()->GetCharacter(ScriptPtr->Entries[1].lValue);
				char Text[30];
				sprintf(Text,"%s：",CharCur->Def.Name);
				m_App->m_Font.Print(Text,60,512);
			}
			m_App->m_Graphics.EndScene();
		}	
		m_App->m_Graphics.Display();
	}
	CanGetInput=true;
	m_Head.Free();
	// Relock keyboard and mouse
	m_App->m_Keyboard.SetLock(KEY_SPACE, TRUE);
	m_App->m_Keyboard.SetKeyState(KEY_SPACE, FALSE);
	m_App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
	m_App->m_Mouse.SetKeyState(MOUSE_LBUTTON, FALSE);
	return ScriptPtr->Next;

	// Create the text
	//m_App->m_Talk.bTalking = true;
	//m_App->m_Talk.SetText((char*)ScriptPtr->Entries[0].strText.c_str(), D3DCOLOR_RGBA(255, 255, 0, 255));
	//while (true)
	//{
	//	if (!m_App->m_Talk.bTalking)
	//		break;
	//}
	//return ScriptPtr->Next; 
}

sScript *CGameScript::Script_Add(sScript *ScriptPtr)//添加一个角色
{
	m_App->m_CharController.Add(ScriptPtr->Entries[0].lValue, ScriptPtr->Entries[1].lValue, 	 
		ScriptPtr->Entries[2].Selection,CHAR_STAND,ScriptPtr->Entries[3].fValue,	 
		ScriptPtr->Entries[4].fValue,ScriptPtr->Entries[5].fValue,ScriptPtr->Entries[6].fValue);                                                                                                                                                                                                                                                                                                 	
	m_App->m_CharController.Update(0);// Force an update of all character w/0 elapsed time  
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_Remove(sScript *ScriptPtr)//移除一个角色
{
    m_App->m_CharController.Remove(ScriptPtr->Entries[0].lValue);  
    m_App->m_CharController.Update(0);// Force an update of all character w/0 elapsed time
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_Move(sScript *ScriptPtr)//瞬移一个角色
{
    m_App->m_CharController.Move(ScriptPtr->Entries[0].lValue, ScriptPtr->Entries[1].fValue,
		ScriptPtr->Entries[2].fValue,ScriptPtr->Entries[3].fValue);                                                                                    
    m_App->m_CharController.Update(0);// Force an update of all character w/0 elapsed time
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_Direction(sScript *ScriptPtr)
{
	sCharacter *CharPtr; 
	if((CharPtr=m_App->m_CharController.GetCharacter(ScriptPtr->Entries[0].lValue))!=NULL)                              
		CharPtr->Direction = ScriptPtr->Entries[1].fValue;
	m_App->m_CharController.Update(0);// Force an update of all character w/0 elapsed time
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_Type(sScript *ScriptPtr)//改变角色类型、如把PC改为一个NPC使其受计算机控制
{  
	m_App->m_CharController.SetType(ScriptPtr->Entries[0].lValue,ScriptPtr->Entries[1].Selection);                                                                                 
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_AI(sScript *ScriptPtr)
{
	m_App->m_CharController.SetAI(ScriptPtr->Entries[0].lValue, ScriptPtr->Entries[1].Selection);                                                                                  
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_Target(sScript *ScriptPtr)
{ 
	m_App->m_CharController.SetTargetCharacter( ScriptPtr->Entries[0].lValue,ScriptPtr->Entries[1].lValue);                                                                      
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_NoTarget(sScript *ScriptPtr)
{
	m_App->m_CharController.SetTargetCharacter(ScriptPtr->Entries[0].lValue, -1);                                     
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_Bounds(sScript *ScriptPtr)
{
	m_App->m_CharController.SetBounds(ScriptPtr->Entries[0].lValue,ScriptPtr->Entries[1].fValue,	 
		ScriptPtr->Entries[2].fValue,ScriptPtr->Entries[3].fValue,ScriptPtr->Entries[4].fValue,	
		ScriptPtr->Entries[5].fValue,ScriptPtr->Entries[6].fValue);                                                                                                                                                                                                                                       
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_Distance(sScript *ScriptPtr)
{
    m_App->m_CharController.SetDistance(ScriptPtr->Entries[0].lValue,ScriptPtr->Entries[1].fValue);                                                                             
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_Script(sScript *ScriptPtr)//2007 12 9
{
	m_App->m_CharController.SetScript(ScriptPtr->Entries[0].lValue,(char*)ScriptPtr->Entries[1].strText.c_str());                                                                              
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_Enable(sScript *ScriptPtr)
{
    m_App->m_CharController.SetEnable(ScriptPtr->Entries[1].lValue,ScriptPtr->Entries[0].bValue);                                                                               
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_CreateRoute(sScript *ScriptPtr)
{
	delete [] m_Route;// Release old route
	m_Route = NULL;
    m_NumRoutePoints = 0;
    m_NumRoutePoints = ScriptPtr->Entries[0].lValue;
    m_Route = new sRoutePoint[m_NumRoutePoints]();
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_AddPoint(sScript *ScriptPtr)
{
    m_Route[ScriptPtr->Entries[0].lValue].XPos =ScriptPtr->Entries[1].fValue;                                               
    m_Route[ScriptPtr->Entries[0].lValue].YPos =ScriptPtr->Entries[2].fValue;                                               
    m_Route[ScriptPtr->Entries[0].lValue].ZPos =ScriptPtr->Entries[3].fValue;                                               
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_AssignRoute(sScript *ScriptPtr)
{
    m_App->m_CharController.SetRoute(ScriptPtr->Entries[0].lValue,m_NumRoutePoints, (sRoutePoint*)m_Route);                                                                               
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_AlterHPMP(sScript *ScriptPtr)
{
    sCharacter *CharPtr;
    if((CharPtr=m_App->m_CharController.GetCharacter(ScriptPtr->Entries[3].lValue)) != NULL)                           
    {  
		if(!ScriptPtr->Entries[0].Selection)	
		{   
			if(!ScriptPtr->Entries[2].Selection)      
				CharPtr->HealthPoints += ScriptPtr->Entries[1].lValue;    
			else      
				CharPtr->ManaPoints += ScriptPtr->Entries[1].lValue;   
		} 	
		else 	
		{    
			if(!ScriptPtr->Entries[2].Selection)     
				CharPtr->HealthPoints -= ScriptPtr->Entries[1].lValue;  
			else       
				CharPtr->ManaPoints -= ScriptPtr->Entries[1].lValue; 
		}
		// Bounds check values   
		if(CharPtr->HealthPoints > CharPtr->Def.HealthPoints)    
			CharPtr->HealthPoints = CharPtr->Def.HealthPoints;  
		if(CharPtr->ManaPoints > CharPtr->Def.ManaPoints) 
			CharPtr->ManaPoints = CharPtr->Def.ManaPoints;
	}
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_Ailment(sScript *ScriptPtr)
{ 
	sCharacter *CharPtr;
	if((CharPtr = m_App->m_CharController.GetCharacter(ScriptPtr->Entries[2].lValue)) != NULL)                            
	{	
		if(!ScriptPtr->Entries[0].Selection) 	
		{   		  
			CharPtr->Ailments &= ~ScriptPtr->Entries[1].lValue;// Cure ailments  
		} 
		else 
		{  		  
			CharPtr->Ailments |= ScriptPtr->Entries[1].lValue;// Cause ailments  
		}
	}
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_AlterSpell(sScript *ScriptPtr)
{
	sCharacter *CharPtr;
	long SpellNum, BitFlag;
	if((CharPtr = m_App->m_CharController.GetCharacter(ScriptPtr->Entries[0].lValue)) != NULL)                            
	{  
		SpellNum = ScriptPtr->Entries[2].lValue;  
		BitFlag = (1 << (SpellNum & 31));
		if(!ScriptPtr->Entries[1].Selection)
		{			    
			CharPtr->Def.MagiCSpells[SpellNum/32] |= BitFlag;// Learn a spell  
		} 
		else
		{   		 
			CharPtr->Def.MagiCSpells[SpellNum/32] &= ~BitFlag;// Forget a spell
		}
	}
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_Teleport(sScript *ScriptPtr)
{
	sCharacter *CharPtr;
	if((CharPtr = m_App->m_CharController.GetCharacter(ScriptPtr->Entries[0].lValue)) != NULL)                           
	{   
		m_App->SetupTeleport(ScriptPtr->Entries[1].lValue,ScriptPtr->Entries[2].fValue,
			ScriptPtr->Entries[3].fValue,ScriptPtr->Entries[4].fValue);                                                               
		return NULL;  // Stop processing after teleport 
	}
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_ShortMessage(sScript *ScriptPtr)//2007 12 9
{
    sCharacter *CharPtr;
    if((CharPtr = m_App->m_CharController.GetCharacter(ScriptPtr->Entries[0].lValue)) != NULL)                           
    {
	    m_App->m_CharController.SetMessage(CharPtr,(char*)ScriptPtr->Entries[1].strText.c_str(),ScriptPtr->Entries[2].lValue);                                                                    
    }
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_Motion(sScript *ScriptPtr)
{
	sCharacter *CharPtr;
	if((CharPtr=m_App->m_CharController.GetCharacter(ScriptPtr->Entries[0].lValue))!=NULL)
	{
		m_App->m_CharController.SetEmotion(CharPtr,ScriptPtr->Entries[1].lValue,
			ScriptPtr->Entries[2].lValue,ScriptPtr->Entries[3].lValue,ScriptPtr->Entries[4].lValue);
	}
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_Action(sScript *ScriptPtr)
{
    sCharacter *CharPtr;
    if((CharPtr=m_App->m_CharController.GetCharacter(ScriptPtr->Entries[0].lValue)) != NULL)                            
    {
        // Set the character's action
        m_App->m_CharController.SetAction(CharPtr,ScriptPtr->Entries[1].lValue,ScriptPtr->Entries[2].lValue);                                                                  
    }
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_IfExpLevel(sScript *ScriptPtr)
{
    BOOL Skipping;
    sCharacter *CharPtr;
    // Get character pointer
    CharPtr = m_App->m_CharController.GetCharacter(ScriptPtr->Entries[0].lValue);                                           
    // See if level matches values
    if(CharPtr != NULL && CharPtr->Def.Level >= ScriptPtr->Entries[1].lValue)                                    
        Skipping = FALSE;
    else 
        Skipping = TRUE; 
    return Script_IfThen(ScriptPtr->Next, Skipping);// Let if/then processor handle the rest
}

sScript *CGameScript::Script_Barter(sScript *ScriptPtr)//2007 12 9
{
	m_App->SetupBarter(m_App->m_CharController.GetCharacter(ScriptPtr->Entries[1].lValue),
		(char*)ScriptPtr->Entries[0].strText.c_str());                                                                          
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_IfItem(sScript *ScriptPtr)//如果一个或多个道具存在角色的背包?
{
	BOOL Skipping; 
	sCharacter *CharPtr; 
	// Get character pointer
	CharPtr = m_App->m_CharController.GetCharacter(ScriptPtr->Entries[2].lValue);                                           
	// See if item in inventory and check count
	if(CharPtr != NULL && CharPtr->CharICS->GetItem(ScriptPtr->Entries[1].lValue)->Quantity 
		>= ScriptPtr->Entries[0].lValue)                              			
		Skipping = FALSE;
	else   
		Skipping = TRUE;
	return Script_IfThen(ScriptPtr->Next, Skipping);// Let if/then processor handle the rest
}

sScript *CGameScript::Script_AddItem(sScript *ScriptPtr)
{
	sCharacter *CharPtr;
	long ItemNum;
	sCharItem *ItemPtr;
	if((CharPtr = m_App->m_CharController.GetCharacter( ScriptPtr->Entries[2].lValue)) != NULL)                         
	{  	
		if(CharPtr->CharICS != NULL)// Only handle add item if character has an ICS 
		{ 			
			ItemNum = ScriptPtr->Entries[1].lValue;// Get item number in question   		 
			ItemPtr = CharPtr->CharICS->GetParentItem();// Find matching item and add to it's quantity 
			while(ItemPtr != NULL) 	
			{    				   
				if(ItemPtr->ItemNum == ItemNum)// If this is matching item, then just add quantity 		
				{       
					ItemPtr->Quantity += ScriptPtr->Entries[0].lValue;        
					break;     
				}        			        
				ItemPtr = ItemPtr->Next;// Go to next item   
			}   
			// Add to inventory if nothing found 
			int pos=m_App->GetBarterUI().FindTheFirstEmptyGridOfEquipUI();   
			if(ItemPtr == NULL)     
				CharPtr->CharICS->Add(ItemNum, ScriptPtr->Entries[0].lValue,pos);                                
		}
	}
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_RemoveItem(sScript *ScriptPtr)
{
	sCharacter *CharPtr;
	sCharItem *ItemPtr;
	if((CharPtr=m_App->m_CharController.GetCharacter(ScriptPtr->Entries[2].lValue)) != NULL)                             
	{ 
		if(CharPtr->CharICS != NULL)
		{   		   
			ItemPtr = CharPtr->CharICS->GetParentItem();// Find item that we're looking for first 
			while(ItemPtr != NULL)	 
			{     
				// If this is item, remove quantity     
				if(ItemPtr->ItemNum == ScriptPtr->Entries[1].lValue) 		
				{        
					ItemPtr->Quantity -= ScriptPtr->Entries[0].lValue;        
					if(ItemPtr->Quantity <= 0)         
						CharPtr->CharICS->Remove(ItemPtr);        
					break;     
				}          				     
				ItemPtr = ItemPtr->Next;// Go to next item    
			}   
		}
	}
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_AddBarrier(sScript *ScriptPtr)
{
	m_App->m_Barrier.AddTriangle(ScriptPtr->Entries[0].lValue,TRUE,0.0f, 0.0f, 0.0f,0.0f, 0.0f, 0.0f,
		ScriptPtr->Entries[1].fValue,ScriptPtr->Entries[2].fValue,ScriptPtr->Entries[3].fValue,
		ScriptPtr->Entries[4].fValue,ScriptPtr->Entries[5].fValue,ScriptPtr->Entries[6].fValue,  
		ScriptPtr->Entries[7].fValue,ScriptPtr->Entries[8].fValue);                                                                                                                                                                                                                                                                                                                                                                                                              	
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_EnableBarrier(sScript *ScriptPtr)
{
	m_App->m_Barrier.Enable(ScriptPtr->Entries[1].lValue,(!ScriptPtr->Entries[0].Selection) ? TRUE : FALSE);                 
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_RemoveBarrier(sScript *ScriptPtr)
{
    m_App->m_Barrier.Remove(ScriptPtr->Entries[0].lValue);
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_AddTrigger(sScript *ScriptPtr)
{
	m_App->m_Trigger.AddTriangle(ScriptPtr->Entries[0].lValue,TRUE, 
		ScriptPtr->Entries[1].fValue,ScriptPtr->Entries[2].fValue, 
		ScriptPtr->Entries[3].fValue,ScriptPtr->Entries[4].fValue,
		ScriptPtr->Entries[5].fValue,ScriptPtr->Entries[6].fValue,                                                           
		ScriptPtr->Entries[7].fValue,ScriptPtr->Entries[8].fValue);                                                                                                                                                                                                                                                      
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_EnableTrigger(sScript *ScriptPtr)
{
	m_App->m_Trigger.Enable(ScriptPtr->Entries[1].lValue,(!ScriptPtr->Entries[0].Selection) ? TRUE : FALSE);                
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_RemoveTrigger(sScript *ScriptPtr)
{
    m_App->m_Trigger.Remove(ScriptPtr->Entries[0].lValue);
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_Sound(sScript *ScriptPtr)
{
    m_App->PlaySound(ScriptPtr->Entries[0].lValue);
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_Music(sScript *ScriptPtr)
{
    m_App->PlayMusic(ScriptPtr->Entries[0].lValue);
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_StopMusic(sScript *ScriptPtr)
{
    m_App->StopMusic();
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_WinGame(sScript *ScriptPtr)
{
    m_App->WinGame();
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_CommentOrSeparator(sScript *ScriptPtr)                                                 
{
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_Wait(sScript *ScriptPtr)
{
    DWORD Timer = timeGetTime() + ScriptPtr->Entries[0].lValue;   
    while(timeGetTime() < Timer);// Wait for # of milliseconds
    return ScriptPtr->Next;
}

sScript *CGameScript::Script_IfRandThen(sScript *ScriptPtr)
{
    BOOL Skipping;
    // See if random # is >= entry
    if((rand() % ScriptPtr->Entries[0].lValue) >= ScriptPtr->Entries[1].lValue)                                             
        Skipping = FALSE;
    else 
        Skipping = TRUE;  
    return Script_IfThen(ScriptPtr->Next, Skipping);// Let if/then processor handle the rest
}

sScript *CGameScript::Script_Render(sScript *ScriptPtr)
{ 
	if(m_App != NULL) 
	{
		m_App->m_Graphics.ClearZBuffer(); 
		if(m_App->m_Graphics.BeginScene() == TRUE) 	
		{   
			m_App->RenderFrame();  
			m_App->m_Graphics.EndScene();   
		}   
		m_App->m_Graphics.Display();
	}
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_SetFaceToFace(sScript *ScriptPtr)
{
	sCharacter	*Char1=NULL;
	sCharacter	*Char2=NULL;
	float XDiff,ZDiff;
	Char1=m_App->m_CharController.GetCharacter(ScriptPtr->Entries[0].lValue);
	Char2=m_App->m_CharController.GetCharacter(ScriptPtr->Entries[1].lValue);
	if(Char1!=NULL && Char2!=NULL)
	{
		XDiff =Char2->XPos - Char1->XPos;
		ZDiff =Char2->ZPos - Char1->ZPos;
		Char1->Direction=(float)atan2(XDiff,ZDiff);
		XDiff =Char1->XPos - Char2->XPos;
		ZDiff =Char1->ZPos - Char2->ZPos;
		Char2->Direction=(float)atan2(XDiff,ZDiff);
	}
	m_App->m_CharController.Update(0);
	return ScriptPtr->Next;
}

sScript *CGameScript::Script_IfThen(sScript *ScriptPtr,BOOL Skip)                                           
{
	BOOL Skipping = Skip;
	// At this point, Skipping states if the script actions need to be skipped due to a conditional if..then statement.
	// Actions are further processed if skipped = FALSE, looking for an else to flip the skip mode, 
	// or an endif to end the conditional block. 
	while(ScriptPtr != NULL)
	{   	
		if(ScriptPtr->Type == SCRIPT_ELSE)// if else, flip skip mode	    
			Skipping = (Skipping == TRUE) ? FALSE : TRUE;   	
		if(ScriptPtr->Type == SCRIPT_ENDIF)// break on end if    
			return ScriptPtr->Next;
		// Process script function in conditional block making sure to skip actions when condition not met.
		if(Skipping == TRUE)    
			ScriptPtr = ScriptPtr->Next;
		else 
		{    		
			if(ScriptPtr->Type == SCRIPT_GOTO)// Return to normal processing if goto encountered      
				return Process(ScriptPtr);        
			if((ScriptPtr = Process(ScriptPtr)) == NULL)      
				return NULL; 
		}
	}
	return NULL; // End of script reached
}

sScript *CGameScript::Script_LetNPCAttack(sScript *Script)
{	
	sCharacter *CharNPC=m_App->GetCharController()->GetCharacter(Script->Entries[0].lValue);
	sSpell *Spell=m_App->GetSpellController()->GetSpell(1);
	m_App->GetCharController()->SetAction(CharNPC,CHAR_ATTACK);
	sCharacter *CharMonster=m_App->GetCharController()->GetCharacter(Script->Entries[1].lValue);
	m_App->GetCharController()->SpellEffect(CharNPC,CharMonster,Spell);
	return Script->Next;
}

sScript *CGameScript::Script_IfKillThen(sScript *Script)
{
	BOOL Skipping;
	if(curKillMonster==Script->Entries[0].lValue)
		Skipping =FALSE;
	else
		Skipping =TRUE;
	curKillMonster=5;
	return Script_IfThen(Script->Next,Skipping);
}

sScript *CGameScript::Script_IfAllWasKilled(sScript *Script)
{
	BOOL Skipping;
	if(MonstersInLevel==TRUE)
		Skipping =TRUE;
	else
		Skipping =FALSE;
	return Script_IfThen(Script->Next,Skipping);
}

sScript *CGameScript::Script_MoveCamera(sScript *Script)
{
	m_App->MoveCamera(Script->Entries[0].fValue,Script->Entries[1].fValue,		              
					  Script->Entries[2].fValue,Script->Entries[3].fValue,		              
					  Script->Entries[4].fValue,Script->Entries[5].fValue,		              
					  Script->Entries[6].fValue,Script->Entries[7].fValue,		              
					  Script->Entries[8].fValue,Script->Entries[9].fValue,	              
					  Script->Entries[10].fValue,Script->Entries[11].fValue,Script->Entries[12].fValue);	              				  
	m_App->ToMoveCamera();
	return Script->Next;
}

sScript *CGameScript::Script_MoveCameraToChar(sScript *Script)
{
	sCharacter *Char=m_App->GetCharController()->GetCharacter(Script->Entries[0].lValue);
	m_App->MoveCamera(m_App->m_pcAtX,m_App->m_pcAtY,m_App->m_pcAtZ,m_App->m_pcPointX,0,m_App->m_pcPointZ,
		Char->XPos+(float)cos(m_App->m_CameraAngle)*CameraHeight,(float)CameraHeight,	
		Char->ZPos+(float)sin(m_App->m_CameraAngle)*CameraHeight,
		Char->XPos,0.0f,Char->ZPos,Script->Entries[1].fValue);
	m_App->ToMoveCamera();
	return Script->Next;
}

sScript *CGameScript::Script_SetPCControlCamera(sScript *Script)
{
	m_App->bPCControl=Script->Entries[0].bValue;
	return Script->Next;
}

sScript *CGameScript::Script_PlayMP3(sScript *Script)
{
	m_App->PlayMP3(Script->Entries[0].lValue);
	return Script->Next;
}

sScript *CGameScript::Script_StopMP3(sScript *Script)
{
	m_App->StopMP3();
	return Script->Next;
}

sScript *CGameScript::Script_PlayAVI(sScript *Script)
{
	m_App->PlayAVI(Script->Entries[0].lValue);
	return Script->Next;
}

sScript *CGameScript::Script_BlackScreenMessage(sScript *Script)
{
	/*m_BlackScreen.SetText((char*)Script->Entries[0].strText.c_str());
	m_App->m_Mouse.SetLock(0,TRUE);
	m_App->m_Mouse.SetButtonState(0,FALSE);
	while(1)
	{
		m_App->m_Mouse.Acquire();
		m_App->m_Mouse.Read();
		if(m_App->m_Mouse.GetButtonState(0)==TRUE)
		{
			if(m_BlackScreen.bPrintAll)			
				break;
			else
				m_BlackScreen.PrintSpeed=30;
		}
		m_App->m_Graphics.Clear();
		if(m_App->m_Graphics.BeginScene()==TRUE)
		{
			m_App->RenderFrame();
			m_BlackScreen.Render();
			m_App->m_Graphics.EndScene();
		}
		m_App->m_Graphics.Display();
	}
	m_App->m_Mouse.SetLock(0,TRUE);
    m_App->m_Mouse.SetButtonState(0,FALSE);
	return Script->Next;*/
	return NULL;
}
