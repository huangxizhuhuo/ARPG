#include "Core_System.h"

CApplication::CApplication()
{	
	g_pApplication = this;// Save instance handle  
    m_hInst = GetModuleHandle(NULL);// Get the instance handle  
    strcpy_s(m_Class, "AppClass");// Set default 
    strcpy_s(m_Caption, "Application Caption");
    m_Style  = WS_OVERLAPPEDWINDOW;
    m_XPos   = 0;
    m_YPos   = 0;
    m_Width  = 256;
    m_Height = 256;
    m_wcex.cbSize        = sizeof(WNDCLASSEX);
    m_wcex.style         = CS_CLASSDC;
    m_wcex.lpfnWndProc   = AppWindowProc;
    m_wcex.cbClsExtra    = 0;
    m_wcex.cbWndExtra    = 0;
    m_wcex.hInstance     = m_hInst;
    m_wcex.hIcon         = LoadIcon(m_hInst, MAKEINTRESOURCE("IDI_ICON1"));
    m_wcex.hCursor       = LoadCursor(NULL,IDC_ARROW);
    m_wcex.hbrBackground = NULL;
    m_wcex.lpszMenuName  = NULL;
    m_wcex.lpszClassName = m_Class;
    m_wcex.hIconSm       = LoadIcon(m_hInst, MAKEINTRESOURCE("IDI_ICON1"));
}

HWND CApplication::GethWnd()
{
	return m_hWnd;
}

HINSTANCE CApplication::GethInst()
{
    return m_hInst;
}

BOOL CApplication::Run()
{
	MSG Msg;   
    if(!RegisterClassEx(&m_wcex))// Register window class
        return FALSE;
    // Create the Main Window
    m_hWnd = CreateWindow(m_Class, m_Caption,m_Style,m_XPos,m_YPos,m_Width,m_Height,NULL,NULL,m_hInst,NULL);
    if(!m_hWnd)
        return FALSE;
    // Show and update the window
    ShowWindow(m_hWnd, SW_NORMAL);
    UpdateWindow(m_hWnd);
    // Make sure client area is correct size
    Resize(m_Width, m_Height);
    // Initialize COM
    CoInitialize(NULL); 
	if(Init() == TRUE) 
	{   
		// Enter the message pump   
		ZeroMemory(&Msg, sizeof(MSG));  
		do
		{
			// Handle Windows messages (if any)     
			if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) 
			{     
				TranslateMessage(&Msg);      
				DispatchMessage(&Msg);     
			} 	
			else 
			{     			
				if(Frame() == FALSE)        					
					break; 
						
			} 
		}while(Msg.message != WM_QUIT);
	} 
	Shutdown();
	// Shutdown COM
	CoUninitialize();
	// Unregister the window class
	UnregisterClass(m_Class, m_hInst);
	return TRUE;
}

BOOL CApplication::Error(BOOL Fatal, char *Text, ...)
{
	char CaptionText[12];
	char ErrorText[2048];
	va_list valist;
	// Build the message box caption based on fatal flag
	if(Fatal == FALSE)
		strcpy_s(CaptionText, "Error");
	else 
		strcpy_s(CaptionText, "Fatal Error");
	// Build variable text buffer
	va_start(valist, Text);
	//vsprintf(ErrorText, Text, valist); 
	size_t sz=strlen(Text)+1;
	vsprintf_s(ErrorText, sz, Text, valist); 
	va_end(valist); 
	// Display the message box
	MessageBox(NULL, ErrorText, CaptionText, MB_OK | MB_ICONEXCLAMATION); 
	// Post a quit message if error was fatal 
	if(Fatal == TRUE)   
		PostQuitMessage(0);
	return TRUE;
}

BOOL CApplication::Move(long XPos, long YPos)
{ 
	RECT ClientRect;
    GetClientRect(m_hWnd, &ClientRect);
    MoveWindow(m_hWnd, XPos, YPos, ClientRect.right, ClientRect.bottom, TRUE);
    return TRUE;
}

BOOL CApplication::Resize(long Width, long Height)
{ 
	RECT WndRect, ClientRect;
    long WndWidth, WndHeight;
    GetWindowRect(m_hWnd, &WndRect);
    GetClientRect(m_hWnd, &ClientRect);
    WndWidth  = (WndRect.right  - (ClientRect.right  - Width))  - WndRect.left;
    WndHeight = (WndRect.bottom - (ClientRect.bottom - Height)) - WndRect.top;
    MoveWindow(m_hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);
    return TRUE;
}

BOOL CApplication::ShowMouse(BOOL Show)
{
	ShowCursor(Show);
    return TRUE;
}

// The message procedure - empty except for destroy message
long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{  
	switch(uMsg) 
	{  
	case WM_DESTROY: 
		PostQuitMessage(0); 
		return 0;
	case WM_SYSKEYDOWN://屏蔽ALT菜单弹出键
		return 0;
    default:
		return g_pApplication->MsgProc(hWnd, uMsg, wParam, lParam);
	}
}

//状态管理器,后进先出堆栈管理
CStateManager::CStateManager() 
{  
	m_StateParent = NULL; 
}

CStateManager::~CStateManager() 
{
	PopAll();
}

// Push a function on to the stack
void CStateManager::Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr)
{  
	sState *StatePtr;
	// Don't push a NULL value
	if(Function != NULL)
	{ 
		// Allocate a new state and push it on stack
		StatePtr = new sState();
		StatePtr->Function = Function;
		StatePtr->Next = m_StateParent;  
		m_StateParent = StatePtr; 
		// Call state with init purpose
		StatePtr->Function(DataPtr, INITPURPOSE);
	}
}

BOOL CStateManager::Pop(void *DataPtr)
{
	sState *StatePtr;
    // Remove the head of stack (if any)
    if((StatePtr = m_StateParent) != NULL)
	{
        // First call with shutdown purpose
        m_StateParent->Function(DataPtr, SHUTDOWNPURPOSE);
        m_StateParent = StatePtr->Next;
        StatePtr->Next = NULL;
        delete StatePtr;
     }
     // return TRUE if more states exist, FALSE otherwise
    if(m_StateParent == NULL)
        return FALSE;
    return TRUE;
}

//m_StateParent==NULL时停止Pop操作，即弹出堆栈中所有的函数
void CStateManager::PopAll(void *DataPtr)
{
	while(Pop(DataPtr) == TRUE)
		;
}

BOOL CStateManager::Process(void *DataPtr)
{ 
	// return an error if no more states
    if(m_StateParent == NULL)
        return FALSE;
    // Process the top-most state
    m_StateParent->Function(DataPtr, FRAMEPURPOSE); 
    return TRUE;
}

CProcessManager::CProcessManager() 
{ 
	m_ProcessParent = NULL; 
}

CProcessManager::~CProcessManager() 
{	
	while(Pop()==TRUE)// Pop each process  
		;
}

// Push a function on to the stack
void CProcessManager::Push(void (*Process)(void *Ptr, long Purpose), void *DataPtr)
{
	// Don't push a NULL value
    if(Process != NULL) 
	{
		// Allocate a new process and push it on stack
        sProcess *ProcessPtr = new sProcess();
        ProcessPtr->Function = Process;
        ProcessPtr->Next = m_ProcessParent;
        m_ProcessParent = ProcessPtr;
        // Call process with init purpose
        ProcessPtr->Function(DataPtr, INITPURPOSE);
     }
}

// Pop top process from stack
BOOL CProcessManager::Pop(void *DataPtr)
{
	sProcess *ProcessPtr;
    // Remove the head of stack (if any)
    if((ProcessPtr = m_ProcessParent) != NULL)
	{
        // First call with shutdown purpose
        m_ProcessParent->Function(DataPtr, SHUTDOWNPURPOSE);
        m_ProcessParent = ProcessPtr->Next;
        ProcessPtr->Next = NULL;
        delete ProcessPtr;
     }
     // return TRUE if more processes exist, FALSE otherwise
     if(m_ProcessParent == NULL)
         return FALSE;
     return TRUE;
}

void CProcessManager::PopAll(void *DataPtr)// Process all functions
{
	while(Pop(DataPtr) == TRUE)
		;
}

void CProcessManager::Process(void *DataPtr)
{ 
	sProcess *ProcessPtr = m_ProcessParent;
    while(ProcessPtr != NULL) 
	{
        ProcessPtr->Function(DataPtr, FRAMEPURPOSE);
        ProcessPtr = ProcessPtr->Next;
	}
}

CDataPackage::CDataPackage()  
{   
	m_Buf = NULL; 
    m_Size = 0; 
}

CDataPackage::~CDataPackage() 
{ 
	Free(); 
}

void *CDataPackage::Create(unsigned long Size)
{
	// Free a previously created buffer
    Free();
    // Allocate some memory and return a pointer
    return (m_Buf = (void*)new char[(m_Size = Size)]);
}

// Free the allocated memory
void CDataPackage::Free() 
{ 
	delete m_Buf; 
    m_Buf = NULL; 
    m_Size = 0; 
}

BOOL CDataPackage::Save(char *Filename)
{
	FILE *fp;
    // Make sure there's something to write
    if(m_Buf != NULL && m_Size) 
	{
        // Open file, write size and data
        //if((fp=fopen(Filename, "wb")) != NULL)
		errno_t err;
		if((err=fopen_s(&fp,Filename, "wb")) == 0)
		{
			fwrite(&m_Size, 1, 4, fp);
            fwrite(m_Buf, 1, m_Size, fp);
            fclose(fp);
            return TRUE;
		} 
	}
	return FALSE;
}

void *CDataPackage::Load(char *Filename, unsigned long *Size)
{
	FILE *fp;
    // Free a prior buffer
    Free();
	errno_t err;
    //if((fp=fopen(Filename, "rb"))!=NULL)
	if((err=fopen_s(&fp,Filename,"rb"))==0)
	{
        // Read in size and data
        fread(&m_Size, 1, 4, fp);
        if((m_Buf = (void*)new char[m_Size]) != NULL)
            fread(m_Buf, 1, m_Size, fp);
        fclose(fp);
        // Store size to return
        if(Size != NULL)
            *Size = m_Size;
       // return pointer
       return m_Buf;
	}
    return NULL;
}

void *CDataPackage::GetPtr()
{
	return m_Buf;
}

unsigned long CDataPackage::GetSize()
{
	return m_Size;
}

