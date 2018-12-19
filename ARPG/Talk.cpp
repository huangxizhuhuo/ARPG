/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
//#include "../Core/Core_Global.h"
#include "Talk.h"
#include "Global.h"
char *g_TalkCursor[]=
{
	{"..\\Image\\cursor\\press.png"},
	{"..\\Image\\cursor\\press1.png"}
};

CTalk::CTalk()
{
	m_Graphics=NULL;
	m_Font=NULL;
	m_Text=NULL;
	m_TextColor=0xFFFFFFFF;
	PrintLength=0;
	bPrintAll=false;
	UpdateTimer=0;
	PrintSpeed=15;

	bTalking = false;
}

CTalk::~CTalk()
{
	Free();
}

bool CTalk::Create(CGraphics *Graphics,CGameFont *Font,CTexture *Texture,CInputDevice *Mouse)
{
	Free();
	if((m_Graphics=Graphics)==NULL||(m_Font=Font)==NULL||(m_Texture=Texture)==NULL||(m_Mouse=Mouse)==NULL)
		return false;
	return true;
}

bool CTalk::Free()
{
	delete []m_Text;
	m_Text=NULL;
	return true;
}

bool CTalk::SetText(char *Text,D3DCOLOR TextColor)
{
	delete []m_Text;
	m_Text=NULL;
	m_Text=strdup(Text);
	m_TextColor=TextColor;
	Length=(int)strlen(Text);
	bPrintAll=false;
	PrintLength=0;
	PrintSpeed=15;
	UpdateTimer=0;
	return true;
}

bool CTalk::Render(char *Text,D3DCOLOR Color,int Type)
{
	if(m_Graphics==NULL||m_Font==NULL)
		return false;
	m_Graphics->BeginSprite();
	m_Texture->Blit(0,482);
	m_Graphics->EndSprite();
	if(timeGetTime()>=UpdateTimer+100)
	{
		if(bPrintAll==false)
		{
			PrintLength+=PrintSpeed;
		}
		UpdateTimer=timeGetTime();
	}
	if(PrintLength>=Length)
	{
		bPrintAll=true;
		bTalking = false;
	}
	strcpy(Temp,m_Text);
	Temp[PrintLength]=0;
	if(Type==0)
		m_Font->Print(Temp,120,535,600,88,m_TextColor, DT_WORDBREAK);
	else
		m_Font->Print(Temp,60,535,600,88,m_TextColor,DT_WORDBREAK);
	{
		static CTexture m_InTalkCursor;
		static int SumFrame=0,iNum=0;
		if(SumFrame%20==0)
			iNum++;
		if(iNum>1)
			iNum=0;
		SumFrame++;
		m_InTalkCursor.Load(m_Graphics,g_TalkCursor[iNum]);
		SumFrame++;
		m_Graphics->BeginSprite();
		m_InTalkCursor.Blit(m_Mouse->GetXPos(),m_Mouse->GetYPos());
		m_Graphics->EndSprite();
		m_InTalkCursor.Free();
	}
	return true;
}