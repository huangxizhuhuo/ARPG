/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#include "SkillUI.h"
#include "Global.h"
#include <sstream>
using namespace std;

CSkillUI::CSkillUI()
{
	m_iStartX=545;
	m_iStartY=10;
	m_iUIWidth=250;
	m_iUIHeight=520;
	m_bRender=false;
}

void CSkillUI::Init(CGraphics *Graphics,CInputDevice *Mouse)
{
	m_Graphics=Graphics;
	m_Mouse=Mouse;
	for(int i=0;i<25;i++)//45,55
	{
		ItemGrid[i].x1=m_iStartX+15+45*(i%5);
		ItemGrid[i].y1=m_iStartY+200+55*(i/5);
	}
	//ItemGrid[25]�������ܸ�
	ItemGrid[25].x1=m_iStartX+15;
	ItemGrid[25].y1=m_iStartY+78;
	m_bFree=true;
	m_iCurPick=-1;	
}

CSkillUI::~CSkillUI()
{
	m_bRender=false;
}

int CSkillUI::FindTheFirstEmptyGrid()
{
	for(int i=0;i<25;i++)
	{
		if(ItemGrid[i].bEmpty==true)		
			return i;
	}
	return -1;
}

void CSkillUI::Render(POINT *pt)
{
	m_bRender=true;
	m_Graphics->BeginSprite();
	g_Texture1.Blit(m_iStartX,m_iStartY,0,0,250,520);	
	m_iSkillCount=g_PCChar->CharSkill->GetNumSkills();
	for(int i=0;i<m_iSkillCount;i++)
	{
		if(g_PCChar->CharSkill->GetSkill(i)->UpDown==0)
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
	if(pt->x>=(m_iStartX+230)&&pt->x<=(m_iStartX+250)&&pt->y>=m_iStartY&&pt->y<=(m_iStartY+20))
	{
		g_Texture1.Blit(m_iStartX+230,m_iStartY,920,113,20,20);
	}
	else		
		g_Texture1.Blit(m_iStartX+230,m_iStartY,900,113,20,20);/**/
	if(m_iCurPick!=-1)
	{
		bUseDefaultCursor=false;
		//curHide=ItemGrid[m_iCurPick].ID;
		ItemGrid[m_iCurPick].bVisible=false;
		//int curPic=(int)ICS->GetItem(ItemGrid[m_iCurPick].ICSID)->ItemNum;
		UseWhichTex(pt->x-5,pt->y-5,ItemGrid[m_iCurPick].ID+306);
    }
	m_Graphics->EndSprite();
	g_Mid.Print("��ѧ",m_iStartX+10,m_iStartY);
	if(ItemGrid[25].bEmpty==false&&ItemGrid[25].bVisible==true)
	{
		int m,n;
		m=ItemGrid[25].ID1;
		n=ItemGrid[25].ID;
		stringstream Text;	
		Text<<g_PCChar->CharSkill->GetSkill(m)->Level;
		g_Mid.Print(Text.str(),220+m_iStartX,105+m_iStartY);
		Text<<g_PCChar->CharSkill->GetSkill(m)->Exp;
		g_Mid.Print(Text.str(),120+m_iStartX,108+m_iStartY);
		Text<<m_App->GetSpellController()->GetSpell(n)->Name;
		g_Mid.Print(Text.str(),100+m_iStartX,80+m_iStartY);

	}
	IfMouseOn(pt);
}

void CSkillUI::IfMouseOn(POINT *pt)//��ʾ������
{
	    int j;
		m_Graphics->BeginSprite();
		if((j=GetMouseOnWhichGrid(pt))!=-1)
		{
			//�м��ܲſɵ�ȡ
			if(ItemGrid[j].bEmpty==false)
			{				
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
				//ֻ��û�м��ܱ�ѡ�м����ܿɼ�ʱ��ʾ����
				if(ItemGrid[j].bVisible&&m_iCurPick==-1)
				{
					m_Graphics->BeginSprite();
				    g_Texture0.Blit(pt->x-210,pt->y-180,540,765,230,180);
					m_Graphics->EndSprite();
				   //�����������,�ȼ�,�˺�/�ָ�,����MP,��������(��/��/Զ),����,���¼����辭��,����
				   stringstream Text;
				   long value,CharValue;
				   sSpell *curSpell;
				   sCharSkill *curSkill;
				   curSpell=m_App->GetSpellController()->GetSpell(ItemGrid[j].ID);
				   curSkill=g_PCChar->CharSkill->GetSkill(ItemGrid[j].ID1);
				   g_Mid.Print(curSpell->Name,pt->x-210+16,pt->y-176);			
				   CharValue=curSkill->Level;
				   Text.str("");
				   Text<<"�ȼ� "<<CharValue;
				   g_Mid.Print(Text.str(),pt->x-210+16,pt->y-144);
				   Text.str("");
				   if((value=(long)((float)curSpell->Value[0]*(float)(1.0f+(CharValue-1)*0.2)))<0)
				       Text<<"�˺� "<<-value;//ÿ��һ�������ӳ�20%
				   else
					   Text<<"����+"<<value;				
				   g_Mid.Print(Text.str(),pt->x-210+16,pt->y-128);
				   //ÿ��һ��������������10%
				   Text.str("");
				   value=(long)((float)curSpell->Cost*(float)((curSkill->Level-1)*0.1+1.0f));
				   Text<<"�������� "<<value;
				   g_Mid.Print(Text.str(),pt->x-210+16,pt->y-112);

				   Text.str("");
				   value=(long)curSpell->Distance;
				   if(value<=256)
					   g_Mid.Print("ʹ����Ч���� ��",pt->x-210+16,pt->y-96);
				   else if(value<=512)
					   g_Mid.Print("ʹ����Ч���� ��",pt->x-210+16,pt->y-96);
				   else
					   g_Mid.Print("ʹ����Ч���� Զ",pt->x-210+16,pt->y-96);

				   Text.str("");
				   Text<<"ʹ�þ��� "<<curSkill->Exp;
				   g_Mid.Print(Text.str(),pt->x-210+16,pt->y-80);
				   CharValue=SkillLevelUp[curSkill->Level-1]-curSkill->Exp;
				   Text.str("");
				   Text<<"����һ�����辭�� "<<CharValue;
				   g_Mid.Print(Text.str(),pt->x-210+16,pt->y-64);
				   g_Mid.Print(curSpell->Description,pt->x-210+16,pt->y-48,220,64,0xFFFFFFFF,DT_WORDBREAK);							
				}
				/*if(m_Mouse->GetButtonState(0)==TRUE&&m_bFree&&m_App->GetQuickUI().GetCurrentPick()==-1)
				{				
					m_iCurPick=j;
					m_bFree=false;
				}*/
				if(m_Mouse->GetButtonState(0)&&m_iCurPick==-1&&m_App->GetQuickUI().GetCurrentPick()==-1)
				{
					m_iCurPick=j;
					m_Mouse->SetButtonState(0,FALSE);
					m_Mouse->SetLock(0,1);
				}
			}
			else//bEmpty=true;
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
		else//j=-1
		{
			m_Graphics->BeginSprite();
			sCharacter *NPCPointTo=m_App->GetCharacterAt(pt->x,pt->y);
			if(NPCPointTo==NULL)
			{	 	        
				bUseDefaultCursor=true;
			}  
	        else 
			{
				if(!m_App->GetBarterUI().GridBasedUIClosed())
				{        
					if(NPCPointTo->Type==CHAR_NPC&&!MouseStopOnGridBasedUI(m_Mouse)&&!m_App->IfThereIsAPick())		
					{		        
						bUseDefaultCursor=false;		        
						static CTexture m_TalkCursor;		        
						static int iNum=0,SumFrame=0;		        
						SumFrame++;	        
						if(SumFrame%10==0)			        
							iNum++;		        
						if(iNum>3)			        
							iNum=0;		        
						m_TalkCursor.Load(m_Graphics,g_Cursor[iNum]);		        
						m_TalkCursor.Blit(pt->x,pt->y);		        
						m_TalkCursor.Free();	 			
					} 		
				}		
			}
			if(m_iCurPick!=-1)
			{
				bUseDefaultCursor=false;
				g_Hand.Blit(pt->x,pt->y);
			}			
		}
		//if(m_Mouse->GetButtonState(0)==FALSE&&!m_bFree)
		if(m_Mouse->GetButtonState(0)&&m_iCurPick!=-1&&m_App->GetQuickUI().GetCurrentPick()==-1)
		{
			m_Mouse->SetButtonState(0,FALSE);
			m_Mouse->SetLock(0,1);
			if(j>=0&&j<=25)
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
			j=m_App->GetQuickUI().GetMouseOnWhichGrid(pt);
			if(j>=0&&j<=6)
			{
				if(m_App->GetQuickUI().ItemGrid[j].bEmpty)
				{
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->Position=j;
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->UpDown=1;
					ItemGrid[m_iCurPick].bEmpty=true;
				}
				else
				{
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->Position=j;
					g_PCChar->CharSkill->GetSkill(ItemGrid[m_iCurPick].ID1)->UpDown=1;
					g_PCChar->CharSkill->GetSkill(m_App->GetQuickUI().ItemGrid[j].ID1)->Position=m_iCurPick;

					g_PCChar->CharSkill->GetSkill(m_App->GetQuickUI().ItemGrid[j].ID1)->UpDown=0;
				}
			}
			m_bFree=true;
			ItemGrid[m_iCurPick].bVisible=true;
			m_iCurPick=-1;			
		}  
		m_Graphics->EndSprite();
}

//�õ����ָ��λ�ںδ�,���ط�����
int CSkillUI::GetMouseOnWhichGrid(POINT *pt)
{
	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			if(InRect(((45)*j+15+m_iStartX),((55)*i+200+m_iStartY),pt))//45,55
				return (i*5+j);
		}
	}
	if(InRect(m_iStartX+15,m_iStartY+78,pt))
		return 25;
	return -1;//û���Ƶ��κο�
}

void CSkillUI::CloseSkillUI()
{
	m_bRender=false;
	for(int i=0;i<=25;i++)
		ItemGrid[i].bEmpty=true;
}

int CSkillUI::GetCurrentStudy()
{
	if(ItemGrid[25].bEmpty)
		return -1;
	else
	{
		return ItemGrid[25].ID;
	}
}