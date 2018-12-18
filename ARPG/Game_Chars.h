/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#ifndef GAME_CHARS_H
#define GAME_CHARS_H

class CApp;
class CChars : public CCharacterController
{ 
private:
    CApp      *m_App;
    BOOL PCUpdate(sCharacter *Character, long Elapsed,float *XMove, float *YMove, float *ZMove);                        
    BOOL ValidateMove(sCharacter *Character,float *XMove, float *YMove, float *ZMove);                                  
    BOOL Experience(sCharacter *Character, long Amount);
    BOOL PCTeleport(sCharacter *Character, sSpell *Spell);
    BOOL ActionSound(sCharacter *Character);
    BOOL DropMoney(float XPos, float YPos, float ZPos,long Quantity);                          
    BOOL DropItem(float XPos, float YPos, float ZPos,long Item, long Quantity=1);  
public:
    BOOL SetData(CApp *App) { m_App = App; return TRUE; }
};
#endif
