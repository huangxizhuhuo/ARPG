/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#include "Global.h"

BOOL CSpells::SpellSound(long Num)
{
	if(m_App != NULL) 
		m_App->PlaySound(Num);
	return TRUE;
}

