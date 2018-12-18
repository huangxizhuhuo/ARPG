/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#ifndef CHARSKILL_H
#define CHARSKILL_H

typedef struct sCharSkill
{
    long         SkillNum;          // 0-64
    sCharSkill  *Prev, *Next;       // linked list pointers
    long         Index;             // This skill index #
    long         Position;          //该技能位于框格的位置
    long         Level;             //PC当前技能等级
    long         Exp;               //当前累积经验值
    long         UpDown;            //0:Up 1:Down
    sCharSkill()
    { 
        Prev = Next  = NULL;
        Index = 0;   
    }   
    ~sCharSkill() 
    { 
	    delete Next;
    } 
} sCharSkill;
class CCharskill
{ 
private:
    long      m_NumSkills;     //所习技能总数
    sCharSkill *m_SkillParent;
    long  GetNextLong(FILE *fp);
    float GetNextFloat(FILE *fp);
public:
    CCharskill();   // Constructor
    ~CCharskill();  // Destructor
    BOOL Load(char *Filename);
    BOOL Save(char *Filename);
    BOOL Free();  
    BOOL Add(long SkillNum,long Position,long Level,long Exp,long UpDown);// Add and remove an skill             
    BOOL Remove(sCharSkill *Item);
    // Retrieve # skills or parent linked list object
    long      GetNumSkills();
    sCharSkill *GetParentSkill();
    sCharSkill *GetSkill(long Num);
};
#endif
