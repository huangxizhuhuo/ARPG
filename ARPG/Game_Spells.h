/*--------------------------------------------
Jim Adams ��Ȩ����
2007-2008 SCY �޸�
--------------------------------------------*/
#ifndef GAME_SPELLS_H
#define GAME_SPELLS_H

class CSpells : public CSpellController
{
private:
	CApp *m_App;
public:
    BOOL SetData(CApp *App) { m_App = App; return TRUE; }
    BOOL SpellSound(long Num);
};
#endif
