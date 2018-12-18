/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#ifndef TALK_H
#define TALK_H
#include "../Core/Core_Global.h"
class CTalk
{
private:  
    CGraphics     *m_Graphics;         // Parent CGraphics object
    CGameFont         *m_Font;             // Font object
	CTexture      *m_Texture;
    char          *m_Text;             // Text to display
    D3DCOLOR       m_TextColor;        // Color to draw text 
    long           m_XPos, m_YPos;     // Window coordinates
    long           m_Width, m_Height;  // Window dimensions
	CInputDevice  *m_Mouse;
	char Temp[256];
	int  PrintLength;
	int  Length;	
	DWORD UpdateTimer;
public:  
	bool bPrintAll;
	int  PrintSpeed;
	bool bTalking;
	CTalk();
	~CTalk();  
	bool Create(CGraphics *Graphics, CGameFont *Font,CTexture *Texture,CInputDevice *Mouse); 
	bool Free();
	bool SetText(char *Text, D3DCOLOR TextColor = 0xFFFFFFFF);
    bool Render(char *Text = NULL, D3DCOLOR Color = 0xFFFFFFFF,int Type=0);
};
#endif
