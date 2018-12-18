/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#ifndef GRIDBASEDUI_H
#define GRIDBASEDUI_H
#include "../Core/Core_Global.h"

class CApp;
class CGraphics;
class CInputDevice;

class CGridBasedUI
{
protected:
	int           m_iStartX,m_iStartY;//界面左上角位于屏幕的坐标	
	int           m_iUIWidth,m_iUIHeight;
	CGraphics    *m_Graphics;
	CInputDevice *m_Mouse;
	int           m_iSkillCount;
	int           m_iItemCount;
	int           m_iCurPick;//当前选择的图元对应之位置
	bool          m_bFree;
	bool          m_bUp;
	int           m_iCurUse;//当前使用的道具位置
	int           m_iCurHide;
	bool          m_bTheFirstTime;
	CApp         *m_App;
	bool          m_bRender;
public:
	CGridBasedUI();
	~CGridBasedUI();
	void SetData(CApp *App);
	
	void UseWhichTex(int DestX,int DestY,int number);
	bool InRect(int x,int y,POINT *pt);
	int  GetStartX();
	int  GetStartY();
	void SetFirstTime();
	int  GetCurrentPick();
	bool GridBasedUIClosed();

	bool MouseStopOnGridBasedUI(CInputDevice *m_Mouse);

};
#endif