/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#ifndef QUICKUI_H
#define QUICKUI_H
#include "GridBasedUI.h"
#include "MSL.h"
struct sItemGrid3
{
	bool bEmpty;
	int ID;//技能的编号
	int x1,y1;//左上角坐标
	bool bVisible;
	int ID1;
	sItemGrid3()
	{
		bEmpty=true;
		bVisible=true;
		ID=-1;
	}
};
class CApp;
class CCharICS;
class CQuickUI :public CGridBasedUI
{
	
public:
	CQuickUI();
	~CQuickUI();
	sItemGrid3 ItemGrid[10];

	void Init(CGraphics *Graphics,CInputDevice *Mouse);
	void Render(POINT *pt);
	void IfMouseOn(POINT *pt);
	
	int GetMouseOnWhichGrid(POINT *pt);

};
#endif