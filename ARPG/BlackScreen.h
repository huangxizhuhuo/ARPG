/*--------------------------------------------
2007-2008 SCY
--------------------------------------------*/
#ifndef BLACKSCREEN_H
#define BLACKSCREEN_H

class CBlackScreen
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
	char Temp[1024];
	int  PrintLength;
	int  Length;	
	DWORD UpdateTimer;
public: 
	  bool bPrintAll;
	  int  PrintSpeed;
	  CBlackScreen();
	  ~CBlackScreen();  
	  bool Create(CGraphics *Graphics, CGameFont *Font,CTexture *Texture,CInputDevice *Mouse); 
	  bool Free();
	  bool SetText(char *Text, D3DCOLOR TextColor = 0xFFFFFFFF);
	  bool Render(char *Text = NULL, D3DCOLOR Color = 0xFFFFFFFF);
};
#endif
