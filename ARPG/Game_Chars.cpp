/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#include "Global.h"

int curUseSpellIDInCharSkill;
bool CanGetInput=true;

BOOL CChars::PCUpdate(sCharacter *Character, long Elapsed, float *XMove, float *YMove, float *ZMove)                      
{
  
	float Speed;
    sCharacter *TargetChar;
    float XDiff, YDiff, ZDiff;
    float Dist, Range;
    char Filename[MAX_PATH];
    long Spell = -1; 
	if(!Elapsed)// Don't update if no elapsed time   
		return TRUE;
	long  Action ;
    static long TempAction;
    Action = m_App->GetInput();
	if(Action&512)
	{ 
		TempAction=Action;
	}
	Speed = (float)Elapsed / 1000.0f * GetSpeed(Character);
	if(m_App != NULL)// Get the input flags  
		if(TempAction==0)	  
			Action = m_App->GetInput();
	if(Action&1)//前
	{	 
		TempAction=0;	 
		*XMove = (float)sin(Character->Direction) * Speed;   
		*ZMove = (float)cos(Character->Direction) * Speed;  
		Character->Action = CHAR_MOVE;	  
	}
	if(Action&2)//左
	{	 
		TempAction=0;	 
		Character->Direction -= (float)Elapsed / 1000.0f * 4.0f;   
		Character->Action = CHAR_MOVE;
	}
	if(Action&4)//右
	{  
		TempAction=0;	 
		Character->Direction += (float)Elapsed / 1000.0f * 4.0f;    
		Character->Action = CHAR_MOVE;
	}
	if(Action&1024||Action&2048||Action&4096)
	{
		TempAction=0;	 
		int curUse; 
		if(Action&1024)	  
			curUse=7; 
		if(Action&2048)	  
			curUse=8;	 
		if(Action&4096)	 
			curUse=9; 
		long value;  
		if(m_MIL[m_App->m_QuickUI.ItemGrid[curUse].ID].Category==FOOD||
			m_MIL[m_App->m_QuickUI.ItemGrid[curUse].ID].Category==HEAL)	  
		{	  
			m_App->PlaySound(12);	  
			if((value=m_MIL[m_App->m_QuickUI.ItemGrid[curUse].ID].ModifyHP)!=0)	 
			{		 
				g_PCChar->HealthPoints+=value;		 
				if(g_PCChar->HealthPoints>m_App->GetCharController()->GetHP(g_PCChar))			  
					g_PCChar->HealthPoints=m_App->GetCharController()->GetHP(g_PCChar);	 
			}	 
			if((value=m_MIL[m_App->m_QuickUI.ItemGrid[curUse].ID].ModifyMP)!=0)	 
			{	  
				g_PCChar->ManaPoints+=value;		 
				if(g_PCChar->ManaPoints>m_App->GetCharController()->GetMP(g_PCChar))			 
					g_PCChar->ManaPoints=m_App->GetCharController()->GetMP(g_PCChar);	 
			}	 
			sCharItem *ItemPtr;						 
			ItemPtr=g_PCChar->CharICS->GetItem(m_App->m_QuickUI.ItemGrid[curUse].ID1);					 
			if(CheckItemFlag(m_MIL[ItemPtr->ItemNum].Flags,USEONCE))				 
			{						
				if(ItemPtr->Quantity==1)
				{					
					g_PCChar->CharICS->Remove(ItemPtr);					
					m_App->m_QuickUI.ItemGrid[curUse].bEmpty=true;
				}
				else														
					ItemPtr->Quantity--;							  
			}		 
			if(curUse==7)     
				m_App->m_Keyboard.SetLock(KEY_1,TRUE);	 
			else if(curUse==8)		  
				m_App->m_Keyboard.SetLock(KEY_2,TRUE);  
			else if(curUse==9)	 
				m_App->m_Keyboard.SetLock(KEY_3,TRUE);	
		}
	}
	if(Action&8||Action&16||Action&32||Action&64||Action&128||Action&8192||Action&16384) 
	{
		TempAction=0;	 
		if(Action&8)	 
		{		 
			Spell=m_App->GetQuickUI().ItemGrid[0].ID;	  
			curUseSpellIDInCharSkill=m_App->GetQuickUI().ItemGrid[0].ID1;  
		}
		if(Action&16)  
		{	 
			Spell=m_App->GetQuickUI().ItemGrid[1].ID;	 
			curUseSpellIDInCharSkill=m_App->GetQuickUI().ItemGrid[1].ID1;
		}
		if(Action&32)  
		{	
			Spell=m_App->GetQuickUI().ItemGrid[2].ID;	 
			curUseSpellIDInCharSkill=m_App->GetQuickUI().ItemGrid[2].ID1;
		}
		if(Action&64)
		{	 
			Spell=m_App->GetQuickUI().ItemGrid[3].ID; 
			curUseSpellIDInCharSkill=m_App->GetQuickUI().ItemGrid[3].ID1; 
		}
		if(Action&128) 
		{	  
			Spell=m_App->GetQuickUI().ItemGrid[4].ID;	  
			curUseSpellIDInCharSkill=m_App->GetQuickUI().ItemGrid[4].ID1;  
		} 
		if(Action&8192)
		{  
			Spell=m_App->GetQuickUI().ItemGrid[5].ID;	  
			curUseSpellIDInCharSkill=m_App->GetQuickUI().ItemGrid[5].ID1; 
		}  
		if(Action&16384) 
		{	 
			Spell=m_App->GetQuickUI().ItemGrid[6].ID;		  
			curUseSpellIDInCharSkill=m_App->GetQuickUI().ItemGrid[6].ID1;	 
		}	   
		if(Spell != -1)// Cast spell if commanded 
		{  		     
			if(g_PCChar->Def.MagiCSpells[Spell/32]&(1<<(Spell&31)) && 
				g_PCChar->ManaPoints >=m_App->m_SpellController.GetSpell(Spell)->Cost)                                        	 
			{// Only cast if spell known and has enough mana points    								
				if((TargetChar = m_App->GetCharacterAt(m_App->m_Mouse.GetXPos(),m_App->m_Mouse.GetYPos()))!=NULL)                                                                      		 
				{// See which character is being pointed   			   
				  if(TargetChar->Type != CHAR_NPC) // Don't target NPC's     		
				  {        		 
					  // Get distance to target        
					  XDiff = (float)fabs(TargetChar->XPos-Character->XPos);         
					  YDiff = (float)fabs(TargetChar->YPos-Character->YPos);         
					  ZDiff = (float)fabs(TargetChar->ZPos-Character->ZPos);        
					  Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;        
					  // Offset dist by target's radius       
					  Range = GetXZRadius(TargetChar);        
					  Dist -= (Range * Range);        
					  // Get maximum spell range        
					  Range = GetXZRadius(Character);       
					  Range +=m_App->m_SpellController.GetSpell(Spell)->Distance;                                                               			         
					  if(Dist <= (Range * Range))// Only perform spell if target in range  	  
					  {         
  					      // Set spell data        
						  Character->SpellNum    = Spell;         
						  Character->SpellTarget = CHAR_MONSTER;         
						  // Store target coordinates         
						  Character->TargetX = TargetChar->XPos;       
						  Character->TargetY = TargetChar->YPos;           
						  Character->TargetZ = TargetChar->ZPos;       				   
						  (*XMove) = (*YMove) = (*ZMove) = 0.0f;// Clear movement               			        
						  SetAction(Character, CHAR_SPELL);// Perform spell action                
						  // Face victim          
						  XDiff = TargetChar->XPos - Character->XPos;          
						  ZDiff = TargetChar->ZPos - Character->ZPos;            
						  Character->Direction = (float)atan2(XDiff, ZDiff);         				 
						  m_App->m_CharController.SetAction(Character,CHAR_SPELL);// Set action                                                          
					  }     			 
				  }    		 
				}    
			}	    
		}
	}
	if(Action&256)
	{	 
		TempAction=0;	  
		TargetChar=m_App->m_CharController.GetCharacter(m_App->TargetID); 	   
		if(TargetChar->Type == CHAR_NPC)// Handle talking to NPCs 	 
		{      
			// No distance checks, just process their script		     
			sprintf(Filename, "..\\Script\\Char%lu.mls", TargetChar->ID);                            
			m_App->m_Script.Execute(Filename);    
			return TRUE; // Don't process anymore      
		}                          
		if(TargetChar->Type == CHAR_MONSTER)// Handle attacking monsters	 
		{     
			// Get distance to target    
			XDiff = (float)fabs(TargetChar->XPos-Character->XPos);     
			YDiff = (float)fabs(TargetChar->YPos-Character->YPos);    
			ZDiff = (float)fabs(TargetChar->ZPos-Character->ZPos);  
			Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff; 
			// Offset dist by target's radius  
			Range = GetXZRadius(TargetChar);  
			Dist -= (Range * Range);    
			// Get maximum attack range 
			Range = GetXZRadius(Character);  
			Range += Character->Def.Range;   			   
			if(Dist <= (Range * Range))// Only perform attack if target in range 	
			{        
				// Set target/victim info    
				TargetChar->Attacker = Character;       
				Character->Victim    = TargetChar;        
				// Face victim      
				XDiff = TargetChar->XPos - Character->XPos;        
				ZDiff = TargetChar->ZPos - Character->ZPos;         
				Character->Direction = (float)atan2(XDiff, ZDiff);       			     
				m_App->m_CharController.SetAction(Character, CHAR_ATTACK);// Set action                                                 
			}	
		}
	}
	if(TempAction&512)
	{	 
		Character->Direction=(float)atan2((m_App->vecMouse.x-m_App->charX),(m_App->vecMouse.z-m_App->charZ));
	    *XMove = (float)sin(Character->Direction) * Speed;
        *ZMove = (float)cos(Character->Direction) * Speed;
	    float MoveX,MoveZ;
	    MoveX=(float)sin(Character->Direction) * Speed;
	    MoveZ=(float)cos(Character->Direction) * Speed;
	    float CurCharX,CurCharZ;
	    CurCharZ=Character->ZPos;
	    CurCharX=Character->XPos;
	    XDiff = (float)fabs(Character->XPos - m_App->vecMouse.x);
        ZDiff = (float)fabs(Character->ZPos - m_App->vecMouse.z);
        Dist  = XDiff*XDiff + ZDiff*ZDiff;
	    Dist  = (float)sqrt(Dist);
	    Character->Action=CHAR_MOVE;
	    if(Character->Direction>=0&&Character->Direction<1.57)//根据四个象限方向确定不同的抵达检测
	    {		 
		    if(((CurCharX+MoveX)>=m_App->vecMouse.x)&&((CurCharZ+MoveZ)>=m_App->vecMouse.z))			
			    TempAction=0;
	    }
	    else if(Character->Direction>=1.57&&Character->Direction<3.14) 
	    { 
		    if(((CurCharX+MoveX)>=m_App->vecMouse.x)&&((CurCharZ+MoveZ)<=m_App->vecMouse.z))		
			    TempAction=0;	
	    }
	    else if(Character->Direction>=-3.14&&Character->Direction<-1.57)
	    {	 
		    if(((CurCharX+MoveX)<=m_App->vecMouse.x)&&((CurCharZ+MoveZ)<=m_App->vecMouse.z))				  
			    TempAction=0;		  
	    }		  
	    else if(Character->Direction>=-1.57&&Character->Direction<0)	  
	    {			 
		    if(((CurCharX+MoveX)<=m_App->vecMouse.x)&&((CurCharZ+MoveZ)>=m_App->vecMouse.z))				
			    TempAction=0;			 
	    }
    } 
	//进入触发区域,TempAction=0
	//if(m_App->m_Trigger.GetTrigger(Character->XPos,Character->YPos,Character->ZPos)!=0)
	//	  TempAction=0;
	if(m_App->m_Keyboard.GetKeyState(KEY_Q) == TRUE&&!m_App->m_EquipUI.GridBasedUIClosed())   
	{// Enter status frame if Q button pressed	  
		if(m_App->m_SkillUI.GridBasedUIClosed())
		{	
			m_App->m_SkillUI.CloseSkillUI();//	
			if(bool1==false)	
			{	
				m_App->m_StateManager.Pop(m_App);					
				m_App->m_StateManager.Push(m_App->StatusFrame, m_App);		
			}	
			else				
			{		
				m_App->m_StateManager.Pop(m_App);		 
				m_App->m_StateManager.Pop(m_App);	 
				m_App->m_StateManager.Push(m_App->BarterFrame,m_App);	 
				m_App->m_StateManager.Push(m_App->StatusFrame,m_App);	
			}
		}	
		else		
			m_App->m_StateManager.Push(m_App->StatusFrame, m_App);
		m_App->m_Keyboard.SetLock(KEY_Q, TRUE);	
		EquipOrSkill=true;
		m_App->PlaySound(13);
	}
	if(m_App->m_Keyboard.GetKeyState(KEY_E)==TRUE&&!m_App->m_SkillUI.GridBasedUIClosed())
	{	 
		if(m_App->m_EquipUI.GridBasedUIClosed())	 
		{	 
			m_App->m_EquipUI.CloseEquipUI();	
			if(bool1==false)  
			{
				m_App->m_StateManager.Pop(m_App);//如果打开技能界面时没有关闭装备界面，则把上一帧弹出栈，即关闭之；那么如果交易界面同时打开		                                   			  	
				m_App->m_StateManager.Push(m_App->StatusFrame, m_App); 
			} 
			else	 
			{		
				m_App->m_StateManager.Pop(m_App);	 
				m_App->m_StateManager.Pop(m_App);		
				m_App->m_StateManager.Push(m_App->BarterFrame,m_App);	
				m_App->m_StateManager.Push(m_App->StatusFrame,m_App);	 
			} 
		} 
		else	 				 
			m_App->m_StateManager.Push(m_App->StatusFrame,m_App);			 
		m_App->m_Keyboard.SetLock(KEY_E,TRUE);  
		EquipOrSkill=false; 
		m_App->PlaySound(13);
	} 
	return TRUE;
}

BOOL CChars::ValidateMove(sCharacter *Character,float *XMove, float *YMove, float *ZMove)                                 
{
	float Height;
	if(m_App != NULL)// Check against terrain mesh for collision
	{
		//+8.0f?
		if(m_App->CheckIntersect(Character->XPos,Character->YPos + 8.0f,Character->ZPos,
			*XMove + Character->XPos,*YMove + Character->YPos + 8.0f, *ZMove + Character->ZPos, NULL) == TRUE) 		                                                                                                                                                                                                                                           
			return FALSE;
	}
	// Get new height
	Height = m_App->GetHeightBelow(*XMove + Character->XPos,32.0f  + Character->YPos,*ZMove + Character->ZPos);                                                                       
	*YMove = Height - Character->YPos; 
	// Check against barriers and clear movement if any
	if(m_App->m_Barrier.GetBarrier(*XMove + Character->XPos,*YMove + Character->YPos,*ZMove + Character->ZPos)!=0)                                                                        
		(*XMove) = (*YMove) = (*ZMove) = 0.0f;
	return TRUE;
}

BOOL CChars::Experience(sCharacter *Character, long Amount)
{ 
	m_App->m_CombatExp += Amount;
	return FALSE;  // Don't display message
}

BOOL CChars::PCTeleport(sCharacter *Character, sSpell *Spell)
{
	m_App->SetupTeleport(1, 100.0f, 0.0f, -170.0f);// Teleport player to town
 	return TRUE;
}

BOOL CChars::ActionSound(sCharacter *Character)
{
	if(m_App == NULL || Character == NULL) 
		return TRUE;
	switch(Character->Action)// Play sound based on character type and action
	{  
	case CHAR_ATTACK:      
		if(!Character->ID)      
			m_App->PlaySound(0);   
		else      
			m_App->PlaySound(1);   
		break;
	case CHAR_SPELL:   
		m_App->PlaySound(2);    
		break;
    case CHAR_HURT:     
		if(!Character->ID)     
			m_App->PlaySound(4); 
		else       
			m_App->PlaySound(5);     
		break;
    case CHAR_DIE:     
		if(!Character->ID)       
			m_App->PlaySound(6);   
		else      
			m_App->PlaySound(7);    
		break;  
	}
	return TRUE;
}

BOOL CChars::DropMoney(float XPos, float YPos, float ZPos,long Quantity)                          
{
	m_App->m_CombatMoney += Quantity; 
	return TRUE;
}

BOOL CChars::DropItem(float XPos, float YPos, float ZPos,long Item, long Quantity)                          
{
	m_App->m_CombatItems[Item] = 1;
	return TRUE;
}
