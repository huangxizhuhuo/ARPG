/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
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
