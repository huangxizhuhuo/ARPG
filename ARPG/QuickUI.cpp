/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#include "QuickUI.h"
#include "Global.h"
#include <sstream>
using namespace std;

CQuickUI::CQuickUI()
{
	m_iStartX=152;
	m_iStartY=538;
	m_iUIWidth=500;
	m_iUIHeight=45;
}

void CQuickUI::Init(CGraphics *Graphics,CInputDevice *Mouse)
{
	m_Graphics=Graphics;
	m_Mouse=Mouse;
	for(int i=0;i<10;i++)
	{
		ItemGrid[i].x1=m_iStartX+35+45*i;
		ItemGrid[i].y1=m_iStartY+2;
	}
	m_bFree=true;
	m_iCurPick=-1;	
}

CQuickUI::~CQuickUI()
{
}

void CQuickUI::Render(POINT *pt)
{
	m_Graphics->BeginSprite();
	m_iSkillCount=g_PCChar->CharSkill->GetNumSkills();
	for(int i=0;i<m_iSkillCount;i++)
	{
		if(g_PCChar->CharSkill->GetSkill(i)->UpDown==1)
		{
		    int iSkillNum=g_PCChar->CharSkill->GetSkill(i)->SkillNum;
		    int iPos=g_PCChar->CharSkill->GetSkill(i)->Position;
		    int Level=g_PCChar->CharSkill->GetSkill(i)->Level;
		    int Exp=g_PCChar->CharSkill->GetSkill(i)->Exp;
		    if(ItemGrid[iPos].bVisible==true)
		    {
			    UseWhichTex(ItemGrid[iPos].x1,ItemGrid[iPos].y1,iSkillNum+306);
			    ItemGrid[iPos].bEmpty=false;
			    ItemGrid[iPos].ID=iSkillNum;//IDΪ���ܵı��
			    ItemGrid[iPos].ID1=i;//ID1Ϊ������CharSkill�����еı��
		    }
		}
	}
	m_iSkillCount=g_PCChar->CharICS->GetNumItems();
	for(int k=0;k<m_iSkillCount;k++)
	{
		if(g_PCChar->CharICS->GetItem(k)->UpDown==1)
		{
			int iPos=(int)g_PCChar->CharICS->GetItem(k)->Position;
		    int iItemIDInMIL=(int)g_PCChar->CharICS->GetItem(k)->ItemNum;
		    int iCount=(int)g_PCChar->CharICS->GetItem(k)->Quantity;
		    if(ItemGrid[iPos].bVisible)
			{
		        UseWhichTex(ItemGrid[iPos].x1,ItemGrid[iPos].y1,atoi(g_MIL[iItemIDInMIL].ImageFilename));		
		        ItemGrid[iPos].bEmpty=false;								
		        ItemGrid[iPos].ID=iItemIDInMIL;
		        ItemGrid[iPos].ID1=k;
			    if(iCount>1)
			    {
				    stringstream ss;
				    ss<<iCount;
				    g_Small.Print(ss.str(),ItemGrid[iPos].x1,ItemGrid[iPos].y1);
			    }
		    }
	    }
	} 
	if(m_iCurPick>-1&&m_iCurPick<7)
	{
		bUseDefaultCursor=false;
		ItemGrid[m_iCurPick].bVisible=false;
		UseWhichTex(pt->x-5,pt->y-5,ItemGrid[m_iCurPick].ID+306);
    }
	else if(m_iCurPick>6&&m_iCurPick<10)
	{
		bUseDefaultCursor=false;
		ItemGrid[m_iCurPick].bVisible=false;
		UseWhichTex(pt->x-5,pt->y-5,atoi(g_MIL[ItemGrid[m_iCurPick].ID].ImageFilename));
	}	
	m_Graphics->EndSprite();
	IfMouseOn(pt);
}
void CQuickUI::IfMouseOn(POINT *pt)//��ʾ������
{
	    int j;
		if((j=GetMouseOnWhichGrid(pt))!=-1)
		{
			//�м��ܲſɵ�ȡ
			if(ItemGrid[j].bEmpty==false)
			{
				//ֻ��û�м��ܱ�ѡ�м����ܿɼ�ʱ��ʾ����
                /*********************************************************************/
				m_Graphics->BeginSprite();
				if(m_iCurPick==-1)
				{
					static int SumFrame=0;
					static int iNum=0;
					if(SumFrame%20==0)
						iNum++;
					if(iNum>1)
						iNum=0;
					static CTexture m_Hand;
					m_Hand.Load(m_Graphics,g_HandCursor[iNum]);
					m_Hand.Blit(pt->x,pt->y);
					bUseDefaultCursor=false;
					SumFrame++;
					m_Hand.Free();
				}
				else
				{
					g_Hand.Blit(pt->x,pt->y);
					bUseDefaultCursor=false;
				}
				m_Graphics->EndSprite();
				if(ItemGrid[j].bVisible&&m_iCurPick==-1)
				{
					if(j>-1&&j<7)
					{
						m_Graphics->BeginSprite();
						g_Texture0.Blit(pt->x,pt->y-180,540,765,230,180);
						m_Graphics->EndSprite();
				        //�����������,�ȼ�,�˺�/�ָ�,����MP,��������(��/��/Զ),����,���¼����辭��,����
				        stringstream Text;
				        long value,CharValue;
				        sSpell *curSpell;
				        sCharSkill *curSkill;
				        curSpell=m_App->GetSpellController()->GetSpell(ItemGrid[j].ID);
				        curSkill=g_PCChar->CharSkill->GetSkill(ItemGrid[j].ID1);
				        g_Mid.Print(curSpell->Name,pt->x+16,pt->y-176);			
				        CharValue=curSkill->Level;
				        //sprintf(Text,"�ȼ� %ld",CharValue);
						Text.str("");
						Text<<"�ȼ� "<<CharValue;
				        g_Mid.Print(Text.str(),pt->x+16,pt->y-144);

						Text.str("");
				        if((value=(long)((float)curSpell->Value[0]*(float)(1.0f+(CharValue-1)*0.2)))<0)
				            Text<<"�˺� "<<-value;//ÿ��һ�������ӳ�20%
				        else
					        Text<<"����+"<<value;				
				        g_Mid.Print(Text.str(),pt->x+16,pt->y-128);
				        //ÿ��һ��������������10%
						Text.str("");
				        value=(long)((float)curSpell->Cost*(float)((curSkill->Level-1)*0.1+1.0f));
				        Text<<"�������� "<<value;
				        g_Mid.Print(Text.str(),pt->x+16,pt->y-112);
				        value=(long)curSpell->Distance;
				        if(value<=256)
					        g_Mid.Print("ʹ����Ч���� ��",pt->x+16,pt->y-96);
				        else if(value<=512)
					        g_Mid.Print("ʹ����Ч���� ��",pt->x+16,pt->y-96);
				         else
					        g_Mid.Print("ʹ����Ч���� Զ",pt->x+16,pt->y-96);

						Text.str("");
				        Text<<"ʹ�þ��� "<<curSkill->Exp;
				        g_Mid.Print(Text.str(),pt->x+16,pt->y-80);
				        CharValue=SkillLevelUp[curSkill->Level-1]-curSkill->Exp;

						Text.str("");
				        Text<<"����һ�����辭�� "<<CharValue;
			            g_Mid.Print(Text.str(),pt->x+16,pt->y-64);
				        g_Mid.Print(curSpell->Description,pt->x+16,pt->y-48,220,64,0xFFFFFFFF,DT_WORDBREAK);								
					}
					else if(j>6&&j<10)
					{
						long value;
						int n=0;
						m_Graphics->BeginSprite();
						g_Texture0.Blit(pt->x,pt->y-70,540,765,100,70);
						m_Graphics->EndSprite();
						g_Mid.Print(g_MIL[ItemGrid[j].ID].Name,pt->x+16,pt->y-64+16*n++);
						if((value=g_MIL[ItemGrid[j].ID].ModifyHP)!=0)
						{
							stringstream Text;
						    Text<<"����+"<<value;
						    g_Mid.Print(Text.str(),pt->x+16,pt->y-64+16*n++);
						}
						if((value=g_MIL[ItemGrid[j].ID].ModifyMP)!=0)
						{
							stringstream Text;
							Text<<"����+"<<value;
							g_Mid.Print(Text.str(),pt->x+16,pt->y-64+16*n++);
						}
						if(m_App->GetBarterUI().GridBasedUIClosed())
						{
							stringstream Text;
							value=g_MIL[ItemGrid[j].ID].Price/2;
							Text<<"��ֵ "<<value<<"��";
							g_Mid.Print(Text.str(),pt->x+16,pt->y-64+16*n++);
						}
					}
				}
                /************************************************************************/

				/*if(m_Mouse->GetButtonState(0)==TRUE&&m_bFree&&m_App->GetSkillUI().GetCurrentPick()==-1\
					&&m_App->GetEquipUI().GetCurrentPick()==-1)
				{				
					m_iCurPick=j;
					m_bFree=false;
				}*/
				if(m_Mouse->GetButtonState(0)&&m_iCurPick==-1&&m_App->GetSkillUI().GetCurrentPick()==-1\
					&&m_App->GetEquipUI().GetCurrentPick()==-1)
				{
					m_iCurPick=j;
					m_Mouse->SetButtonState(0,FALSE);
					m_Mouse->SetLock(0,1);
				}
                /**************************************************************/
				if(m_Mouse->GetButtonState(1)==TRUE&&j>6&&j<10)
				{
					int curUse=j;
					m_Mouse->SetLock(1,TRUE);
					long value;			
					if(g_MIL[ItemGrid[curUse].ID].Category==FOOD||g_MIL[ItemGrid[curUse].ID].Category==HEAL)			
					{
						m_App->PlaySound(12);				
						if((value=g_MIL[ItemGrid[curUse].ID].ModifyHP)!=0)			
						{					
							g_PCChar->HealthPoints+=value;					
							if(g_PCChar->HealthPoints>m_App->GetCharController()->GetHP(g_PCChar))					
								g_PCChar->HealthPoints=m_App->GetCharController()->GetHP(g_PCChar);				
						}			
						if((value=g_MIL[ItemGrid[curUse].ID].ModifyMP)!=0)				
						{					
							g_PCChar->ManaPoints+=value;						
							if(g_PCChar->ManaPoints>m_App->GetCharController()->GetMP(g_PCChar))						
								g_PCChar->ManaPoints=m_App->GetCharController()->GetMP(g_PCChar);				
						}				
						sCharItem *ItemPtr;				
						ItemPtr=g_PCChar->CharICS->GetItem(ItemGrid[curUse].ID1);				
						if(CheckItemFlag(g_MIL[ItemPtr->ItemNum].Flags,USEONCE))			
						{			
							if(ItemPtr->Quantity==1)
							{
								g_PCChar->CharICS->Remove(ItemPtr);	
								ItemGrid[curUse].bEmpty=true;
							}
							else				
							{					
								ItemPtr->Quantity--;				
							}			
						}
					}                   
				}
				/**************************************************************************/
			}
			else//bEmpty=false
			{
				m_Graphics->BeginSprite();
				bUseDefaultCursor=true;
				if(m_iCurPick!=-1)
				{
					bUseDefaultCursor=false;
					g_Hand.Blit(pt->x,pt->y);
				}
				m_Graphics->EndSprite();
			}
		}
		else//j==-1
		{
			m_Graphics->BeginSprite();
			//bUseDefaultCursor=true;
			if(m_iCurPick!=-1)
			{
				bUseDefaultCursor=false;
				g_Hand.Blit(pt->x,pt->y);
			}
			m_Graphics->EndSprite();
		}
		//if(m_Mouse->GetButtonState(0)==FALSE&&!m_bFree)
		if(m_Mouse->GetButtonState(0)&&m_iCurPick!=-1&&m_App->GetSkillUI().GetCurrentPick()==-1		
			&&m_App->GetEquipUI().GetCurrentPick()==-1)
		{
			m_Mouse->SetButtonState(0,FALSE);
			m_Mouse->SetLock(0,1);
			if(m_iCurPick>=0&&m_iCurPick<7&&j>=0&&j<=6)//(j>=0&&j<=10)
			{					
				if(ItemGrid[j].bEmpty)					
				{					
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->Position=j;					
					ItemGrid[m_iCurPick].bEmpty=true;	
					//ֻ���λ�øı�һ��,��ԭ���ÿ�					
				}				
				else			
				{					
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->Position=j;					
					g_PCChar->CharSkill->GetSkill(ItemGrid[j].ID1)->Position=m_iCurPick;					
				}
			}
			else if(m_iCurPick>=7&&m_iCurPick<=9&&j>6&&j<10)
			{
				if(ItemGrid[j].bEmpty)
				{
					g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ID1)->Position=j;
					ItemGrid[m_iCurPick].bEmpty=true;
				}
				else
				{
					g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ID1)->Position=j;
					g_PCChar->CharICS->GetItem(ItemGrid[j].ID1)->Position=m_iCurPick;
				}
			}
			j=m_App->GetSkillUI().GetMouseOnWhichGrid(pt);
			if(m_iCurPick>-1&&m_iCurPick<7&&j>=0&&j<=25&&m_App->GetSkillUI().GridBasedUIClosed())
			{
				if(m_App->GetSkillUI().ItemGrid[j].bEmpty)
				{
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->Position=j;
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->UpDown=0;
					ItemGrid[m_iCurPick].bEmpty=true;
				}
				else
				{
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->Position=j;
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->UpDown=0;
					g_PCChar->CharSkill->GetSkill(m_App->GetSkillUI().ItemGrid[j].ID1)->Position=m_iCurPick;
					g_PCChar->CharSkill->GetSkill(m_App->GetSkillUI().ItemGrid[j].ID1)->UpDown=1;
				}
			}
			j=m_App->GetEquipUI().GetMouseOnWhichGrid(pt);
			if(m_iCurPick>6&&m_iCurPick<10&&j>=0&&j<=19&&m_App->GetEquipUI().GridBasedUIClosed())
			{
				if(m_App->GetEquipUI().ItemGrid[j].bEmpty)
				{
					g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ID1)->Position=j;
	                g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ID1)->UpDown=0;
					ItemGrid[m_iCurPick].bEmpty=true;
				}
				//sItemGrid ItemGrid=m_App->GetEquipUI().ItemGrid[j];
				else if(!m_App->GetEquipUI().ItemGrid[j].bEmpty&&(g_MIL[m_App->GetEquipUI().ItemGrid[j].ID].Category==FOOD||\
					g_MIL[m_App->GetEquipUI().ItemGrid[j].ID].Category==HEAL))
				{
					g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ID1)->Position=j;
					g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ID1)->UpDown=0;
					g_PCChar->CharICS->GetItem(m_App->GetEquipUI().ItemGrid[j].ICSID)->Position=m_iCurPick;
					g_PCChar->CharICS->GetItem(m_App->GetEquipUI().ItemGrid[j].ICSID)->UpDown=1;
				}
			}
            /********************************************************************/
			int bx,by;
			bx=m_App->GetBarterUI().GetStartX();
			by=m_App->GetBarterUI().GetStartY();
			if(pt->x>bx&&pt->x<bx+250&&pt->y>by&&pt->y<by+390&&\
				m_App->GetBarterUI().GridBasedUIClosed())//���׽���򿪵�����½���Ʒ�������� ��
			{
				sCharItem *ScanPtr;
				ScanPtr=g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ID1);				
				if(CheckItemFlag(g_MIL[ScanPtr->ItemNum].Flags,SELLABLE))
				{
				m_App->PlaySound(16);
				if(ScanPtr->Quantity>1)
				{
					ScanPtr->Quantity--;
					g_PCChar->Def.Money+=g_MIL[ScanPtr->ItemNum].Price/2;
				}
				else
				{
					g_PCChar->Def.Money+=g_MIL[ScanPtr->ItemNum].Price/2;
					g_PCChar->CharICS->Remove(ScanPtr);					
					ItemGrid[m_iCurPick].bEmpty=true;
				}
				}
				else
				{
					m_App->PlaySound(17);
				}
			}
            /********************************************************************/
			m_bFree=true;
			ItemGrid[m_iCurPick].bVisible=true;
			m_iCurPick=-1;			
		}  
}

//�õ����ָ��λ�ںδ�,���ط�����
int CQuickUI::GetMouseOnWhichGrid(POINT *pt)
{
		for(int i=0;i<10;i++)
		{							
			if(InRect(((45)*i+35+m_iStartX),(2+m_iStartY),pt))//45,55				
				return i;		
		}
	    return -1;//û���Ƶ��κο�
}

