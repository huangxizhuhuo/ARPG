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
    long         Position;          //�ü���λ�ڿ���λ��
    long         Level;             //PC��ǰ���ܵȼ�
    long         Exp;               //��ǰ�ۻ�����ֵ
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
    long      m_NumSkills;     //��ϰ��������
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
