/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#include "BarterUI.h"
#include "Global.h"
#include <sstream>
using namespace std;

CBarterUI::CBarterUI()
{
	m_iStartX=10;
	m_iStartY=60;
	m_iUIWidth=250;
	m_iUIHeight=390;
	m_bRender=false;
}
CBarterUI::~CBarterUI()
{
	m_bRender=false;
}
void CBarterUI::Init(CGraphics *Graphics,CInputDevice *Mouse)
{
	m_Graphics=Graphics;
	m_Mouse=Mouse;
	for(int i=0;i<30;i++)
	{
		ItemGrid[i].x1=m_iStartX+15+45*(i%5);
		ItemGrid[i].y1=m_iStartY+60+45*(i/5);
	}
	m_iCurPick=-1;		
}

int CBarterUI::FindTheFirstEmptyGrid()
{
	for(int i=0;i<30;i++)
	{
		if(ItemGrid[i].bEmpty==true)		
			return i;
	}
	return -1;
}
void CBarterUI::Render(POINT *pt,CCharICS *ICS)
{
	m_bRender=true;
	m_Graphics->BeginSprite();
	g_Texture0.Blit(m_iStartX,m_iStartY,504,0,250,390);	
	
	//读取ShopICS(商店道具清单),显示到对应Grid
	m_iItemCount=ICS->GetNumItems();
	//必须在每帧进行贴图
	int j=0;
	for(int i=0;i<m_iItemCount;i++)
	{	
		int iPos=(int)ICS->GetItem(i)->Position;
		int iItemIDInMIL=(int)ICS->GetItem(i)->ItemNum;
		int iCount=(int)ICS->GetItem(i)->Quantity;
		if(ItemGrid[iPos].bVisible)
		{
		    UseWhichTex(ItemGrid[iPos].x1,ItemGrid[iPos].y1,atoi(g_MIL[iItemIDInMIL].ImageFilename));		
		    ItemGrid[iPos].bEmpty=false;								
		    ItemGrid[iPos].ID=iItemIDInMIL;
		    ItemGrid[iPos].ICSID=i;
		}
	}
	m_Graphics->EndSprite();	
	IfMouseOn(pt,ICS);
	m_Graphics->BeginSprite();
	if(m_iCurPick!=-1)
	{
		m_iCurHide=(int)ICS->GetItem(ItemGrid[m_iCurPick].ICSID)->Position;
		ItemGrid[m_iCurHide].bVisible=false;
		int curPic=(int)ICS->GetItem(ItemGrid[m_iCurPick].ICSID)->ItemNum;
		UseWhichTex(pt->x-5,pt->y-5,atoi(g_MIL[curPic].ImageFilename));
	}
	if(pt->x>=(m_iStartX+230)&&pt->x<=(m_iStartX+250)&&pt->y>=m_iStartY&&pt->y<=(m_iStartY+20))
	{
		g_Texture1.Blit(m_iStartX+230,m_iStartY,920,113,20,20);
	}
	else		
		g_Texture1.Blit(m_iStartX+230,m_iStartY,900,113,20,20);/**/
	m_Graphics->EndSprite();
	//char Text[50];
	//sprintf(Text,"%s的杂货铺",g_BarterChar->Def.Name);
	//g_Mid.Print(Text,m_iStartX+10,m_iStartY);
	stringstream strBarterName;
	strBarterName<<g_BarterChar->Def.Name<<"的杂货铺";
	g_Mid.Print(strBarterName.str(),m_iStartX+10,m_iStartY);
}
void CBarterUI::IfMouseOn(POINT *pt,CCharICS *ICS)//显示其描述
{	
	int j;		
	if((j=GetMouseOnWhichGrid(pt,m_iStartX,m_iStartY))!=-1)		
	{			
		//有物品才可点取			
		if(ItemGrid[j].bEmpty==false)			
		{
				
			//只有没有道具被选中及道具可见时显示描述
			if(ItemGrid[j].bVisible&&m_iCurPick==-1)
			{
				long Value;
				int n=0,m=0;
				if(g_MIL[ItemGrid[j].ID].ModifyHP!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyMP!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyStr!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyDef!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyDex!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyEne!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyHit!=0)
					m++;
				if(g_MIL[ItemGrid[j].ID].ModifyRes!=0)
					m++;
				m_Graphics->BeginSprite();
				g_Texture0.Blit(pt->x-10,pt->y-30,540,765,100,40+16*m);
				m_Graphics->EndSprite();				
				g_Mid.Print(g_MIL[ItemGrid[j].ID].Name,pt->x+16,pt->y-24);
				if((Value=g_MIL[ItemGrid[j].ID].ModifyHP)!=0)
				{
					stringstream Text;
					//sprintf(Text,"生命+ %ld",Value);
					Text<<"生命+ "<<Value;
					g_Mid.Print(Text.str(),pt->x+16,pt->y+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyMP)!=0)
				{
					stringstream Text;
					//sprintf(Text,"真气+ %ld",Value);
					Text<<"真气+ "<<Value;
					g_Mid.Print(Text.str(),pt->x+16,pt->y+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyStr)!=0)
				{
					stringstream Text;
					//sprintf(Text,"武力+ %ld",Value);
					Text<<"武力+ "<<Value;
					g_Mid.Print(Text.str(),pt->x+16,pt->y+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyDef)!=0)
				{
					stringstream Text;
					//sprintf(Text,"防御+ %ld",Value);
					Text<<"防御+ "<<Value;
					g_Mid.Print(Text.str(),pt->x+16,pt->y+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyDex)!=0)
				{
					stringstream Text;
					//sprintf(Text,"身法+ %ld",Value);
					Text<<"身法+ "<<Value;
					g_Mid.Print(Text.str(),pt->x+16,pt->y+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyEne)!=0)
				{
					stringstream Text;
					//sprintf(Text,"内力+ %ld",Value);
					Text<<"内力+ "<<Value;
					g_Mid.Print(Text.str(),pt->x+16,pt->y+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyRes)!=0)
				{
					stringstream Text;
					//sprintf(Text,"气御+ %ld",Value);
					Text<<"气御+ "<<Value;
					g_Mid.Print(Text.str(),pt->x+16,pt->y+16*(n++));
				}
				if((Value=g_MIL[ItemGrid[j].ID].ModifyHit)!=0)
				{
					stringstream Text;
					//sprintf(Text,"命中+ %ld",Value);
					Text<<"命中+ "<<Value;
					g_Mid.Print(Text.str(),pt->x+16,pt->y+16*(n++));
				}
				g_Mid.Print(g_MIL[ItemGrid[j].ID].Description,m_iStartX+15,m_iStartY+340,\
					220, 64, 0xFFFFFFFF, DT_WORDBREAK);
				//sprintf(Text,"价值%ld金",g_MIL[ItemGrid[j].ID].Price);
				{
					stringstream Text;
					Text << "价值+ " << g_MIL[ItemGrid[j].ID].Price << "金";
					g_Mid.Print(Text.str(), m_iStartX + 15, m_iStartY + 372);
				}
				//sprintf(Text,"现有%ld金",g_PCChar->Def.Money);
				{
					stringstream Text;
					Text << "现有+ " << g_PCChar->Def.Money << "金";
					g_Mid.Print(Text.str(), m_iStartX + 100, m_iStartY + 372);
				}
			}
			//右键单击购买
			int iFirst=0;//一定要赋初值，否则会有运行时错误
			if(m_Mouse->GetButtonState(1)==TRUE)
			{
				m_Mouse->SetLock(1,TRUE);
				sCharItem  *ScanPtr;										
				if(g_PCChar->Def.Money>=g_MIL[ItemGrid[j].ID].Price)
				{						
					ScanPtr=g_PCChar->CharICS->GetParentItem();
					while(ScanPtr!=NULL)
					{
						if(ScanPtr->ItemNum==ItemGrid[j].ID)//!此处
						{
							//如果一个物品对应一个格,则CharICS中存放的物品的数量都该为一
			                //如果n个物品可以放在一个格内,则只需Quantity++
			                //可以考虑,对于消耗品,如FOOD、HEAL一个格可以放n个(如99)
			                //对于装备等,一个格只能放一个
							if(g_MIL[ScanPtr->ItemNum].Category==FOOD||g_MIL[ScanPtr->ItemNum].Category==HEAL)	          				                
								ScanPtr->Quantity++;
			                else
						    {
								//必须在此将装备界面当前占用格置位满状态 FindTheFirstEmptyGrid之前
								//m_App->GetEquipUI().ItemGrid[iFirst].bEmpty=false;
								//如果EquipUI处于关闭状态的话，有必要根据CharICS计算出
								//第一个空的方格
								if(m_App->GetEquipUI().GridBasedUIClosed()==false)
								{
									//如果道具栏中有此物品，没有问题
									//如果无，则第一次iFirst为0 ?
									iFirst=FindTheFirstEmptyGridOfEquipUI();									
								}
								else
								{																		
									iFirst=m_App->GetEquipUI().FindTheFirstEmptyGrid();
									//m_App->GetEquipUI().ItemGrid[iFirst].bEmpty=false;
								}
							    //m_App->GetEquipUI().SetItemGrid(iFirst);
				                if(iFirst!=-1)		
					                g_PCChar->CharICS->Add(ItemGrid[j].ID, 1, iFirst);								    									
							}
							break;
						}
						ScanPtr=ScanPtr->Next;
					}
					//如果现有的物品清单中没有当前买的物品
					if(ScanPtr == NULL)
					{						
						if(m_App->GetEquipUI().GridBasedUIClosed()==false)							
						{								
							iFirst=FindTheFirstEmptyGridOfEquipUI();
						}
						else
						{							
			                iFirst=m_App->GetEquipUI().FindTheFirstEmptyGrid();
						}
			            if(iFirst!=-1)
				            g_PCChar->CharICS->Add(ItemGrid[j].ID, 1, iFirst,NULL);	  
					}	      		                                      		               
					if(iFirst!=-1)					   
					{			               
					    g_PCChar->Def.Money -=g_MIL[ItemGrid[j].ID].Price;
                        m_App->PlaySound(15);
					}
	                else
					{
					}                      										
				}								
			}
        }
    }			
}
//得到鼠标指针位于何处,返回方格编号
int CBarterUI::GetMouseOnWhichGrid(POINT *pt,int m_iStartX,int m_iStartY)
{	
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<5;j++)
		{
			if(InRect(((45)*j+15+m_iStartX),((45)*i+60+m_iStartY),pt))
				return (i*5+j);
		}
	}
	return -1;//没有移到任何框
}

void CBarterUI::CloseBarterUI()
{
	m_bRender=false;
	for(int i=0;i<30;i++)
		ItemGrid[i].bEmpty=true;
}

int CBarterUI::FindTheFirstEmptyGridOfEquipUI()
{	
	sCharItem *ItemPtr=g_PCChar->CharICS->GetParentItem();
	int Pos[100];
	int Num=0;
	//遍历角色物品清单,对位于道具栏纪录在数组Pos中,Num即道具栏被占用的总数
	while(ItemPtr!=NULL)
	{
		int pos=ItemPtr->Position;
		if(ItemPtr->UpDown==0&&pos>=0&&pos<20)
		{
			Pos[Num]=pos;
			Num++;
		}
		ItemPtr=ItemPtr->Next;
	}		
	int k;
	for(int m=0;m<20;m++)
	{
		k=0;
		for(int n=0;n<Num;n++)
		{
			if(m==Pos[n])
				break;
			else
				k++;
		}
		if(k==Num)			
			return m;
	}
	return -1;
}