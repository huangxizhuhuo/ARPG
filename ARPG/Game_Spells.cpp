/*--------------------------------------------
Jim Adams ��Ȩ����
2007-2008 SCY �޸�
--------------------------------------------*/
#include "Global.h"

BOOL CSpells::SpellSound(long Num)
{
	if(m_App != NULL) 
		m_App->PlaySound(Num);
	return TRUE;
}

