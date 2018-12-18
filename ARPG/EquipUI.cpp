/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
/*Load֮����New����װ��ж�»���bug��SKillUIҲ����������NewӦ�ý���ItemGrid��λ*/
#include "EquipUI.h"
#include "Global.h"
#include <sstream>
using namespace std;

char *g_HandCursor[]=
{
	{"..\\Image\\cursor\\hand.png"},
	{"..\\Image\\cursor\\hand1.png"}
};

CEquipUI::CEquipUI()
{
	m_iStartX=545;
	m_iStartY=60;
	m_iUIWidth=250;
	m_iUIHeight=390;
	m_bRender=false;
}

void CEquipUI::Init(CGraphics *Graphics,CInputDevice *Mouse)
{
	m_Graphics=Graphics;
	m_Mouse=Mouse;
	for(int i=0;i<20;i++)
	{
		ItemGrid[i].x1=m_iStartX+10+45*(i%5);
		ItemGrid[i].y1=m_iStartY+180+45*(i/5);
	}
	m_bFree=true;
	m_bUp=false;
	m_iCurPick=-1;	
    m_bTheFirstTime=false;
	//�ֳ�
	ItemGrid[20].x1=11+m_iStartX;
	ItemGrid[20].y1=72+m_iStartY;
	//ͷ��
	ItemGrid[21].x1=55+m_iStartX;
	ItemGrid[21].y1=27+m_iStartY;
	//�Ŵ�
	ItemGrid[22].x1=55+m_iStartX;
	ItemGrid[22].y1=115+m_iStartY;
	//����
	ItemGrid[23].x1=100+m_iStartX;
	ItemGrid[23].y1=72+m_iStartY;
	//������Ʒ		
	ItemGrid[24].x1=145+m_iStartX;
	ItemGrid[24].y1=25+m_iStartY;
	ItemGrid[25].x1=190+m_iStartX;
	ItemGrid[25].y1=25+m_iStartY;			
	ItemGrid[26].x1=145+m_iStartX;
	ItemGrid[26].y1=70+m_iStartY;			
	ItemGrid[27].x1=190+m_iStartX;
	ItemGrid[27].y1=70+m_iStartY;			
	ItemGrid[28].x1=145+m_iStartX;
	ItemGrid[28].y1=115+m_iStartY;			
	ItemGrid[29].x1=190+m_iStartX;
	ItemGrid[29].y1=115+m_iStartY;			
}

CEquipUI::~CEquipUI()
{
	m_bRender=false;
}

//����������,���ص�һ���յķ���,-1��ʾ��
int CEquipUI::FindTheFirstEmptyGrid()
{
	for(int i=0;i<20;i++)
	{
		if(ItemGrid[i].bEmpty==true)		
			return i;
	}
	return -1;
}
void CEquipUI::Render(POINT *pt)
{
	m_bRender=true;
	if(m_bTheFirstTime)
	{
	if(g_PCChar->Def.Adorn!=-1)
	{
		ItemGrid[24].bEmpty=false;
		g_PCChar->CharICS->Add(g_PCChar->Def.Adorn,1,24);
	}
	if(g_PCChar->Def.Armor!=-1)
	{
		ItemGrid[23].bEmpty=false;
		g_PCChar->CharICS->Add(g_PCChar->Def.Armor,1,23);
	}
	if(g_PCChar->Def.Boots!=-1)
	{
		ItemGrid[22].bEmpty=false;
		g_PCChar->CharICS->Add(g_PCChar->Def.Boots,1,22);
	}
	if(g_PCChar->Def.Weapon!=-1)
	{
		ItemGrid[20].bEmpty=false;
		g_PCChar->CharICS->Add(g_PCChar->Def.Weapon,1,20);
	}
	if(g_PCChar->Def.Helmet!=-1)
	{
		ItemGrid[21].bEmpty=false;
		g_PCChar->CharICS->Add(g_PCChar->Def.Helmet,1,21);
	}
	m_bTheFirstTime=false;
	}
	m_Graphics->BeginSprite();
	g_Texture0.Blit(m_iStartX,m_iStartY,250,0,250,390);
	m_Graphics->EndSprite();
	
	//��ȡCharICS(��ɫ�����嵥),��ʾ����ӦGrid
		
	m_iItemCount=g_PCChar->CharICS->GetNumItems();
	//������ÿ֡������ͼ
	m_Graphics->BeginSprite();
	int j=0;
	for(int i=0;i<m_iItemCount;i++)
	{
		if(g_PCChar->CharICS->GetItem(i)->UpDown==0)
		{
		int iPos=(int)g_PCChar->CharICS->GetItem(i)->Position;
		int iItemIDInMIL=(int)g_PCChar->CharICS->GetItem(i)->ItemNum;
		int iCount=(int)g_PCChar->CharICS->GetItem(i)->Quantity;
		if(ItemGrid[iPos].bVisible)
		{
		    UseWhichTex(ItemGrid[iPos].x1,ItemGrid[iPos].y1,atoi(g_MIL[iItemIDInMIL].ImageFilename));		
		    ItemGrid[iPos].bEmpty=false;								
		    ItemGrid[iPos].ID=iItemIDInMIL;
		    ItemGrid[iPos].ICSID=i;
			if(iCount>1)
			{
				//char a[10];
				//sprintf(a,"%ld",iCount);
				stringstream ss;
				ss<<iCount;
				g_Small.Print(ss.str().c_str(),ItemGrid[iPos].x1,ItemGrid[iPos].y1);
			}
		}
		}
	}
	if(pt->x>=(m_iStartX+230)&&pt->x<=(m_iStartX+250)&&pt->y>=m_iStartY&&pt->y<=(m_iStartY+20))
	{
		g_Texture1.Blit(m_iStartX+230,m_iStartY,920,113,20,20);
	}
	else		
		g_Texture1.Blit(m_iStartX+230,m_iStartY,900,113,20,20);	
	g_Texture0.Blit(m_iStartX+130,m_iStartY+363,170,180,70,20);
	if(m_iCurPick!=-1)
	{
		bUseDefaultCursor=false;
		m_iCurHide=(int)g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ICSID)->Position;
		ItemGrid[m_iCurHide].bVisible=false;
		int curPic=(int)g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ICSID)->ItemNum;
		UseWhichTex(pt->x-5,pt->y-5,atoi(g_MIL[curPic].ImageFilename));
	}
	m_Graphics->EndSprite();
	//char Text[10];
	//sprintf(Text,"%ld",g_PCChar->Def.Money);
	stringstream Text;
	Text<<g_PCChar->Def.Money;
	g_Big.Print(Text.str(),m_iStartX+140,m_iStartY+365);
	if(m_iCurPick==-1&&pt->x>(m_iStartX+85)&&
		pt->x<(m_iStartX+120)&&pt->y>(m_iStartY+365)&&pt->y<(m_iStartY+385))
	{
		g_Mid.Print("����",pt->x-20,pt->y-12);
	}
	g_Mid.Print("װ��",m_iStartX+10,m_iStartY);
	
	IfMouseOn(pt);
}
//��MIL�������Ʒ��һ����־��,ȷ��������λ��(30������)if(bool==true){����,����;bool=false;}
//1.�����Ʒ�����λ�ã��ͷ���꣩���Ϸ�,�򷵻���ǰ����λ��
//2.�����Ʒ�����λ�úϷ�,��λ��Ϊ��,��ֱ�ӷ���,��ǰһλ��Ϊ��,��λ��Ϊ��,��¼��Ʒ��λ��,д��MIL,bool=true,�������
//3.�����Ʒ�����λ�úϷ�,��λ��Ϊ��,�򽻻�����λ��,д��CharICS,bool=true,�������
//��ע:������д����ļ��Ķ�д,ֻ�����ڴ��н��д���,����m_bUpdateʵ���Ƕ����
void CEquipUI::IfMouseOn(POINT *pt)//��ʾ������
{
	int j;
	m_Graphics->BeginSprite();
	if((j=GetMouseOnWhichGrid(pt))!=-1)
	{
		//����Ʒ�ſɵ�ȡ
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
			//ֻ��û�е��߱�ѡ�м����߿ɼ�ʱ��ʾ����
			if(ItemGrid[j].bVisible&&m_iCurPick==-1)
			{
				long Value;
				int n=0,m=0;
				if(g_MIL[ItemGrid[j].ID].ModifyDef!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyDex!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyEne!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyHit!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyHP!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyMP!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyRes!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyStr!=0)
					m++;
				g_Texture0.Blit(pt->x-170,pt->y-100,540,765,230,16*m+100);
				m_Graphics->EndSprite();
				g_Mid.Print(g_MIL[ItemGrid[j].ID].Name,pt->x-150,pt->y-90);
				if((Value=g_MIL[ItemGrid[j].ID].ModifyHP)!=0)
				{
					stringstream Text;
					//sprintf(Text,"����+ %ld",Value);
					Text<<"����+ "<<Value;
					g_Mid.Print(Text.str(),pt->x-150,pt->y-70+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyMP)!=0)
				{
					stringstream Text;
					//sprintf(Text,"����+ %ld",Value);
					Text<<"����+ "<<Value;
					g_Mid.Print(Text.str(),pt->x-150,pt->y-70+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyStr)!=0)
				{
					stringstream Text;
					//sprintf(Text,"����+ %ld",Value);
					Text<<"����+ "<<Value;
					g_Mid.Print(Text.str(),pt->x-150,pt->y-70+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyDef)!=0)
				{
					stringstream Text;
					//sprintf(Text,"����+ %ld",Value);
					Text<<"����+ "<<Value;
					g_Mid.Print(Text.str(),pt->x-150,pt->y-70+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyDex)!=0)
				{
					stringstream Text;
					//sprintf(Text,"��+ %ld",Value);
					Text<<"��+ "<<Value;
					g_Mid.Print(Text.str(),pt->x-150,pt->y-70+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyEne)!=0)
				{
					stringstream Text;
					//sprintf(Text,"����+ %ld",Value);
					Text<<"����+ "<<Value;
					g_Mid.Print(Text.str(),pt->x-150,pt->y-70+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyRes)!=0)
				{
					stringstream Text;
					//sprintf(Text,"����+ %ld",Value);
					Text<<"����+ "<<Value;
					g_Mid.Print(Text.str(),pt->x-150,pt->y-70+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyHit)!=0)
				{
					stringstream Text;
					//sprintf(Text,"����+ %ld",Value);
					Text<<"����+ "<<Value;
					g_Mid.Print(Text.str(),pt->x-150,pt->y-70+16*(n++));
				}				
				if(m_App->GetBarterUI().GridBasedUIClosed())
				{
					stringstream Text;
					if(CheckItemFlag(g_MIL[ItemGrid[j].ID].Flags,SELLABLE))
					{
					    Value=g_MIL[ItemGrid[j].ID].Price/2;				
					    //sprintf(Text,"���� ��ֵ: %ld",Value);
						Text<<"���� ��ֵ: "<<Value;
					    g_Mid.Print(Text.str(),pt->x-150,pt->y-70+16*(n++));
					}
					else
					{											
						g_Mid.Print("������",pt->x-150,pt->y-70+16*(n++));
					}
				}
				g_Mid.Print(g_MIL[ItemGrid[j].ID].Description,pt->x-150,pt->y-65+16*(n++),\
					200,48,0xFFFFFFFF,DT_WORDBREAK);
			}
			//����ѡ,һֱΪ��,ֱ�����������ͷ�
			/*if(m_Mouse->GetButtonState(0)==TRUE&&m_bFree&&m_App->GetQuickUI().GetCurrentPick()==-1)//�����ѡ�ƶ�,��װ��
			{
				m_iCurPick=j;
				m_bFree=false;	
			}*/	
			//��굥��һ��ѡ�����
			if(m_Mouse->GetButtonState(0)&&m_iCurPick==-1&&m_App->GetQuickUI().GetCurrentPick()==-1)
			{
				m_iCurPick=j;
				m_Mouse->SetButtonState(0,FALSE);
				m_Mouse->SetLock(0,1);
			}
            /********************************************************************************************/
			m_Graphics->BeginSprite();
			if(m_Mouse->GetButtonState(1)==TRUE)		
			{
				m_iCurUse=j;
				m_Mouse->SetLock(1,TRUE);
				long value;			
				if(g_MIL[ItemGrid[m_iCurUse].ID].Category==FOOD||g_MIL[ItemGrid[m_iCurUse].ID].Category==HEAL)			
				{
					m_App->PlaySound(12);
					//�˴�ֻ������FOOD��HEALΪ��������HP��MP				
					//��Equip��Ϊ�����������ԵĻ�ֵ				
					//����һ����������ƷΪ���ӽ�ɫ��ֵ(��Collect��Throw��GutUse)				
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyHP)!=0)			
					{					
						g_PCChar->HealthPoints+=value;					
						if(g_PCChar->HealthPoints>m_App->GetCharController()->GetHP(g_PCChar))					
							g_PCChar->HealthPoints=m_App->GetCharController()->GetHP(g_PCChar);				
					}			
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyMP)!=0)				
					{					
						g_PCChar->ManaPoints+=value;						
						if(g_PCChar->ManaPoints>m_App->GetCharController()->GetMP(g_PCChar))						
							g_PCChar->ManaPoints=m_App->GetCharController()->GetMP(g_PCChar);				
					}				
					sCharItem *ItemPtr;				
					ItemPtr=g_PCChar->CharICS->GetItem(ItemGrid[m_iCurUse].ICSID);				
					if(CheckItemFlag(g_MIL[ItemPtr->ItemNum].Flags,USEONCE))			
					{			
						if(ItemPtr->Quantity==1)
						{
							g_PCChar->CharICS->Remove(ItemPtr);	
							ItemGrid[m_iCurUse].bEmpty=true;
						}
						else				
						{					
							ItemPtr->Quantity--;				
						}			
					}		
				}					
				//�����COLLECT�һ�ʹ��,�����ӽ�ɫ�����ֵ			
				else if(g_MIL[ItemGrid[m_iCurUse].ID].Category==COLLECT)			
				{	
					m_App->PlaySound(12);		
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyHP)!=0)					
						g_PCChar->Def.HealthPoints+=value;			
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyMP)!=0)		
						g_PCChar->Def.ManaPoints+=value;				
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyStr)!=0)					
						g_PCChar->Def.STR+=value;				
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyDef)!=0)					
						g_PCChar->Def.DEF+=value;			
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyDex)!=0)				
						g_PCChar->Def.DEX+=value;			
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyEne)!=0)				
						g_PCChar->Def.ENE+=value;				
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyRes)!=0)					
						g_PCChar->Def.RES+=value;					
					if((value=g_MIL[ItemGrid[m_iCurUse].ID].ModifyHit)!=0)						
						g_PCChar->Def.HIT+=value;					
					sCharItem *ItemPtr;					
					ItemPtr=g_PCChar->CharICS->GetItem(ItemGrid[m_iCurUse].ICSID);			
					if(CheckItemFlag(g_MIL[ItemGrid[m_iCurUse].ID].Flags,USEONCE))				
					{				
						if(ItemPtr->Quantity==1)
						{
							g_PCChar->CharICS->Remove(ItemPtr);	
							ItemGrid[m_iCurUse].bEmpty=true;
						}
						else				
						{					
							ItemPtr->Quantity--;				
						}		
					}			
				}										
			}
            /************************************************************************************/
        }
		else//bEmpty=true;
		{
			bUseDefaultCursor=true;
			if(m_iCurPick!=-1)
			{
				bUseDefaultCursor=false;
				g_Hand.Blit(pt->x,pt->y);
			}
		}
		//������Ʒ�����κο�����ı߽���
		//if(m_Mouse->GetButtonState(0)==false&&m_bFree==false)
		//�ٵ���һ�η���
		if(m_Mouse->GetButtonState(0)&&m_iCurPick!=-1&&m_App->GetQuickUI().GetCurrentPick()==-1)
		{	
			m_Mouse->SetButtonState(0,FALSE);
			m_Mouse->SetLock(0,1);
			//..���н����ȴ���								
			int ID=ItemGrid[m_iCurPick].ID;//�˴�IDΪ��ǰѡ�ĸ��Ӧ����Ʒ(MIL(�ܵ����б�)���)	
			int ICSID=ItemGrid[m_iCurPick].ICSID;//�˴�IDΪ��ǰѡ�ĸ��Ӧ����Ʒ(ICS���)
			//���������,ֱ�ӷ���				
			if(ItemGrid[j].bEmpty)				
			{
				//�����Ҫ����Ŀ��Ƿ�Ϊ�޶��Ŀ��䡢�����ס�Ь��
				if(j>19&&j<30)
				{
					switch(j)
					{
					case 20:
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=WEAPON)//��
						{
							m_iCurPick=-1;
							m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;								
							return;
						}
						else
						{
						    m_App->GetCharController()->Equip(g_PCChar,ID,WEAPON,true);
							m_App->PlaySound(10);
						}
						break;
					case 23:
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=ARMOR)//��
						{
							m_iCurPick=-1;
							m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;
							return;
						}
						else
						{
							m_App->PlaySound(10);
							m_App->GetCharController()->Equip(g_PCChar,ID,ARMOR,true);
						}
						break;
					case 21://��
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=HELMET)
						{
							m_iCurPick=-1;
							m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;
							return;
						}
						else
						{
							m_App->PlaySound(10);
							m_App->GetCharController()->Equip(g_PCChar,ID,HELMET,true);
						}
						break;
					case 22://Ь
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=BOOTS)
						{
							m_iCurPick=-1;
						    m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;
						    return;
						}
						else
						{
							m_App->PlaySound(10);
							m_App->GetCharController()->Equip(g_PCChar,ID,BOOTS,true);
						}
						break;
					case 24:
					case 25:
					case 26:
					case 27:
					case 28:
					case 29:
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=ADORN)
						{						
							m_iCurPick=-1;
						    m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;
							return;
						}
						else
						{
							m_App->PlaySound(10);
						}
						break;		
					}
				}
				ItemGrid[m_iCurPick].bEmpty=true;//��ԭ��Ϊ��
				ItemGrid[m_iCurHide].bVisible=true;
				//жװ������,ע��m_iCurPick=20,�Ǵ���������
				if(g_MIL[ItemGrid[m_iCurPick].ID].Category==WEAPON&&j>=0&&j<20&&m_iCurPick==20)
				{
					m_App->GetCharController()->Equip(g_PCChar,ID,WEAPON,false);
					m_App->PlaySound(11);
					//bEquip=false;
				}
				else if(g_MIL[ItemGrid[m_iCurPick].ID].Category==ARMOR&&j>=0&&j<20&&m_iCurPick==23)
				{
					m_App->GetCharController()->Equip(g_PCChar,ID,ARMOR,false);
					m_App->PlaySound(11);
				}
				else if(g_MIL[ItemGrid[m_iCurPick].ID].Category==HELMET&&j>=0&&j<20&&m_iCurPick==21)
				{
					m_App->GetCharController()->Equip(g_PCChar,ID,HELMET,false);
					m_App->PlaySound(11);
				}
				else if(g_MIL[ItemGrid[m_iCurPick].ID].Category==BOOTS&&j>=0&&j<20&&m_iCurPick==22)
				{
					m_App->GetCharController()->Equip(g_PCChar,ID,BOOTS,false);
					m_App->PlaySound(11);
				}
				m_iCurPick=-1;
				m_bFree=true;	
				g_PCChar->CharICS->GetItem(ICSID)->Position=(long)j;
			}		
			else//�ǿ�,����
			{
			    //һ���(������)���߻��ཻ��û������,�ɵ�������װ����û������,
				//��װ������װ����Ҳû������
				//����װ�������������Ľ���Ҫ����Դ�,Ҫ�ж����ߵ�����Ƿ���ͬ
			    if(m_iCurPick>19&&m_iCurPick<30&&j>-1&&j<20)//��װ������������
				{
				    if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=g_MIL[ItemGrid[j].ID].Category)
					{
						m_iCurPick=-1;
						m_bFree=true;
						ItemGrid[m_iCurHide].bVisible=true;
					    return;
					}
				}
				//�����Ҫ����Ŀ��Ƿ�Ϊ�޶��Ŀ��䡢�����ס�Ь��
				if(j>19&&j<30)
				{
					switch(j)
					{
					case 20:
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=WEAPON)//ʰȡ����
						{
							m_iCurPick=-1;
							m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;
							return;
						}
						else//ж��ԭ��װ��,������װ��
						{
							m_App->GetCharController()->Equip(g_PCChar,ID,WEAPON,true);
							m_App->PlaySound(10);
							//m_App->PlaySound(11);
						}
						break;
					case 23:
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=ARMOR)//��
						{
							m_iCurPick=-1;
							m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;
							return;
						}
						else
						{
							m_App->PlaySound(10);
							m_App->GetCharController()->Equip(g_PCChar,ID,ARMOR,true);
							//m_App->PlaySound(11);
						}
						break;
					case 21://��
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=HELMET)
						{
							m_iCurPick=-1;
							m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;
							return;
						}
						else
						{
							m_App->PlaySound(10);
							m_App->GetCharController()->Equip(g_PCChar,ID,HELMET,true);
							//m_App->PlaySound(11);
						}
						break;
					case 22://Ь
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=BOOTS)
						{
							m_iCurPick=-1;
							m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;
							return;
						}
						else
						{
							m_App->PlaySound(10);
							m_App->GetCharController()->Equip(g_PCChar,ID,BOOTS,true);
							//m_App->PlaySound(11);
						}
						break;
					case 24:
					case 25:
					case 26:
					case 27:
					case 28:
					case 29:
						if(g_MIL[ItemGrid[m_iCurPick].ID].Category!=ADORN)
						{
							m_iCurPick=-1;
							m_bFree=true;
							ItemGrid[m_iCurHide].bVisible=true;
							return;
						}
						else
						{
							m_App->PlaySound(10);
							//m_App->PlaySound(11);
						}
						break;
					}
				}
				//�������Ϸŵ�һ���յ�λ��,�Ƚϼ�,ֻ�迼�Ǵ���(װ������ж)�Ϻʹ���
				//(��������װ)�������,�����ϵ÷�Ϊ�������ºʹ�������,����������װ��
				//����������ʲôҲ����
				//������Ʒ����Ҫ����һЩ,��������ҲҪ����
				//���������Ϸŵ�һ�����յ�λ��
				//�˴��Ǵ���������,����ж,��Ҫװ��jλ�õ�����
				if(g_MIL[ItemGrid[m_iCurPick].ID].Category==WEAPON&&j>=0&&j<20&&m_iCurPick==20)
				{
					m_App->GetCharController()->Equip(g_PCChar,ItemGrid[j].ID,WEAPON,true);
					m_App->PlaySound(10);
				}
				else if(g_MIL[ItemGrid[m_iCurPick].ID].Category==ARMOR&&j>=0&&j<20&&m_iCurPick==23)
				{
					m_App->GetCharController()->Equip(g_PCChar,ID,ARMOR,true);
					m_App->PlaySound(10);
				}
				else if(g_MIL[ItemGrid[m_iCurPick].ID].Category==HELMET&&j>=0&&j<20&&m_iCurPick==21)
				{
					m_App->GetCharController()->Equip(g_PCChar,ID,HELMET,true);
					m_App->PlaySound(10);
				}
				else if(g_MIL[ItemGrid[m_iCurPick].ID].Category==BOOTS&&j>=0&&j<20&&m_iCurPick==22)
				{
					m_App->GetCharController()->Equip(g_PCChar,ID,BOOTS,true);
					m_App->PlaySound(10);
				}
				if((g_MIL[ItemGrid[m_iCurPick].ID].Category==FOOD||g_MIL[ItemGrid[m_iCurPick].ID].Category==HEAL)
					&&(ItemGrid[m_iCurPick].ID==ItemGrid[j].ID)&&m_iCurPick!=j)
				{
					g_PCChar->CharICS->GetItem(ItemGrid[j].ICSID)->Quantity+=g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ICSID)->Quantity;						
					g_PCChar->CharICS->Remove(g_PCChar->CharICS->GetItem(ICSID));
					ItemGrid[m_iCurPick].bEmpty=true;
				}
				else
				{
					g_PCChar->CharICS->GetItem(ICSID)->Position=(long)j;
					g_PCChar->CharICS->GetItem(ItemGrid[j].ICSID)->Position=(long)m_iCurPick;
				}
				m_bFree=true;
				ItemGrid[m_iCurHide].bVisible=true;
				m_iCurPick=-1;
			}
		}													
	}
	else
	{
		//bUseDefaultCursor=true;
		sCharacter *NPCPointTo=m_App->GetCharacterAt(pt->x,pt->y);
		if(NPCPointTo==NULL)
		{	 
		    bUseDefaultCursor=true;
		}  
	    else 
		{
		    if(m_App->GetBarterUI().GridBasedUIClosed()==false)
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
			}//������׽����,���ܽ��жԻ�
		}
		if(m_iCurPick!=-1)
		{
			bUseDefaultCursor=false;
			g_Hand.Blit(pt->x,pt->y);
		}
	}	
	//������Ʒ�����κο�����ı߽���
	if(m_Mouse->GetButtonState(0)&&m_iCurPick!=-1&&m_App->GetQuickUI().GetCurrentPick()==-1)
	//if(m_Mouse->GetButtonState(0)==false&&m_bFree==false)
	{
		m_Mouse->SetButtonState(0,FALSE);
		m_Mouse->SetLock(0,1);
		j=m_App->GetQuickUI().GetMouseOnWhichGrid(pt);
		if(j>=7&&j<=9&&m_iCurPick!=-1&&(g_MIL[ItemGrid[m_iCurPick].ID].Category==FOOD\
			||g_MIL[ItemGrid[m_iCurPick].ID].Category==HEAL||g_MIL[ItemGrid[m_iCurPick].ID].Category==COLLECT))
		{
			if(m_App->GetQuickUI().ItemGrid[j].bEmpty)
			{
				g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ICSID)->Position=j;
				g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ICSID)->UpDown=1;
				ItemGrid[m_iCurPick].bEmpty=true;
			}
			else
			{
				g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ICSID)->Position=j;
				g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ICSID)->UpDown=1;
				g_PCChar->CharICS->GetItem(m_App->GetQuickUI().ItemGrid[j].ID1)->Position=m_iCurPick;
				g_PCChar->CharICS->GetItem(m_App->GetQuickUI().ItemGrid[j].ID1)->UpDown=0;
			}
		}
        /**************************************************************************************/
		int bX,bY;
		bX=m_App->GetBarterUI().GetStartX();
		bY=m_App->GetBarterUI().GetStartY();
		if(pt->x>bX&&pt->x<bX+250&&pt->y>bY&&pt->y<bY+390&&\
			m_App->GetBarterUI().GridBasedUIClosed())//���׽���򿪵�����½���Ʒ�������� ��
		{
			sCharItem *ScanPtr;
			ScanPtr=g_PCChar->CharICS->GetItem(ItemGrid[m_iCurPick].ICSID);				
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
        /*****************************************************************************/
		m_iCurPick=-1;
		m_bFree=true;
		ItemGrid[m_iCurHide].bVisible=true;												
	}//����Ʒ�����κο�����ı߽���	
	m_Graphics->EndSprite();
}

//�õ����ָ��λ�ںδ�,���ط�����
int CEquipUI::GetMouseOnWhichGrid(POINT *pt)
{
	if(InRect((11+m_iStartX),(72+m_iStartY),pt))
		return 20;
	else if(InRect((55+m_iStartX),(27+m_iStartY),pt))
		return 21;
	else if(InRect((55+m_iStartX),(115+m_iStartY),pt))
		return 22;
	else if(InRect((100+m_iStartX),(72+m_iStartY),pt))
		return 23;//��Ʒ�Ȳ�����
	else if(InRect((145+m_iStartX),(26+m_iStartY),pt))
		return 24;
	else if(InRect((189+m_iStartX),(26+m_iStartY),pt))
		return 25;
	else if(InRect((145+m_iStartX),(72+m_iStartY),pt))
		return 26;
	else if(InRect((189+m_iStartX),(72+m_iStartY),pt))
		return 27;
	else if(InRect((145+m_iStartX),(116+m_iStartY),pt))
		return 28;
	else if(InRect((189+m_iStartX),(116+m_iStartY),pt))
		return 29;
	else
	{
		for(int i=0;i<4;i++)
		{
			for(int j=0;j<5;j++)
			{
				if(InRect(((40+5)*j+10+m_iStartX),((40+5)*i+180+m_iStartY),pt))
					return (i*5+j);
			}
		}
	}
	return -1;//û���Ƶ��κο�
}

void CEquipUI::CloseEquipUI()
{
	m_bRender=false;
	for(int i=0;i<30;i++)
		ItemGrid[i].bEmpty=true;
}

void CEquipUI::SetItemGrid(int ID)
{
	ItemGrid[ID].bEmpty=false;
}