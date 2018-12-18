/*-------------------------------------------------------
Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
-------------------------------------------------------*/

#ifndef CORE_SYSTEM_H
#define CORE_SYSTEM_H

#include <cstdio>
#include <windows.h>

class CApplication
{ 
private:
	HINSTANCE     m_hInst;
    HWND          m_hWnd;
protected:
    char          m_Class[MAX_PATH];
    char          m_Caption[MAX_PATH];
    WNDCLASSEX    m_wcex;
    DWORD         m_Style;
    DWORD         m_XPos;
    DWORD         m_YPos;
    DWORD         m_Width;
    DWORD         m_Height; 
public:
    CApplication();
    HWND      GethWnd();
    HINSTANCE GethInst();
    BOOL Run();
    BOOL Error(BOOL Fatal, char *Text, ...);
    BOOL Move(long XPos, long YPos);
    BOOL Resize(long Width, long Height);
    BOOL ShowMouse(BOOL Show = TRUE);
    virtual int MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return (int)DefWindowProc(hWnd, uMsg, wParam, lParam); }
    virtual BOOL Init()       { return TRUE; }
    virtual BOOL Shutdown()   { return TRUE; }
    virtual BOOL Frame()      { return TRUE; }
};
static CApplication *g_pApplication = NULL;
static long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
enum Purposes 
{
    NOPURPOSE = 0,
    INITPURPOSE,
    SHUTDOWNPURPOSE,
    FRAMEPURPOSE
};   

class CStateManager
{
	typedef struct sState//链式堆栈,元素为函数指针 
	{
		void (*Function)(void *Ptr, long Purpose); 
		sState *Next;
		sState()
		{
			Function = NULL;
            Next = NULL;
		}
		~sState()
		{
            delete Next;
        }
	} sState;  
protected:
	sState *m_StateParent;  
public:
    CStateManager();
    ~CStateManager();
    void Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr = NULL);
    BOOL Pop(void *DataPtr = NULL);
    void PopAll(void *DataPtr = NULL);
    BOOL Process(void *DataPtr = NULL);
};

class CProcessManager
{
  typedef struct sProcess 
  {
	  void  (*Function)(void *Ptr, long Purpose);
      sProcess *Next;
      sProcess()
      {
          Function = NULL;
          Next = NULL;
      }
      ~sProcess()
      {
		  delete Next; 
		  Next = NULL;
      }
  } sProcess;  
protected:
	sProcess *m_ProcessParent;  
public:
    CProcessManager();
    ~CProcessManager();
    void Push(void (*Process)(void *Ptr, long Purpose), void *DataPtr = NULL);
    BOOL Pop(void *DataPtr = NULL);
    void PopAll(void *DataPtr = NULL);
    void Process(void *Ptr = NULL);
};

class CDataPackage
{  
protected:
	void          *m_Buf;
    unsigned long  m_Size;  
public:
    CDataPackage();
    ~CDataPackage();
    void *Create(unsigned long Size);
    void Free();
    BOOL Save(char *Filename);
    void *Load(char *Filename, unsigned long *Size);
    void          *GetPtr();
    unsigned long  GetSize();
}; 

#endif
