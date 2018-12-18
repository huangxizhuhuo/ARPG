/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#include "GridBasedUI.h"
#include "Global.h"

CGridBasedUI::CGridBasedUI()
{
}

CGridBasedUI::~CGridBasedUI()
{
}

void CGridBasedUI::SetData(CApp *App)
{
	m_App=App;
}

//只在NewGame时有效
void CGridBasedUI::SetFirstTime()
{
	m_bTheFirstTime=true;
}

bool CGridBasedUI::InRect(int x,int y,POINT *pt)
{
	if(pt->x>=x&&pt->x<=(x+40)&&pt->y>=y&&pt->y<=(y+40))
		return true;
	return false;
}

void CGridBasedUI::UseWhichTex(int DestX,int DestY,int number)
{
	if(number<=100)//100以内为装备
	{
		int i=number/6;
		int j=number%6;
		if(i>5||j>5)
			return;
		else
		{
			g_Tex1.Blit(DestX,DestY,j*40,i*40,40,40);
		}
	}
	else if(number<=200)//100-200为道具
	{
		int i=(number-100)/6;
		int j=(number-100)%6;
		if(i>5||j>5)
			return;
		else
		{
			g_Tex2.Blit(DestX,DestY,j*40,i*40,40,40);
		}
	}
	else if(number<=300)//100-200为特殊
	{
		int i=(number-200)/6;
		int j=(number-200)%6;
		if(i>5||j>5)
			return;
		else
		{
			g_Tex3.Blit(DestX,DestY,j*40,i*40,40,40);
		}
	}
	else if(number<=400)
	{
		int i=(number-300)/6;
		int j=(number-300)%6;
		if(i>5||j>5)
			return;
		else
		{
			g_Tex4.Blit(DestX,DestY,j*40,i*40,40,40);
		}
	}
}

int CGridBasedUI::GetCurrentPick()
{
	return m_iCurPick;
}

bool CGridBasedUI::GridBasedUIClosed()
{
	return m_bRender;
}

int CGridBasedUI::GetStartX()
{
	return m_iStartX;
}

int CGridBasedUI::GetStartY()
{
	return m_iStartY;
}

bool CGridBasedUI::MouseStopOnGridBasedUI(CInputDevice *m_Mouse)
{
	long mouseX=m_Mouse->GetXPos();
	long mouseY=m_Mouse->GetYPos();
	if(mouseX>=m_iStartX&&mouseX<=m_iStartX+m_iUIWidth&&mouseY>=m_iStartY&&
		mouseY<=m_iStartY+m_iUIHeight&&m_bRender)
		return TRUE;
	return FALSE;
}
