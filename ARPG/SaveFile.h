/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#ifndef SAVEFILE_H
#define SAVEFILE_H

typedef struct sSaveFile
{
    long CharNum;         
    sSaveFile *Prev, *Next;// linked list pointers
    long HP;
    long MP;
    float X;
    float Y;
    float Z;
    sSaveFile()
    {
	    Prev=Next=NULL;
    }
} sSaveFile;
class CSaveFile
{
private:
	int m_NumChars;
    sSaveFile *m_CharParent;
    long  GetNextLong(FILE *fp);
    float GetNextFloat(FILE *fp);
public:
    CSaveFile();   // Constructor
    ~CSaveFile();  // Destructor
    BOOL Load(char *Filename);
    BOOL Save(char *Filename);
    BOOL Free();
    BOOL Add(long CharNum,float X,float Y,float Z,long HP,long MP);             
    BOOL Remove(sSaveFile *Item);
    long      GetNumChars();
    sSaveFile *GetParentChar();
    sSaveFile *GetChar(long Num);
};
#endif
