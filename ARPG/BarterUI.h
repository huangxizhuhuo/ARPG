/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#ifndef BARTERUI_H
#define BARTERUI_H

#include "GridBasedUI.h"
#include "MIL.h"

struct sItemGrid1//全局可见
{
	bool bEmpty;
	int ID;//所在MIL中的编号
	int ICSID;//所在ICS中的编号
	int x1,y1;//左上角坐标
	bool bVisible;
	sItemGrid1()
	{
		bEmpty=true;
		bVisible=true;
	}
};
class CApp;
class CCharICS;

class CBarterUI : public CGridBasedUI
{
private:
	sItemGrid1 ItemGrid[100];
public:
	CBarterUI();
	~CBarterUI();
	void Init(CGraphics *Graphics,CInputDevice *Mouse);
	void Render(POINT *pt,CCharICS *ICS);
	void IfMouseOn(POINT *pt,CCharICS *ICS);	
	int  FindTheFirstEmptyGrid();
	int  GetMouseOnWhichGrid(POINT *pt,int BarterX,int BarterY);
	void CloseBarterUI();
	int  FindTheFirstEmptyGridOfEquipUI();
};
#endif