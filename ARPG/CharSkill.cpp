/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include "CharSkill.h"

CCharskill::CCharskill()
{
    m_NumSkills = 0;
    m_SkillParent = NULL;
}

CCharskill::~CCharskill()
{
    Free();
}

BOOL CCharskill::Load(char *Filename)
{
    FILE *fp;
    long LongNum;
    sCharSkill *Skill, *SkillPtr = NULL;
    Free();
    if((fp=fopen(Filename, "rb"))==NULL)
        return FALSE;
    while(1) 
    {
        if((LongNum = GetNextLong(fp)) == -1)
            break;
        Skill = new sCharSkill();
        if(SkillPtr == NULL)
            m_SkillParent = Skill;
        else 
	    {
            Skill->Prev = SkillPtr;
            SkillPtr->Next = Skill;
        }
        SkillPtr = Skill;
        Skill->SkillNum = LongNum;
	    Skill->Position=GetNextLong(fp);
        Skill->Level= GetNextLong(fp);
	    Skill->Exp=GetNextLong(fp);
	    Skill->UpDown=GetNextLong(fp);
        Skill->Index = m_NumSkills++;
	}
	fclose(fp); 
	return TRUE;
}

BOOL CCharskill::Save(char *Filename)
{
    FILE *fp;
    sCharSkill *Skill;
    long Index = 0;
    if((fp=fopen(Filename, "wb"))==NULL)
        return FALSE;
    if((Skill = m_SkillParent) == NULL) 
    {
        fclose(fp);
        return TRUE;
    }
    while(Skill != NULL) 
    {
        Skill->Index = Index++;
        Skill=Skill->Next;
    }
    Skill = m_SkillParent;
    while(Skill != NULL) 
    {
        fprintf(fp, "%lu\r\n", Skill->SkillNum);   
        fprintf(fp, "%lu\r\n", Skill->Position);  
        fprintf(fp, "%ld\r\n", Skill->Level);	
	    fprintf(fp, "%ld\r\n", Skill->Exp);  
	    fprintf(fp, "%ld\r\n", Skill->UpDown);
        Skill = Skill->Next;
    }
    fclose(fp); // Close the file
    return TRUE; // Return success!
}

BOOL CCharskill::Free()
{
    m_NumSkills = 0;
    delete m_SkillParent;
    m_SkillParent = NULL;
    return TRUE;
}

BOOL CCharskill::Add(long SkillNum, long Position,long Level,long Exp,long UpDown)                 
{
    sCharSkill *Skill;
    Skill = new sCharSkill();   
    Skill->Next = m_SkillParent;// Insert into top of list
    if(m_SkillParent != NULL)
        m_SkillParent->Prev = Skill;
    m_SkillParent = Skill;
    Skill->SkillNum  = SkillNum;
    Skill->Position  = Position;
    Skill->Level     = Level;
    Skill->Exp       = Exp;
    Skill->UpDown    = UpDown;
    m_NumSkills++; 
    return TRUE;
}

BOOL CCharskill::Remove(sCharSkill *Skill)
{ 
    if(Skill->Prev != NULL)
        Skill->Prev->Next = Skill->Next;
    else
        m_SkillParent = Skill->Next;
    if(Skill->Next != NULL)
        Skill->Next->Prev = Skill->Prev;
    Skill->Prev = Skill->Next = NULL;  
    m_NumSkills--;
    delete Skill;
    return TRUE;
}

long CCharskill::GetNumSkills()
{
    return m_NumSkills;
}

sCharSkill *CCharskill::GetParentSkill()
{
    return m_SkillParent;
}

sCharSkill *CCharskill::GetSkill(long Num)
{
    sCharSkill *Skill;
    Skill = m_SkillParent;
    while(Num--)
    {
        if(Skill == NULL)
            return NULL;
        Skill = Skill->Next;
    }
    return Skill;
}

long CCharskill::GetNextLong(FILE *fp)
{
    char Buf[1024];
    long Pos = 0;
    int c;
    while(1)// Read until EOF or EOL 
    {    
		if((c = fgetc(fp)) == EOF)   
			break;   
		if(c == 0x0a)    
			break; 
		if((c >= '0' && c <= '9') || c == '.' || c == '-')    
			Buf[Pos++] = c;
	}
    if(!Pos)
        return -1;
    Buf[Pos] = 0;
    return atol(Buf);
}

float CCharskill::GetNextFloat(FILE *fp)
{
    char Buf[1024];
    long Pos = 0;
    int c;  
    while(1) // Read until EOF or EOL
    {  
		if((c = fgetc(fp)) == EOF)   
			break; 
		if(c == 0x0a)   
			break; 
		if((c >= '0' && c <= '9') || c == '.' || c == '-')     
			Buf[Pos++] = c;  
	}
	Buf[Pos] = 0;
	return (float)atof(Buf);
}
