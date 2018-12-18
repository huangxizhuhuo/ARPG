/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#ifndef EQUIPUI_H
#define EQUIPUI_H
#include "GridBasedUI.h"
#include "MIL.h"

struct sItemGrid
{
	bool bEmpty;
	int ID;//����MIL�еı��
	int ICSID;//����ICS�еı��
	int x1,y1;//���Ͻ�����
	bool bVisible;
	sItemGrid()
	{
		bEmpty=true;
		bVisible=true;
	}
};	
class CApp;
class CEquipUI:public CGridBasedUI
{
public:
	sItemGrid ItemGrid[100];
	CEquipUI();
	~CEquipUI();
	void Init(CGraphics *Graphics,CInputDevice *Mouse);
	void Render(POINT *pt);
	
	void IfMouseOn(POINT *pt);
	int GetMouseOnWhichGrid(POINT *pt);
	
	int FindTheFirstEmptyGrid();
	
	void CloseEquipUI();
	void SetItemGrid(int ID);
};
#endif