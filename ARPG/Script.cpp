/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#include <windows.h>
#include <cstdio>
#include "Script.h"

using namespace std;

CScript::CScript()
{
	m_NumActions = 0;
	m_ScriptParent = NULL;
}

CScript::~CScript()
{ 
	Free();
}

BOOL CScript::Load(char *Filename)
{
	FILE *fp;
	long i, j;
	sScript *ScriptHead = NULL, *Script, *ScriptPtr = NULL;
	Free();  // Free prior script
	if((fp=fopen(Filename, "rb"))==NULL)// Open the file for input 
		return FALSE;
	fread(&m_NumActions, 1, sizeof(long), fp);// Get # of script actions and store them
	for(i=0;i<m_NumActions;i++)// Loop through each script action
	{
		// Allocate a script structure and link in  
		Script = new sScript();  
		Script->Next = NULL;  
		if(ScriptPtr == NULL)  
			ScriptHead = Script;    
		else   
			ScriptPtr->Next = Script; 
		ScriptPtr = Script;
		// Get type of action and # of entries  
		fread(&Script->Type, 1, sizeof(long), fp);  
		fread(&Script->NumEntries, 1, sizeof(long), fp);		
		if(Script->NumEntries)// Get entry data (if any)    
		{			   
			Script->Entries = new sScriptEntry[Script->NumEntries]();// Allocate entry array   			    
			for(j=0;j<Script->NumEntries;j++)// Load in each entry
			{
				// Get entry type and data
                fread(&Script->Entries[j].Type, 1, sizeof(long), fp);
                fread(&Script->Entries[j].IOValue,1,sizeof(long),fp);     
				// Get text (if any)
                if(Script->Entries[j].Type == _TEXT&&Script->Entries[j].Length)                
				{
					char *Text;         
					Text=new char[Script->Entries[j].Length];       
					fread(Text,1,Script->Entries[j].Length,fp);
					Script->Entries[j].strText=std::string(Text);                                  
				}
			}   
		}  
	} 
	fclose(fp);
	m_ScriptParent = ScriptHead;
	return TRUE;
}

BOOL CScript::Free()
{
	delete m_ScriptParent;
	m_ScriptParent = NULL;
	m_NumActions = 0;
	return TRUE;
}

BOOL CScript::Execute(char *Filename)
{
	sScript *ScriptPtr;	
	if(Filename != NULL)// Load script if none already
		Load(Filename);
	if(Prepare() == FALSE)// Prepare script data for execution
		return FALSE;
	if((ScriptPtr = m_ScriptParent) == NULL)// Start at beginning of script
		return FALSE;
	while(ScriptPtr != NULL)// Loop until no more script actions
	{
		// Call script function and break on NULL return value. 
		// Any other return type is the pointer to the next function,which is typically ScriptPtr->Next.
		ScriptPtr = Process(ScriptPtr);
	}
	Release();// Release data
	if(Filename != NULL)// Release script if execute loaded it
		Free();
	return TRUE;
}

sScript *CScript::GetParentScript()
{ 
	return m_ScriptParent;
}
