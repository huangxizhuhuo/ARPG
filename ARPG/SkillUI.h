/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#ifndef SKILLUI_H
#define SKILLUI_H
#include "GridBasedUI.h"
#include "MSL.h"

struct sItemGrid2
{
	bool bEmpty;
	int ID;//技能的编号
	int x1,y1;//左上角坐标
	bool bVisible;
	int ID1;
	sItemGrid2()
	{
		bEmpty=true;
		bVisible=true;
		ID=-1;
	}
};
class CApp;
class CCharICS;
class CSkillUI:public CGridBasedUI
{
public:
	CSkillUI();
	~CSkillUI();
	sItemGrid2 ItemGrid[100];
	
	void Init(CGraphics *Graphics,CInputDevice *Mouse);
	void Render(POINT *pt);
	
	void IfMouseOn(POINT *pt);
	
	int GetMouseOnWhichGrid(POINT *pt);
	
	int FindTheFirstEmptyGrid();

	void CloseSkillUI();

	int  GetCurrentStudy();	
};
#endif