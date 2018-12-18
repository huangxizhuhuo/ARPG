/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#include <windows.h>
#include <stdio.h>
#include "SaveFile.h"

CSaveFile::CSaveFile()
{
    m_NumChars=0;
    m_CharParent = NULL;
}

CSaveFile::~CSaveFile()
{
    Free();
}

BOOL CSaveFile::Free()
{
    m_NumChars = 0;
    delete m_CharParent;
    m_CharParent = NULL;
    return TRUE;
}

BOOL CSaveFile::Load(char *Filename)
{
    FILE *fp;
    long LongNum;
    sSaveFile *Char, *CharPtr = NULL;
    Free();
    if((fp=fopen(Filename, "rb"))==NULL)
        return FALSE;
    while(1) 
    {
        if((LongNum = GetNextLong(fp)) == -1)
            break;
        Char=new sSaveFile();
        if(CharPtr== NULL)     
			m_CharParent = Char;
		else 	
		{ 
			Char->Prev = CharPtr;      
			CharPtr->Next = Char;
		}
		CharPtr = Char;
		Char->CharNum=LongNum;
		Char->X=GetNextFloat(fp);
		Char->Y=GetNextFloat(fp);	
		Char->Z=GetNextFloat(fp);	
		Char->HP=GetNextLong(fp);	
		Char->MP=GetNextLong(fp);
	}
	fclose(fp); 
	return TRUE;
}

BOOL CSaveFile::Save(char *Filename)
{
    FILE *fp;
    sSaveFile *Char;
    if((fp=fopen(Filename, "wb"))==NULL)
        return FALSE;
    if((Char=m_CharParent) == NULL) 
    {
        fclose(fp);
        return TRUE;
    }
    while(Char != NULL) 
    {
        fprintf(fp, "%lu\r\n", Char->CharNum);   
        fprintf(fp, "%lf\r\n", Char->X); 
	    fprintf(fp, "%lf\r\n", Char->Y);
	    fprintf(fp, "%lf\r\n", Char->Z);
        fprintf(fp, "%lu\r\n", Char->HP);	
	    fprintf(fp, "%lu\r\n", Char->MP);  
		Char = Char->Next;
	}
	fclose(fp); // Close the file
	return TRUE; // Return success!
}

BOOL CSaveFile::Add(long CharNum,float X,float Y,float Z,long HP,long MP)                
{
    sSaveFile *Char;
    Char = new sSaveFile();  
    Char->Next = m_CharParent;// Insert into top of list
    if(m_CharParent != NULL)
        m_CharParent->Prev = Char;
    m_CharParent = Char;
    Char->CharNum=CharNum;
    Char->X=X;
    Char->Y=Y;
    Char->Z=Z;
    Char->HP=HP;
    Char->MP=MP;
    m_NumChars++; 
    return TRUE;
}

BOOL CSaveFile::Remove(sSaveFile *Char)
{ 
    if(Char->Prev != NULL)
        Char->Prev->Next = Char->Next;
    else
        m_CharParent = Char->Next;
    if(Char->Next != NULL)
        Char->Next->Prev = Char->Prev;
    Char->Prev = Char->Next = NULL;  
    m_NumChars--;
    delete Char;
    return TRUE;
}

long CSaveFile::GetNumChars()
{
    return m_NumChars;
}

sSaveFile *CSaveFile::GetParentChar()
{
    return m_CharParent;
}

sSaveFile *CSaveFile::GetChar(long Num)
{
    sSaveFile *Char;
    Char = m_CharParent;
    while(Num--)
    {
        if(Char == NULL)
            return NULL;
        Char = Char->Next;
    }
    return Char;
}

long CSaveFile::GetNextLong(FILE *fp)
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
    if(!Pos)
        return -1;
    Buf[Pos] = 0;
    return atol(Buf);
}
float CSaveFile::GetNextFloat(FILE *fp)
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
    Buf[Pos] = 0;
    return (float)atof(Buf);
}
