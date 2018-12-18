//#define FULLSCREENMODE
#include "Global.h"
#include <sstream>
using namespace std;

char *g_Cursor[]=
{	
	{"..\\Image\\cursor\\36-12.png"},
	{"..\\Image\\cursor\\36-13.png"},
	{"..\\Image\\cursor\\36-14.png"},
	{"..\\Image\\cursor\\36-15.png"}
};

static char *g_MainMenu[]=
{
	{"..\\Image\\UI\\36-9.png"},
	{"..\\Image\\UI\\36-10.png"},
	{"..\\Image\\UI\\36-11.png"}
};
// Global names of character meshes
static char *g_CharMeshNames[] =
{
    { "..\\Scene\\Warrior1.x" },   // Mesh # 0
    { "..\\Scene\\Warrior1.x" },   // Mesh # 1
    { "..\\Scene\\Warrior1.x"   },   // Mesh # 2
    { "..\\Scene\\Warrior1.x"   },   // Mesh # 3
    { "..\\Scene\\Warrior1.x"   },   // Mesh # 4
    { "..\\Scene\\Warrior1.x"   },   // Mesh # 5
	{ "..\\Scene\\Warrior1.x" },   // Mesh # 6
	{ "..\\Scene\\Warrior1.x"    },   // Mesh # 7
    { "..\\Scene\\Warrior1.x"     },   // Mesh # 8
    { "..\\Scene\\Warrior1.x"     },   // Mesh # 9
	{ "..\\Scene\\Warrior1.x"     }    // Mesh # 10
};
// Global character animation information
static sCharAnimationInfo g_CharAnimations[] = 
{
    { "Idle",  TRUE  },
    { "Walk",  TRUE  },
    { "Swing", FALSE },
    { "Spell", FALSE },
    { "Swing", FALSE },
    { "Hurt",  FALSE },
    { "Die",   FALSE },
    { "Idle",  TRUE  }
};
// Global spell mesh information
static char *g_SpellMeshNames[] =
{
    { "..\\Magic\\Fireball.x"   },
    { "..\\Magic\\Explosion.x"  },
    { "..\\Magic\\Ice.x"        },
    { "..\\Magic\\Heal.x"       },
    { "..\\Magic\\Teleport.x"   },
    { "..\\Magic\\Groundball.x" },
    { "..\\Magic\\Bomb.x"       },
    { "..\\Magic\\Force.x"      }
};
// Global sound effect filenames
#define NUM_SOUNDS 20
static char *g_SoundFilenames[NUM_SOUNDS] = 
{
    { "..\\Sound\\Attack1.wav" },
    { "..\\Sound\\Attack2.wav" },
    { "..\\Sound\\Spell.wav"   },
    { "..\\Sound\\null.wav"    },
    { "..\\Sound\\Hurt1.wav"   },
    { "..\\Sound\\Hurt2.wav"   },
    { "..\\Sound\\Die1.wav"    },
    { "..\\Sound\\Die2.wav"    },
    { "..\\Sound\\null.wav"    },
	{"..\\Sound\\UI\\in_button_press.wav"},//9
	{"..\\Sound\\UI\\in_item_equip.wav"},
	{"..\\Sound\\UI\\in_item_down.wav"},
	{"..\\Sound\\UI\\in_item_use.wav"},//12
	{"..\\Sound\\UI\\in_window_open.wav"},
	{"..\\Sound\\UI\\in_window_close.wav"},
	{"..\\Sound\\UI\\in_trade_ok.wav"},//15
	{"..\\Sound\\UI\\in_quickslot.wav"},
	{"..\\Sound\\UI\\in_error.wav"},
	{"..\\Sound\\UI\\null.wav"},
	{"..\\Sound\\UI\\in_screen_change.wav"}
};
// Global music filenames
static long g_CurrentMP3=-1;
static char *g_MP3Filenames[]=
{
	{"..\\Sound\\BGM\\HuiMengYouXian.mp3"},
	{"..\\Sound\\BGM\\HMYXBD.mp3"},
};
static char *g_VideoFilenames[]=
{
	{"..\\Video\\20070702.mpg"},
	{"..\\Video\\clock.avi"},
	{"..\\Video\\yycs.wmv"}
};
static long g_CurrentMusic = -1;
static char *g_MusicFilenames[] = 
{
    { "..\\Sound\\Cathedral_Sunrise.mid" },
    { "..\\Sound\\Distant_tribe.mid"     },
    { "..\\Sound\\Escape.mid"            },
    { "..\\Sound\\Jungle1.mid"           },
    { "..\\Sound\\Magic_Harp.mid"        },
    { "..\\Sound\\Medi_Strings.mid"      },
    { "..\\Sound\\Medi_techno.mid"       },
    { "..\\Sound\\Song_of_the_sea.mid"   },
    { "..\\Sound\\Storm.mid"             },
	{ "..\\Sound\\BGM\\HuanShenJie.mid"  },
	{ "..\\Sound\\gameover.mid"          }
};
// Global character pointer to PC, bartering NPC, and barter ICS
 sCharacter *g_PCChar = NULL;
 sCharacter *g_BarterChar = NULL;
static char   g_BarterICS[MAX_PATH];
 sItem       g_MIL[1024];//全局主道具列表
 CTexture    g_Tex1,g_Tex2,g_Tex3,g_Tex4,g_Texture0,g_Texture1,g_Mouse1,g_Hand,g_Hand1;
 CGameFont   g_Big,g_Small,g_Mid;
 bool GameIsOver=false;
 bool EquipOrSkill;
 bool bool1;
 bool bKillMonster;
 bool bUseDefaultCursor;
static bool bPlay0=true,bPlay1=true,bPlay2=true,bPlay3=true;
static bool bShowSpellLevelUpMessage;
static bool bLastFramePressCtrl=false;
 bool MonstersInLevel;
static long LevelUp[6] = { 50, 200, 500, 1200, 2800, 5500 };
 int CameraHeight;
static long r=0;
static CCharICS ICS;
static float curCameraPosY;//全局作用域内的静态变量，初始值为主角的Y坐标加摄像机高度
static stringstream ss;
CApp::CApp()
{  
	m_Width  = 800; 
    m_Height = 600;
    m_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;                                  
    strcpy_s(m_Class, "GameClass");
    strcpy(m_Caption, "ARPG");
}

BOOL CApp::Init()
{
	// Initialize the graphics device
    m_Graphics.Init();
    // Determine to use fullscreen mode or not
#ifdef FULLSCREENMODE
	m_Graphics.SetMode(GethWnd(), FALSE, TRUE, 800, 600);
#else
    m_Graphics.SetMode(GethWnd(), TRUE, TRUE);
#endif
	// Set perspective
    m_Graphics.SetPerspective(0.6021124f,1.33333f,10.0f,20000.0f);//Pi/5
    // Enable cursor
    ShowMouse(FALSE);
    // Create a font
    m_Font.Create(&m_Graphics, "Arial", 16, TRUE);
    // Initialize input and input devices
    m_Input.Init(GethWnd(), GethInst());
    m_Keyboard.Create(&m_Input, KEYBOARD);
    m_Mouse.Create(&m_Input, MOUSE, TRUE);
	m_JoyStick.Create(&m_Input,JOYSTICK,TRUE);
  
	SetFocus(GethWnd());

    // Initialize the sound system and channels
    m_Sound.Init(GethWnd(), 22050, 1, 16);
    m_SoundChannel.Create(&m_Sound, 22050, 1, 16);
    //m_MusicChannel.Create(&m_Sound);
    m_MP3.Init(GethWnd());
    bUseDefaultCursor=true;
    bShowSpellLevelUpMessage=false;
    // Load the master item list
    FILE *fp;
    for(long i=0;i<1024;i++)
        ZeroMemory(&g_MIL[i], sizeof(sItem));
    if((fp=fopen("..\\Script\\Game.mil", "rb")) != NULL) 
    {
        for(int i=0;i<1024;i++)
            fread(&g_MIL[i], 1, sizeof(sItem), fp);
        fclose(fp);
    }
    // Initialize the character controller
    m_CharController.SetData(this);
    m_CharController.Init(&m_Graphics, &m_Font,"..\\Script\\Game.mcl", (sItem*)&g_MIL,	 
		m_SpellController.GetSpell(0),sizeof(g_CharMeshNames)/sizeof(char*), g_CharMeshNames,	  
		"..\\Scene\\", "..\\Scene\\",sizeof(g_CharAnimations)/sizeof(sCharAnimationInfo),  
		(sCharAnimationInfo*)&g_CharAnimations,&m_SpellController);                      
    // Initialize the spell controller
    m_SpellController.SetData(this);
    m_SpellController.Init(&m_Graphics, "..\\Script\\Game.msl",
		sizeof(g_SpellMeshNames)/sizeof(char*),g_SpellMeshNames,"..\\Magic\\", &m_CharController);     
    m_StatsBar.Load(&m_Graphics, "..\\Image\\UI\\StatsBar.png");
    m_HPMPBar.Load(&m_Graphics, "..\\Image\\UI\\hpmp.bmp",0x00000000,D3DFMT_A1R5G5B5);
    m_Energy.Load(&m_Graphics, "..\\Image\\UI\\energy.png",0xFFFFFFFF,D3DFMT_A1R5G5B5);
    m_PCPic.Load(&m_Graphics, "..\\Image\\UI\\pc.png",0xFFFFFFFF,D3DFMT_A1R5G5B5);
    m_MessageFont.Create(&m_Graphics,"Arial",12,true);

	g_Mid.Create(&m_Graphics,"Arial",12,true);
    g_Small.Create(&m_Graphics,"Arial",10,true);
    g_Big.Create(&m_Graphics,"Arial",16,true);
    g_Tex1.Load(&m_Graphics,"..\\Image\\UI\\Item_1.bmp",0xFF000000,D3DFMT_A1R5G5B5);
    g_Tex2.Load(&m_Graphics,"..\\Image\\UI\\Item_2.bmp",0xFF000000,D3DFMT_A1R5G5B5);
    g_Tex3.Load(&m_Graphics,"..\\Image\\UI\\Item_3.bmp",0xFF000000,D3DFMT_A1R5G5B5);
    g_Tex4.Load(&m_Graphics,"..\\Image\\UI\\Item_4.bmp",0xFF000000,D3DFMT_A1R5G5B5);
    g_Texture0.Load(&m_Graphics,"..\\Image\\UI\\big2.bmp",0x00000000,D3DFMT_A8R8G8B8);
    g_Texture1.Load(&m_Graphics,"..\\Image\\UI\\big1.bmp",0x00000000,D3DFMT_A8R8G8B8);
    g_Hand.Load(&m_Graphics,"..\\Image\\cursor\\\\hand.png");
    g_Hand1.Load(&m_Graphics,"..\\Image\\cursor\\hand1.png");
    g_Mouse1.Load(&m_Graphics,"..\\Image\\cursor\\mouse1.png");
  
	m_Script.SetData(this);
    m_EquipUI.SetData(this);
    m_BarterUI.SetData(this);
    m_SkillUI.SetData(this);
	m_QuickUI.SetData(this);
    m_EquipUI.Init(&m_Graphics,&m_Mouse);  
    m_BarterUI.Init(&m_Graphics,&m_Mouse);
    m_SkillUI.Init(&m_Graphics,&m_Mouse);   
    m_QuickUI.Init(&m_Graphics,&m_Mouse);

    m_TalkPic.Load(&m_Graphics,"..\\Image\\UI\\talk.png",0xFF000000,D3DFMT_A1R5G5B5);////////
    m_Black.Load(&m_Graphics,"..\\Image\\black.bmp",0xFFFFFFFF,D3DFMT_A1R5G5B5);
  
	//m_Talk.Create(&m_Graphics, &m_Font, &m_TalkPic, &m_Mouse);

    bKillMonster=false;
    
	// Push the main menu state, setting menu options first
	//m_StateManager.Push(MenuFrame, this);
    UpdateTimerAtX=0;
    UpdateTimerAtY=0;
    UpdateTimerAtZ=0;
    UpdateTimerPointZ=0;
    UpdateTimerPointY=0;
    UpdateTimerPointX=0;
    bPCControl=TRUE;
  
	m_CameraAngle=0;
    CameraHeight=500;
    bMoveCamera=false;

	// initialize it!
	CMinMax <float> minMax;
	minMax.m_Min=30;
	minMax.m_Max=50;
	m_PartSys.SetSize(minMax);
	m_PartSys.SetLifeTime(minMax);
	m_PartSys.SetStartColor1(D3DCOLOR_RGBA(255,0,0,0));
	m_PartSys.SetEndColor1(D3DCOLOR_RGBA(255,255,255,255));
	m_PartSys.SetTexture("..\\Image\\Particle.png");
	m_PartSys.RestoreDeviceObjects(m_Graphics.GetDeviceCOM());

	m_bFreeLeftButton=true;
  
	// Push the main menu state, setting menu options first
	m_StateManager.Push(MenuFrame, this);
    return TRUE;
}

CChars* CApp::GetCharController()
{
	return &m_CharController;
}

BOOL CApp::Shutdown()
{
	// Pop all states
    m_StateManager.PopAll(this);
    // Free controllers
    m_CharController.Free();
    m_SpellController.Free();
    // Free script object
    m_Script.Free();
    // Free level data
    FreeLevel();
    m_StatsBar.Free();
    m_HPMPBar.Free();
    m_PCPic.Free();
    m_Energy.Free();
    // Shutdown sound
    //m_MusicChannel.Free();
    m_SoundChannel.Free();
    m_Sound.Shutdown();
    m_MP3.Shutdown();
    // Shutdown input
    m_Keyboard.Free();
    m_Mouse.Free();
	m_JoyStick.Free();
    m_Input.Shutdown();
    // Shutdown graphics
    m_Font.Free();
    m_MessageFont.Free();
    m_Graphics.Shutdown();
    return TRUE;
}

BOOL CApp::Frame()
{	
	static DWORD UpdateTimer = timeGetTime();
    // Limit all frame updates to 30 fps
    if(timeGetTime() < UpdateTimer + 33)
        return TRUE;
    UpdateTimer = timeGetTime();
    // Acquire input devices and read input for all states
    m_Keyboard.Acquire(TRUE);// Read keyboard
    m_Keyboard.Read();
    m_Mouse.Acquire(TRUE);// Read mouse
    m_Mouse.Read();
	m_JoyStick.Acquire(TRUE);
	m_JoyStick.Read();
    // Process state, returning result
	
	m_PartSys.Update(0.01f);
    return m_StateManager.Process(this);
}

BOOL CApp::RenderFrame(long Elapsed)
{ 
	// Render simplified mesh for z-values
    m_Graphics.EnableZBuffer(TRUE);
    m_SceneObject.Render();  
    m_CharController.Render(Elapsed);
    m_SpellController.Render();
    return TRUE;
}

float CApp::GetHeightBelow(float XPos, float YPos, float ZPos)//射线检测法
{
	BOOL  Hit;
    float u, v, Dist;
    DWORD FaceIndex;
    D3DXIntersect(m_SceneMesh.GetParentMesh()->m_Mesh,&D3DXVECTOR3(XPos,YPos,ZPos),                  
                  &D3DXVECTOR3(0.0f, -1.0f, 0.0f),//y负向,向下
                  &Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL);
    if(Hit == TRUE)
        return YPos-Dist;
    return YPos;
}

BOOL CApp::CheckIntersect(float XStart, float YStart, float ZStart,float XEnd,float YEnd,float ZEnd,float *Length)
{//返回a点到b点的直线是否与面相交,Length(a点到面的距离)
    BOOL  Hit;
    float u, v, Dist;
    float XDiff, YDiff, ZDiff, Size;
    DWORD FaceIndex;
 
	D3DXVECTOR3 vecDir;
    XDiff = XEnd - XStart;
    YDiff = YEnd - YStart;
    ZDiff = ZEnd - ZStart;
    D3DXVec3Normalize(&vecDir, &D3DXVECTOR3(XDiff, YDiff, ZDiff));
    D3DXIntersect(m_SceneMesh.GetParentMesh()->m_Mesh,&D3DXVECTOR3(XStart,YStart,ZStart),    
		&vecDir,&Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL);                         
    if(Hit == TRUE)//如果直线与面相交,(也可能平行、异面)
    {    
		Size = (float)sqrt(XDiff*XDiff+YDiff*YDiff+ZDiff*ZDiff);
        if(Dist > Size)//即两点确定的直线并未相交于面
            Hit = FALSE;     
	    else
	    { 
			if(Length != NULL)
            *Length = Dist;
        }
	}
    return Hit;
}

float CApp::GetNextFloat(FILE *fp)
{
    char Buf[1024];
    long Pos = 0;
    int c; 
    while(1)// Read until EOF or EOL
    {
      if((c = fgetc(fp)) == EOF)
          break;
      if(c == 0x0a || c == ' ')
          break;
      if((c >= '0' && c <= '9') || c == '.' || c == '-')
          Buf[Pos++] = c;
    }
    Buf[Pos] = 0;
    return (float)atof(Buf);
}

long CApp::GetNextLong(FILE *fp)
{
	char Buf[1024];
    long Pos = 0;
    int c;  
    while(1)// Read until EOF or EOL
	{
      if((c = fgetc(fp)) == EOF)
        break;
      if(c == 0x0a || c == ' ')
        break;
      if((c >= '0' && c <= '9') || c == '.' || c == '-')
        Buf[Pos++] = c;
	}
    Buf[Pos] = 0;
    return (long)atoi(Buf);
}

BOOL CApp::LoadLevel(long Num)
{
    char Filename[MAX_PATH];
    FreeLevel(); // Free a prior level 
    m_SceneNum = Num;// Record scene number
    sprintf(Filename,"..\\Scene\\Scene%ld.x",Num);
	if(m_SceneMesh.Load(&m_Graphics, Filename,"..\\Scene\\") == FALSE)
        return FALSE;
	//if(m_SceneMesh.Load(&m_Graphics, "..Scene\\ho\\zx.x","..\\Scene\\ho\\") == FALSE)
    //    return FALSE;
    m_SceneObject.Create(&m_Graphics, &m_SceneMesh);
    m_MonstersLastFrame = FALSE;
    // Execute the script for loading this scene
	sprintf(Filename,"..\\Script\\Scene%ld.mls",Num);
    //ss<<"..\\Script\\Scene"<<Num<<".mls";
    m_Script.Execute(Filename);
    return TRUE;
}

BOOL CApp::FreeLevel()//保留PC至下一场景,其他一切清空
{
    sCharacter *CharPtr, *NextChar;
    m_SceneMesh.Free();
    m_SceneObject.Free();
    m_Barrier.Free();
    m_Trigger.Free();
    // Free all non-pc characters
    if((CharPtr=m_CharController.GetParentCharacter()) != NULL)
    {
        while(CharPtr != NULL) 	  
		{     	       	   
		   NextChar = CharPtr->Next;// Remember next character          
           if(CharPtr->Type != CHAR_PC)// Remove non-PC character       			  
			   m_CharController.Remove(CharPtr);                 
		   CharPtr = NextChar;// Go to next character
	   }
	}  
    m_SpellController.Free(); // Free all spell effects
    return TRUE;
}

bool CApp::PlayAVI(long Num)
{
	m_AVI.Init(GethWnd());
	m_AVI.Render(g_VideoFilenames[Num]);
	m_AVI.Play();
	return true;
}

bool CApp::StopAVI()
{
	m_AVI.Stop();
	return true;
}

bool CApp::PlayMP3(long Num)
{
	if(g_CurrentMP3==Num)
		return true;
	m_MP3.Init(GethWnd());
	m_MP3.Render(g_MP3Filenames[Num]);	   
	m_MP3.Play();	   
	g_CurrentMP3=Num;
	return true;
}

bool CApp::StopMP3()//如何循环播放MP3?
{
	m_MP3.Shutdown();
	g_CurrentMP3=-1;
	return true;
}

BOOL CApp::PlaySound(long Num)
{
	if(Num >=0 && Num < NUM_SOUNDS)
    {
		m_SoundData.Free();
        if(m_SoundData.LoadWAV(g_SoundFilenames[Num]) == TRUE)
            m_SoundChannel.Play(&m_SoundData);
        return TRUE; 
	}
    return FALSE;
}

BOOL CApp::PlayMusic(long Num)
{
    // Don't bother changing song if same already playing
    if(g_CurrentMusic == Num)
        return TRUE;
    // Stop and free current song
    //m_MusicChannel.Stop();
    //m_MusicChannel.Free();
    // Fade music out, giving DirectMusic enough time to finish up last song (or else new song doesn't 
    // play correctly.  The 700 is based on play volume of music, so adjust ahead.
    DWORD Timer = timeGetTime() + 700;
    while(timeGetTime() < Timer)//足够的时间逐渐减小音量
    {
        DWORD Level = (Timer - timeGetTime())/10;
        //m_MusicChannel.SetVolume(Level);
    }
    // Load and play new song
    //m_MusicChannel.Load(g_MusicFilenames[Num]);
    //m_MusicChannel.Play(70,0);
    // Remember new song #
    g_CurrentMusic = Num;
    return TRUE;
}

BOOL CApp::StopMusic()
{
    // Stop and free music, marking current song as none
    //m_MusicChannel.Stop();
   // m_MusicChannel.Free();
    g_CurrentMusic = -1;
    return TRUE;
}

BOOL CApp::WinGame()//游戏结束,返回主界面
{
    m_StateManager.PopAll(this);
    m_StateManager.Push(MenuFrame, this);
    return TRUE;
}

BOOL CApp::StartOfCombat()
{
    long i;
    char Filename[MAX_PATH];
    // Clear combat booty
    m_CombatExp = 0;
    m_CombatMoney = 0;
    for(i=0;i<1024;i++)
        m_CombatItems[i] = 0;
    // 触发战斗开始脚本
    //ss<<"..\\Script\\SOC"<<m_SceneNum<<".mls";
	sprintf(Filename,"..\\Script\\SOC%ld.mls",m_SceneNum);
    m_Script.Execute(Filename);
    return TRUE;
}

BOOL CApp::EndOfCombat()
{
    char Filename[MAX_PATH];
	char Text[2000] = {};
	char Gained[128] = {};
    long i;
    // Give earned combat stuff
    g_PCChar->Def.Money += m_CombatMoney;
    g_PCChar->Def.Experience += m_CombatExp;
    long NumItems=g_PCChar->CharICS->GetNumItems();
    strcpy(Text, "\r\n\n");
    // Gold gained
    if(m_CombatMoney)
    {
        //sprintf(Gained, "得到 %lu 金!\r\n", m_CombatMoney);
        strcat(Text, Gained);
    }
    // Experience gained
    if(m_CombatExp)
    {
        sprintf(Gained, "得到 %lu 点经验!\r\n", m_CombatExp);
        strcat(Text, Gained);
    }
	int curStudySpell;
    if((curStudySpell=GetCurrentStudySpell())!=-1)
        g_PCChar->CharSkill->GetSkill(curStudySpell)->Exp+=3;
    // Process level up 
    for(i=0;i<6;i++)
    {
        if(g_PCChar->Def.Experience >= LevelUp[i] &&g_PCChar->Def.Level < (i+2))                                         
	    {    
			g_PCChar->Def.Level = i+2;
            strcat(Text, "等级提升!\r\n");
            // Add bonuses for leveling up Abilities
            g_PCChar->Def.HealthPoints += 10;
            g_PCChar->Def.ManaPoints += 10;
            g_PCChar->Def.STR += 10;
            g_PCChar->Def.DEF += 10;
            g_PCChar->Def.DEX += 10;
            g_PCChar->Def.RES += 10;
            g_PCChar->Def.ENE += 10;
            g_PCChar->Def.HIT += 10;     
			if(i < 5)  
			{      
				g_PCChar->Def.MagiCSpells[0] |= (1 << i);
                sprintf(Gained, "领悟武学 %s\r\n",m_SpellController.GetSpell(i)->Name);                                      
                strcat(Text, Gained);
		        int pos=FindTheFirstEmptyGridOfSkillUI();
		        g_PCChar->CharSkill->Add(i,pos,1,0,0);
            }     
			// Max health and mana to match definition
            g_PCChar->HealthPoints=m_CharController.GetHP(g_PCChar); // g_PCChar->Def.HealthPoints;
            g_PCChar->ManaPoints=m_CharController.GetMP(g_PCChar);   // g_PCChar->Def.ManaPoints;  
		}
	}
	// Add gained items
    for(i=0;i<50;i++)
    {
    
		if(m_CombatItems[i])
	    {
	        int pos=m_BarterUI.FindTheFirstEmptyGridOfEquipUI();
	        g_PCChar->CharICS->Add(i,m_CombatItems[i],pos);
            sprintf(Gained, "得到%lu个%s\r\n", m_CombatItems[i],g_MIL[i].Name); //数量X名称             
            strcat(Text, Gained);
        }
     }
     m_CharController.SetMessage(g_PCChar,Text,2000,0xFF0000FA);
     //触发战斗结束脚本
     sprintf(Filename, "..\\Script\\EOC%lu.mls", m_SceneNum);
     m_Script.Execute(Filename);
     return TRUE;
}

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)                           
{	  
	CApp App; 
	return App.Run();
}

BOOL CApp::SetupTeleport(long Level,float XPos, float YPos, float ZPos)                                                 
{  
	// Remember which map to teleport to
    m_TeleportMap = Level;
    // Move character to correct teleport location
    m_CharController.Move(0, XPos, YPos, ZPos);//0:第一PC
    return TRUE;
}

BOOL CApp::SetupBarter(sCharacter *Character, char *ICSFilename)
{
    g_BarterChar = Character;
    strcpy(g_BarterICS, ICSFilename);
    m_StateManager.Push(BarterFrame, this);
    return TRUE;
}

sCharacter *CApp::GetCharacterAt(long XPos, long YPos)
{ 
	D3DXVECTOR3 vecRay, vecDir;   
    D3DXVECTOR3 veCMeshRay, veCMeshDir;
    D3DXVECTOR3 vecTemp;          
    D3DXMATRIX  matProj, matView, *matWorld;
    D3DXMATRIX  matInv;           
    DWORD       FaceIndex;
    BOOL        Hit;
    float       u, v, Dist;
    sCharacter  *CharPtr;         
    sMesh      *MeshPtr;
    // Get parent character object  
    if((CharPtr = m_CharController.GetParentCharacter()) == NULL)
        return NULL;
    // Get the project, view, and inversed view matrices
    m_Graphics.GetDeviceCOM()->GetTransform(D3DTS_PROJECTION,&matProj);                                         
    m_Graphics.GetDeviceCOM()->GetTransform(D3DTS_VIEW,&matView);         
    //与dxsdkpick所述同                                        
    D3DXMatrixInverse(&matInv, NULL, &matView);
    // Compute the vector of the pick ray in screen space
    vecTemp.x =  (((2.0f * (float)XPos) /(float)m_Graphics.GetWidth()) - 1.0f) / matProj._11;                                                          
	vecTemp.y = -(((2.0f * (float)YPos) /(float)m_Graphics.GetHeight()) - 1.0f) /matProj._22;                                                          
	vecTemp.z =  1.0f;
	// Transform the screen space ray 
	vecRay.x = matInv._41;
	vecRay.y = matInv._42;
	vecRay.z = matInv._43;
	vecDir.x = vecTemp.x * matInv._11 + vecTemp.y * matInv._21 + vecTemp.z * matInv._31;                                                                     
	vecDir.y = vecTemp.x * matInv._12 + vecTemp.y * matInv._22 + vecTemp.z * matInv._32;                                                                   
	vecDir.z = vecTemp.x * matInv._13 + vecTemp.y * matInv._23 + vecTemp.z * matInv._33;                                                                       
	// Scan through each character and intersect check
	while(CharPtr != NULL) 
	{
		MeshPtr = CharPtr->Object.GetMesh()->GetParentMesh();
		while(MeshPtr != NULL)// Scan through character meshes
		{  
			// Transform ray and direction by object's world transformation matrix     
			matWorld = CharPtr->Object.GetMatrix(); 
			D3DXMatrixInverse(&matInv, NULL, matWorld);   
			D3DXVec3TransformCoord(&veCMeshRay, &vecRay, &matInv); 
			D3DXVec3TransformNormal(&veCMeshDir, &vecDir, &matInv);    
			// Check for intersection    
			D3DXIntersect(MeshPtr->m_Mesh, &veCMeshRay,&veCMeshDir,&Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL);                     
			// Check if ray hit character and return ID if so 
			if(Hit == TRUE)        
				return CharPtr;    		
			MeshPtr = MeshPtr->m_Next;// Go to next mesh
		}	  
		CharPtr = CharPtr->Next;// Go to next character 
	}
	return NULL;
}

BOOL CApp::LastPointReached(sCharacter *Character)
{
	float XDiff, YDiff, ZDiff;
	float Dist, Radius;
	if(Character == NULL)// Error checking
		return FALSE;
	if(Character->AI != CHAR_ROUTE)  
		return FALSE;
	// Determine if character has reached point 
	XDiff = (float)fabs(Character->XPos - Character->Route[Character->NumPoints-1].XPos);                               
	YDiff = (float)fabs(Character->YPos - Character->Route[Character->NumPoints-1].YPos);                               
	ZDiff = (float)fabs(Character->ZPos - Character->Route[Character->NumPoints-1].ZPos);                               
	Dist = XDiff*XDiff + YDiff*YDiff + ZDiff*ZDiff;
	//按常理应该除以2?
	Radius = m_CharController.GetXZRadius(Character) * 0.25f;  
	if(Dist < (Radius*Radius))// Return TRUE if point being touched   
		return TRUE;	
	return FALSE;// Last point not being touched
}

void CApp::EnsureFrame(void *Ptr,long Purpose)
{
	static CTexture m_Ensure,m_OK,m_OKDown;
	static CGameFont    m_EnsureFont;
	CApp *App=(CApp*)Ptr;
	if(Purpose==INITPURPOSE)
	{
		m_EnsureFont.Create(&App->m_Graphics,"隶书",22,true);
		m_Ensure.Load(&App->m_Graphics,"..\\Image\\Ensure.png",0xFFFFFFFF,D3DFMT_A8R8G8B8);
		m_OK.Load(&App->m_Graphics,"..\\Image\\OK.png",0xFFFFFFFF,D3DFMT_A1R5G5B5);
		return;
	}
	if(Purpose==SHUTDOWNPURPOSE)
	{
		m_Ensure.Free();
		m_EnsureFont.Free();
		m_OK.Free();
		return;
	}
	long XPos,YPos;
	XPos=App->m_Mouse.GetXPos();
	YPos=App->m_Mouse.GetYPos();
	int ensureX=(App->m_Width-300)/2;
	int ensureY=(App->m_Height-200)/2;
	int Num=-1;
	if(YPos>ensureY+113&&YPos<ensureY+138)
	{
		if(XPos>ensureX+35&&XPos<ensureX+109)
			Num=0;
		else if(XPos>ensureX+188&&XPos<ensureX+260)
			Num=1;
	}
	if(App->m_Mouse.GetButtonState(0))
	{
		App->m_Mouse.SetLock(0);
		App->m_Mouse.SetButtonState(0,FALSE);
		if(Num==0)
		{
		  App->PlaySound(19);
		  App->m_Script.Save("..\\Save\\Script.sav");
		  sCharacter* CharPtr;
		  CharPtr=App->m_CharController.GetParentCharacter(); 
          while(CharPtr!=NULL)		  
		  {
			  App->m_SaveFile.Add(CharPtr->ID,CharPtr->XPos,CharPtr->YPos,
				  CharPtr->ZPos,CharPtr->HealthPoints,CharPtr->ManaPoints);			  			  
			  CharPtr=CharPtr->Next;
		  }
		  App->m_SaveFile.Save("..\\Save\\Character.sav");
		  float CameraPosX=App->m_Camera.GetXPos();
		  float CameraPosY=App->m_Camera.GetYPos();
		  float CameraPosZ=App->m_Camera.GetZPos();
		  FILE *fp;
          fp=fopen("..\\Save\\Camera.sav", "wb");            
          fprintf(fp, "%lf\r\n", CameraPosX); 
	      fprintf(fp, "%lf\r\n", CameraPosY);
	      fprintf(fp, "%lf\r\n", CameraPosZ);
          fclose(fp); // Close the file	  
		  App->m_StateManager.Pop(App);
		}
		else if(Num==1)
		{
			App->PlaySound(14);
			App->m_StateManager.Pop(App);
		}
		return;
	}
	App->m_Graphics.Clear();
	if(App->m_Graphics.BeginScene())
	{	
		App->RenderFrame();
		App->m_Graphics.BeginSprite();
		m_Ensure.Blit(ensureX,ensureY);
		m_OK.Blit(ensureX+25,ensureY+110);
		m_OK.Blit(ensureX+175,ensureY+110);
		App->m_Graphics.EndSprite();
		m_EnsureFont.Print("将会覆盖,是否确定?",ensureX+40,ensureY+50);
		if(Num==0)
			m_EnsureFont.Print("是",ensureX+64,ensureY+118,0,0,0xFFFFFF00);
		else
			m_EnsureFont.Print("是",ensureX+64,ensureY+118);
		if(Num==1)
			m_EnsureFont.Print("否",ensureX+215,ensureY+117,0,0,0xFFFFFF00);
		else 
			m_EnsureFont.Print("否",ensureX+215,ensureY+117);
		App->m_Graphics.BeginSprite();
		g_Mouse1.Blit(App->m_Mouse.GetXPos(),App->m_Mouse.GetYPos());
		App->m_Graphics.EndSprite();
		App->m_Graphics.EndScene();
	}
	App->m_Graphics.Display();
}

void CApp::MenuFrame(void *Ptr, long Purpose)//主界面帧
{
	CApp* App=(CApp*)Ptr;
	App->StopMusic();
	GameIsOver = false;

	App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
	App->m_Mouse.SetButtonState(MOUSE_LBUTTON, FALSE);
	App->StopMP3();
	App->PlaySound(19);
	App->m_EquipUI.SetFirstTime();
	//App->m_StateManager.PopAll(App);
	App->m_CharController.Free();
	App->m_SpellController.Free();
	// Reset script flags
	App->m_Script.Reset();
	// Create the PC and get pointer to him/her
	App->m_CharController.Add(0, 0, CHAR_PC, CHAR_STAND, -100.0f, 0.0f, 50.0f, 3.14f);
	g_PCChar = App->m_CharController.GetCharacter(0);
	curCameraPosY = g_PCChar->YPos + CameraHeight;
	// Clear teleport to map #
	App->m_TeleportMap = -1;
	// Push game frame state
	App->m_StateManager.Push(App->GameFrame, App);
	// Start new game and let script process as startup
	App->LoadLevel(1);
	
	/*static CTexture Qi,Cheng,Zhuan,Jie,MainMenu;
	CApp* App=(CApp*)Ptr;
	long  Num=-1;
	App->StopMusic();
	GameIsOver=false;

    if(App->m_MP3.Playing())
		App->PlayMP3(g_CurrentMP3);
    if(Purpose==INITPURPOSE) 
    {	  
	    MainMenu.Load(&App->m_Graphics,"..\\Image\\MainMenu.png");
        Qi.Load(&App->m_Graphics,"..\\Image\\Qi.png",D3DCOLOR_RGBA(255,255,255,255));     
		Cheng.Load(&App->m_Graphics,"..\\Image\\Cheng.png",D3DCOLOR_RGBA(255,255,255,255));
        Zhuan.Load(&App->m_Graphics,"..\\Image\\Zhuan.png",D3DCOLOR_RGBA(255,255,255,255));
        Jie.Load(&App->m_Graphics,"..\\Image\\Jie.png",D3DCOLOR_RGBA(255,255,255,255));
		App->PlayMP3(0);
        return; 
	}
    if(Purpose == SHUTDOWNPURPOSE) 
    {
	    Qi.Free();
	    Cheng.Free();
	    Zhuan.Free();
	    Jie.Free();
	    MainMenu.Free();
        return;
    }
	
	long XPos=App->m_Mouse.GetXPos();
	long YPos=App->m_Mouse.GetYPos();
	if(YPos>425&&YPos<463)
	{
	    if(XPos>175&&XPos<212.5)
	    {
		    Num=0;
		    if(bPlay0)
		    {
		        App->PlaySound(14);
		        bPlay0=false;
		        bPlay1=true;
		        bPlay2=true;
		        bPlay3=true;
		    }
	    }
	    else if(XPos>312.5&&XPos<350)
	    {
		    Num=1;
		    if(bPlay1)
		    {
		        App->PlaySound(14);
		        bPlay1=false;
		        bPlay0=true;
		        bPlay2=true;
		        bPlay3=true;
		    }
	    }
	    else if(XPos>360*1.25&&XPos<390*1.25)
	    {
		    Num=2;
		    if(bPlay2)
		    {
		        App->PlaySound(14);
		        bPlay2=false;
		        bPlay0=true;
		        bPlay1=true;
		        bPlay3=true;
		    }
	    }
	    else if(XPos>460*1.25&&XPos<490*1.25)
	    {
		    Num=3;
		    if(bPlay3)
		    {
		        App->PlaySound(14);
		        bPlay3=false;
		        bPlay0=true;
		        bPlay1=true;
		        bPlay2=true;
		    }
	    }
	    else 
	    {
		    bPlay0=true;
		    bPlay1=true;
		    bPlay2=true;
		    bPlay3=true;
	    }
	}
	else
	{
		bPlay0=true;
		bPlay1=true;
		bPlay2=true;
		bPlay3=true;
	}
	// See which option was selected if mouse button pressed
    if(App->m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE)
    {     
		// Lock the mouse button and clear button state
        App->m_Mouse.SetLock(MOUSE_LBUTTON, TRUE);
        App->m_Mouse.SetButtonState(MOUSE_LBUTTON, FALSE);
		switch(Num)
		{
		case 0://起      
			App->StopMP3();
		    App->PlaySound(19);	  
			App->m_EquipUI.SetFirstTime();
            App->m_StateManager.PopAll(App);
            App->m_CharController.Free();
            App->m_SpellController.Free();
            // Reset script flags
            App->m_Script.Reset();
            // Create the PC and get pointer to him/her
            App->m_CharController.Add(0,0, CHAR_PC, CHAR_STAND,-100.0f, 0.0f, 50.0f, 3.14f);                          
            g_PCChar = App->m_CharController.GetCharacter(0);
		    curCameraPosY=g_PCChar->YPos+CameraHeight;
            // Clear teleport to map #
            App->m_TeleportMap = -1;
            // Push game frame state
            App->m_StateManager.Push(App->GameFrame, App);
            // Start new game and let script process as startup
            App->LoadLevel(1);
		    break;		  
		case 1://承		
			App->StopMP3();
		    App->PlaySound(19);
		    sSaveFile* CharPtr;		   		  
			App->m_StateManager.PopAll(App);
            // Clear all character and spell data
            App->m_CharController.Free();
            App->m_SpellController.Free();		  
            // Load character's stats and inventory
		    FILE *fp;
		    float cX,cY,cZ;
		    fp=fopen("..\\Save\\Camera.sav","rb");
		    cX=App->GetNextFloat(fp);
		    cY=App->GetNextFloat(fp);
		    cZ=App->GetNextFloat(fp);
		    App->m_SaveFile.Load("..\\Save\\Character.sav");
		    CharPtr=App->m_SaveFile.GetParentChar();
	        while (CharPtr!=NULL)
		    {
				if(CharPtr->CharNum==0)  
				{			 
					//ID,Defination,PC/NPC/Monster,AI,Pos,Direction
				    App->m_CharController.Add(CharPtr->CharNum,0, CHAR_PC, CHAR_STAND,
						CharPtr->X, CharPtr->Y,CharPtr->Z, 3.14f);				 
					g_PCChar = App->m_CharController.GetCharacter(0);
			  
				}
				else
				{
					//NPC\Monster添加不进去???
					App->m_CharController.Add(CharPtr->CharNum,8, CHAR_NPC, CHAR_STAND,
						CharPtr->X, CharPtr->Y,CharPtr->Z, 3.14f);
				}			 
				CharPtr=CharPtr->Next;	  
			}	  
			// Load script flags
            App->m_Script.Load("..\\Save\\Script.sav");
            // Clear teleport to map #
            App->m_TeleportMap = -1;
		    App->m_StateManager.Push(App->GameFrame, App);
		    App->LoadLevel(1);
            break;		  	
		case 2://转	
			App->StopMP3();
			App->m_StateManager.PopAll(App);
			App->m_StateManager.Push(App->FirstCGFrame,App);
			break;
		case 3://结
			App->m_StateManager.PopAll(App);
	        break;
		}
		return;  
	}
	App->m_Graphics.Clear(0); 
	if(App->m_Graphics.BeginScene() == TRUE)
	{
		App->m_Graphics.EnableZBuffer(FALSE);
		App->m_Graphics.BeginSprite();	
	    MainMenu.Blit(0,0,0,0,0,0,0.78125,0.5859375);  
		switch(Num)
	    {
		case 0:
			Qi.Blit(int((140-28)*1.25),int((340-20)*1.25));
			break;
		case 1:
			Cheng.Blit(int((250-26)*1.25),int((340-20)*1.25));
		    break;
	    case 2:
			Zhuan.Blit(int((360-24)*1.25),int((340-20)*1.25));
			break;
		case 3:
			Jie.Blit(int((460-12)*1.25),int((340-20)*1.25));
		    break;
		}
		g_Mouse1.Blit(App->m_Mouse.GetXPos(),App->m_Mouse.GetYPos());
		App->m_Graphics.EndSprite();
		App->m_Graphics.EndScene();
	}
	App->m_Graphics.Display();*/
}

void CApp::MinorMenuFrame(void *Ptr,long Purpose)
{
	CApp *App=(CApp*)Ptr;
	static CTexture m_MinorMenu;
	if(Purpose==INITPURPOSE)
	{
		m_MinorMenu.Load(&App->m_Graphics,"..\\Image\\MinorMenu.png",0xFFFFFFFF,D3DFMT_A8R8G8B8);
		return; 
	}
	if(Purpose==SHUTDOWNPURPOSE)
	{
		m_MinorMenu.Free();
		return;
	}
	long XPos=App->m_Mouse.GetXPos();
	long YPos=App->m_Mouse.GetYPos();
	long Num=-1;
	int minorX=(App->m_Width-207)/2;
	int minorY=(App->m_Height-375)/2;
	if(XPos>minorX+40&&XPos<minorX+170)
	{
		if(YPos>minorY+70&&YPos<minorY+100)
		{
			Num=0;
		}
		else if(YPos>minorY+130&&YPos<minorY+160)
		{
			Num=1;
		}
		else if(YPos>minorY+190&&YPos<minorY+220)
		{
			Num=2;
		}
		else if(YPos>minorY+250&&YPos<minorY+280)
		{
			Num=3;
		}
		else if(YPos>minorY+310&&YPos<minorY+340)
		{
			Num=4;
		}
	}
	if(App->m_Mouse.GetButtonState(0))
	{
		App->m_Mouse.SetLock(0);
		App->m_Mouse.SetButtonState(0,FALSE);
		switch(Num)
		{
		case 0:		  
			App->m_StateManager.Pop(App);
		    App->m_StateManager.Push(EnsureFrame,App);
            break;
		case 1:    
			break;		  
		case 2:
			App->PlaySound(14);
			App->m_StateManager.Pop(App);
			break;
		case 3:
			App->PlaySound(19);
			App->m_StateManager.PopAll(App);
			App->m_StateManager.Push(MenuFrame,App);
			break;
		case 4:
			App->m_StateManager.PopAll(App);
			break;
		}
		return;
	}
	App->m_Graphics.Clear();
	if(App->m_Graphics.BeginScene())
	{	
		App->RenderFrame();
		App->m_Graphics.EnableZBuffer(FALSE);
		App->m_Graphics.BeginSprite();
		m_MinorMenu.Blit(minorX,minorY);
		App->m_Graphics.EndSprite();
		if(0==Num)
		    g_Big.Print("储存游戏",minorX+70,minorY+78,0,0,0xFFFFFF00);
		else
			g_Big.Print("储存游戏",minorX+70,minorY+78);
		if(Num==1)
		    g_Big.Print("载入游戏",minorX+70,minorY+137,0,0,0xFFFFFF00);
		else
			g_Big.Print("载入游戏",minorX+70,minorY+137);
		if(Num==2)
			g_Big.Print("回到游戏",minorX+70,minorY+195,0,0,0xFFFFFF00);
		else
			g_Big.Print("回到游戏",minorX+70,minorY+195);
		if(Num==3)
			g_Big.Print("退出游戏",minorX+70,minorY+258,0,0,0xFFFFFF00);
		else
			g_Big.Print("退出游戏",minorX+70,minorY+258);
		if(Num==4)
			g_Big.Print("返回桌面",minorX+70,minorY+318,0,0,0xFFFFFF00);
		else
			g_Big.Print("返回桌面",minorX+70,minorY+318);
		App->m_Graphics.BeginSprite();
		g_Mouse1.Blit(App->m_Mouse.GetXPos(),App->m_Mouse.GetYPos());
		App->m_Graphics.EndSprite();
		App->m_Graphics.EndScene();
	}
	App->m_Graphics.Display();
}
void CApp::GameFrame(void *Ptr, long Purpose)
{
	CApp *App = (CApp*)Ptr;  
	POINT pt;
	GetCursorPos(&pt);
    ScreenToClient(App->GethWnd(),&pt); 
	sCharacter *NPCPointTo=App->GetCharacterAt(pt.x,pt.y);
	App->m_Script.Execute("..\\Script\\shua1.mls");
	App->m_Script.Execute("..\\Script\\shua.mls");
    sCharacter *CharPtr;
    long        TriggerNum;
    char        Filename[MAX_PATH];
    // Only process frame states
    if(Purpose != FRAMEPURPOSE)
        return;
    // Quit to minor menu screen if ESC is pressed
    if(App->m_Keyboard.GetKeyState(KEY_ESC) == TRUE) //压入次菜单帧
    {
	    App->PlaySound(13);
        App->m_StateManager.Push(App->MinorMenuFrame, App);
        return;
    }
	long wheelDelta;
    if(App->m_Keyboard.GetKeyState(KEY_CTRL)==TRUE)
    {	
		wheelDelta=App->m_Mouse.m_MouseState->lZ;//滚轮的相对偏移 
	    //在每一帧都要重新计算摄像机指向的位置，因为角色随时可能移动
	    App->m_pcPointX=g_PCChar->XPos;
	    App->m_pcPointY=g_PCChar->YPos;
	    App->m_pcPointZ=g_PCChar->ZPos;
	    //摄像机的Y坐标根据此帧鼠标滚轮的滚动偏移量来确定，没有滚动其值为0
	    curCameraPosY-=(float)wheelDelta*0.3f;
	    App->m_pcAtY=curCameraPosY;
	    //限制摄像机的Y坐标
	    if(curCameraPosY<150)
		    curCameraPosY=150;
	    else if(curCameraPosY>CameraHeight+300)
			curCameraPosY=(float)(CameraHeight+300);
	  
		if(curCameraPosY>150&&curCameraPosY<(CameraHeight+300))
		{  
			App->MoveCamera(App->m_pcAtX,App->m_pcAtY+(float)wheelDelta*0.3f,App->m_pcAtZ,
				App->m_pcPointX,App->m_pcPointY,App->m_pcPointZ,
				App->m_pcAtX,App->m_pcAtY,App->m_pcAtZ,
				App->m_pcPointX,App->m_pcPointY,App->m_pcPointZ,1000.0f+abs(wheelDelta));  
			App->ToMoveCamera();	
		}
		else
		{
			App->m_Camera.Point(App->m_pcAtX,App->m_pcAtY,App->m_pcAtZ,
				App->m_pcPointX,App->m_pcPointY,App->m_pcPointZ);
			App->m_Graphics.SetCamera(&App->m_Camera);
		} 
	} 
	if(App->bPCControl==TRUE&&App->m_Keyboard.GetKeyState(KEY_CTRL)==FALSE)
	{
		wheelDelta=App->m_Mouse.m_MouseState->lZ;//滚轮的相对偏移		   
		App->m_CameraAngle+=((float)wheelDelta*(float)33/320.0f);
		App->m_pcAtX=g_PCChar->XPos+(float)cos(App->m_CameraAngle)*CameraHeight;
	    App->m_pcAtY=curCameraPosY;
	    App->m_pcAtZ=g_PCChar->ZPos-(float)sin(App->m_CameraAngle)*CameraHeight;
		App->m_pcPointX=g_PCChar->XPos;
		App->m_pcPointY=g_PCChar->YPos;
		App->m_pcPointZ=g_PCChar->ZPos;
		App->m_Camera.Point(App->m_pcAtX,App->m_pcAtY,App->m_pcAtZ,				
			App->m_pcPointX,App->m_pcPointY,App->m_pcPointZ);
		App->m_Graphics.SetCamera(&App->m_Camera); 
	} 
	// If teleporting, then handle that first and return
	if(App->m_TeleportMap != -1)
	{   
		// Free level and process a new one  
		App->FreeLevel();   
		App->LoadLevel(App->m_TeleportMap);    
		App->m_TeleportMap = -1;  // Clear out teleport map #   
		return;  // No more processing this frame
	} 
	// Mark no monsters in level
	MonstersInLevel = FALSE;
    // See if any character are in level. If any monsters, flag as such and change their AI to wander if their
    //charge is less then 70, follow AI otherwise.Also, process whenever a character reaches a route point.
    CharPtr = App->m_CharController.GetParentCharacter();
    while(CharPtr != NULL) 
    {
		// Alter monster's AI based on HP
        if(CharPtr->Type == CHAR_MONSTER) 
	    {     
			MonstersInLevel = TRUE;
            if(CharPtr->HealthPoints>=CharPtr->Def.HealthPoints*0.4)
			{
				CharPtr->AI = CHAR_FOLLOW;
                CharPtr->TargetChar = g_PCChar;
                CharPtr->Distance = 0.0f;   
			} 
			else	  
			{      
				CharPtr->AI = CHAR_WANDER;  
			}
		} 
		// Check if an NPC character has reached last route point
		if(CharPtr->Type==CHAR_NPC && CharPtr->AI==CHAR_ROUTE) 	
		{  
			// Was last point reached? EOR: End Of Route 
			//到达路线终点执行对应EOR脚本   
			if(App->LastPointReached(CharPtr) == TRUE) 
			{  
				// Process the route point script for character.    
				sprintf(Filename,"..\\Script\\EOR%lu.mls", CharPtr->ID);       
				App->m_Script.Execute(Filename);      
				// Don't process any more this frame    
				return;      
			}
		}	   
		CharPtr = CharPtr->Next;// Go to next character 
	}
	// Handle start of combat stuff
	if(MonstersInLevel==TRUE && App->m_MonstersLastFrame==FALSE)  
		App->StartOfCombat();
	// Handle end of combat stuff if combat over
	if(bKillMonster==true)
	{	 
		App->EndOfCombat();	  
		bKillMonster=false;
	}
	// Remember if monsters where in this frame And reset player's charge to full if no monsters
	if((App->m_MonstersLastFrame = MonstersInLevel) == FALSE) 
		g_PCChar->Charge = 100.0f; 
	// Check for triggers and execute script
	if((TriggerNum = App->m_Trigger.GetTrigger(g_PCChar->XPos,g_PCChar->YPos,g_PCChar->ZPos)))                                                                                       
	{  
		sprintf(Filename, "..\\Script\\Trig%lu.mls", TriggerNum); 
		App->m_Script.Execute(Filename); 
		return;  // Don't process any more this frame
	}
	if(wheelDelta==0&&App->bMoveCamera==false)
	{
		App->m_CharController.Update(33);
		App->m_SpellController.Update(33);
		App->m_Graphics.Clear();
		if(App->m_Graphics.BeginScene() == TRUE)
		{ 
			App->RenderFrame(33);
			App->RenderStats();
			App->m_Graphics.BeginSprite();		
			if(bUseDefaultCursor)
				g_Mouse1.Blit(pt.x,pt.y);
			if(NPCPointTo==NULL)
				bUseDefaultCursor=true;
			else
			{
				if(NPCPointTo->Type==CHAR_NPC&&!App->IfThereIsAPick())
				{
					bUseDefaultCursor=false;
					static CTexture m_TalkCursor;
					static int iNum=0,SumFrame=0;
					SumFrame++;
					if(SumFrame%10==0)
						iNum++;
					if(iNum>3)
						iNum=0;
					m_TalkCursor.Load(&App->m_Graphics,g_Cursor[iNum]);
					m_TalkCursor.Blit(pt.x,pt.y);
					m_TalkCursor.Free();
				}
			}
			char Text[512];
			char Gained[128];
			int Num=g_PCChar->CharSkill->GetNumSkills();
			strcpy(Text,"\r\n");
			for(int k=0;k<Num;k++)
			{
				if(g_PCChar->CharSkill->GetSkill(k)->Exp>=
					SkillLevelUp[g_PCChar->CharSkill->GetSkill(k)->Level-1])
				{
					g_PCChar->CharSkill->GetSkill(k)->Level+=1;
					sprintf(Gained,"习得%s第%ld重\r\n",
						App->GetSpellController()->GetSpell(g_PCChar->CharSkill->GetSkill(k)->SkillNum)->Name,
						g_PCChar->CharSkill->GetSkill(k)->Level);
					strcat(Text,Gained);
					bShowSpellLevelUpMessage=true;
				}
			}
			if(bShowSpellLevelUpMessage)
			{
				App->m_CharController.SetMessage(g_PCChar,Text,2000,0xFF0000FF);
				bShowSpellLevelUpMessage=false;
			}

			//if (App->m_Talk.bTalking)
			//{
				//App->m_Talk.Render(0, -1, 1);
				//App->m_Graphics.BeginSprite();
				//m_Head.Blit(650, 460);
				//App->m_Graphics.EndSprite();
				//sCharacter * CharCur = m_App->GetCharController()->GetCharacter(ScriptPtr->Entries[1].lValue);
				//char Text[30];
				//sprintf(Text, "%s：", CharCur->Def.Name);
				//m_App->m_Font.Print(Text, 60, 512);
			//}
			D3DXVECTOR3 v=App->ScreenTo3D(pt.x,pt.y,0);
			char Text1[200] = {};
			//sprintf(Text1,"%f %f %f",v.x,v.y,v.z);
			//g_Big.Print(Text1,200,0);
			sprintf(Text1,"%f %f %f %f %f %f",App->m_pcAtX,App->m_pcAtY,App->m_pcAtZ,
				App->m_pcPointX,App->m_pcPointY,App->m_pcPointZ);
			g_Big.Print(Text1,200,500);
			App->m_Graphics.EndSprite();			
			//世界为场景，而非角色
			//S*X*Y*Z*T 世界变换矩阵相乘的顺序
			D3DXMATRIX mOldWorld,mWorld,mScale,mRot,mTrans;
			D3DXMatrixIdentity( &mWorld );
			D3DXMatrixScaling( &mScale, 20.0f, 20.0f, 20.0f );
			D3DXMatrixRotationY( &mRot, timeGetTime()/2000.0f );
			D3DXMatrixTranslation(&mTrans,-100.0f,100.0f,550.0f);
			D3DXMatrixMultiply(&mWorld,&mScale,&mRot);
			D3DXMatrixMultiply(&mWorld,&mWorld,&mTrans);
		
			App->m_Graphics.GetDeviceCOM()->GetTransform( D3DTS_WORLD, &mOldWorld);
			App->m_Graphics.GetDeviceCOM()->SetTransform( D3DTS_WORLD, &mWorld );
			
			//RenderState for particle system
			App->m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_ZWRITEENABLE,     FALSE );
			//App->m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_ALPHABLENDENABLE,  TRUE );	
			//App->m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );		
			//App->m_Graphics.GetDeviceCOM()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
			App->m_Graphics.EnableAlphaBlending(TRUE,D3DBLEND_ONE,D3DBLEND_ONE);
			//Render the particle system		
			App->m_PartSys.Render();
			App->m_Graphics.GetDeviceCOM()->SetTransform( D3DTS_WORLD, &mOldWorld);
			App->m_Graphics.EnableAlphaBlending(FALSE);
			App->m_Graphics.EndScene();
       }   
    }
	App->m_Graphics.Display();
	if(GameIsOver)
		App->m_StateManager.Push(MenuFrame,App);
}

void CApp::SetCameraForParticleSystem()
{	
}

void CApp::ResetCamera()
{
}


void CApp::RenderStats()//渲染状态条
{	
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(GethWnd(),&pt);
	g_Mid.Print("Z",200,585);
	g_Mid.Print("X",200+45,585);
	g_Mid.Print("C",200+45*2,585);
	g_Mid.Print("V",200+45*3,585);
	g_Mid.Print("B",200+45*4,585);
	g_Mid.Print("N",200+45*5,585);
	g_Mid.Print("M",200+45*6,585);
	g_Mid.Print("1",200+45*7,585);
	g_Mid.Print("2",200+45*8,585);
	g_Mid.Print("3",200+45*9,585);
	m_Graphics.BeginSprite();
	g_Texture0.Blit(150,540,150,518,500,45);
	m_Graphics.EndSprite();
	m_QuickUI.Render(&pt);
	m_Graphics.BeginSprite();
	if(g_PCChar->HealthPoints>0)
	{
		float hpPercent=(float)((int)((float)g_PCChar->HealthPoints/(float)m_CharController.GetHP(g_PCChar)*100.0f)/100.0f);
    	m_HPMPBar.Blit(150,56,0,0,(int)(166*hpPercent),10);
	}
	if(g_PCChar->ManaPoints>0)
	{
	    float mpPercent=(float)((int)((float)g_PCChar->ManaPoints/(float)m_CharController.GetMP(g_PCChar)*100.0f)/100.0f);
	    m_HPMPBar.Blit(156,71,0,10,(int)(166*mpPercent),10);
	}
	static int s=0;
	if(m_Keyboard.GetKeyState(KEY_S)==TRUE)
		s++;
	if(m_Keyboard.GetKeyState(KEY_A)==TRUE)
		s--;
	if(s>100||s<0)
		s=0;
	long Level;
	float energyPercent;
	Level=m_CharController.GetCharacter(0)->Def.Level;
	if(Level>1)
	{
	    energyPercent=(float)(m_CharController.GetCharacter(0)->Def.Experience-LevelUp[Level-2])
			/((float)LevelUp[Level-1]-LevelUp[Level-2]);		    
	}
	else
	{
		energyPercent=(float)(m_CharController.GetCharacter(0)->Def.Experience)
			/(float)LevelUp[Level-1];
	}
	if(energyPercent<=0.25)
	{
		m_Energy.Blit(25,47,0,0,(int)(energyPercent/0.25*52)+8,120);//1
	}
	else if(energyPercent<=0.375)
	{
        m_Energy.Blit(25,107,0,60,(int)((energyPercent-0.25)/0.125*30.0f)+60,60);//2
	}
	else if(energyPercent<=0.625)
	{
		int x=(int)((energyPercent-0.375)/0.25*18.0f)+42;//x增大,y减小
		int t=3600-x*x;
        int y=(int)sqrt((double)t);
		m_Energy.Blit(25,47,0,0,60,120);
		m_Energy.Blit(25,107+y,0,60+y,120,60-y);
	}
	else if(energyPercent<=0.75)
	{		
		int x=60-(int)((energyPercent-0.625)/0.125*10.0f);
        int y=(int)sqrt((double)(3600-x*x));//x减小
		m_Energy.Blit(25,107-y,0,60-y,120,y);//107=47+60
		m_Energy.Blit(25,107,0,60,120,60);//下半圆
	}
	else
	{
		int x=40-(int)((energyPercent-0.75)/0.25*40.0f);
        int y=(int)sqrt((double)(3600-x*x));
    	m_Energy.Blit(25+60+x,47+60-y,x+60,60-y,60,y);//3-1
	    m_Energy.Blit(25,107,0,60,120,60);//3-2
	}
	m_StatsBar.Blit(0,0,0,0,0,0,0.65f,0.4f);
	m_PCPic.Blit(43,65);//直径88
	m_Graphics.EndSprite();
	long mouseX=pt.x;
	long mouseY=pt.y;
	char Text[20];
	long Value;
	if(m_BarterUI.GridBasedUIClosed()==false)
	{
	    if(mouseX>57&&mouseY>84&&mouseY<137&&mouseX<124)
	    {		
		    int n=-4;
		    m_Graphics.BeginSprite();
		    g_Texture0.Blit(pt.x-60,pt.y-80,540,765,140,170);
		    m_Graphics.EndSprite();
		    Value=m_CharController.GetCharacter(0)->Def.Level;
		    sprintf(Text,"%s的等级 %ld",m_CharController.GetCharacter(0)->Def.Name,Value);
		    m_MessageFont.Print(Text,mouseX-48,mouseY+16*(n++));
		    Value=m_CharController.GetAttack(g_PCChar);
		    sprintf(Text,"武力 %ld",Value);
		    m_MessageFont.Print(Text,mouseX-48,mouseY+16*(n++));	
		    Value=m_CharController.GetDefense(g_PCChar);
		    sprintf(Text,"防御 %ld",Value);
		    m_MessageFont.Print(Text,mouseX-48,mouseY+16*(n++));
		    Value=m_CharController.GetAgility(g_PCChar);
		    sprintf(Text,"身法 %ld",Value);
		    m_MessageFont.Print(Text,mouseX-48,mouseY+16*(n++));
		    Value=m_CharController.GetMental(g_PCChar);
		    sprintf(Text,"内力 %ld",Value);
		    m_MessageFont.Print(Text,mouseX-48,mouseY+16*(n++));	
		    Value=m_CharController.GetResistance(g_PCChar);
		    sprintf(Text,"气御 %ld",Value);
		    m_MessageFont.Print(Text,mouseX-48,mouseY+16*(n++));
		    Value=m_CharController.GetToHit(g_PCChar);
		    sprintf(Text,"命中 %ld",Value);
		    m_MessageFont.Print(Text,mouseX-48,mouseY+16*(n++));	
		    Value=m_CharController.GetCharacter(0)->Def.Experience;
		    sprintf(Text,"当前经验 %ld",Value);
		    m_MessageFont.Print(Text,mouseX-48,mouseY+16*(n++));
		    Value=m_CharController.GetCharacter(0)->Def.Level;
		    Value=LevelUp[Value-1]-m_CharController.GetCharacter(0)->Def.Experience;
		    sprintf(Text,"升级所需经验 %ld",Value);
		    m_MessageFont.Print(Text,mouseX-48,mouseY+16*(n++));
	    }
	    else if(mouseX>150&&mouseX<315&&mouseY>55&&mouseY<65)
	    {
		    Value=m_CharController.GetHP(g_PCChar);
		    int curValue=g_PCChar->HealthPoints;
		    sprintf(Text,"生命 %ld/%ld",curValue,Value);
		    m_MessageFont.Print(Text,mouseX-64,mouseY+16);	
	    }
	    else if(mouseX>160&&mouseX<325&&mouseY>72&&mouseY<82)
	    {
		    Value=m_CharController.GetMP(g_PCChar);
		    int curValue=g_PCChar->ManaPoints;
		    sprintf(Text,"真气 %ld/%ld",curValue,Value);
		    m_MessageFont.Print(Text,mouseX-64,mouseY+16);
	    }
	}	
}

void CApp::BarterFrame(void *Ptr, long Purpose)
{ 
	CApp *App = (CApp*)Ptr;
	//如果装备或技能界面未关闭而打开交易
	if(App->m_EquipUI.GridBasedUIClosed()||App->m_SkillUI.GridBasedUIClosed())
		bool1=true;
	else
		bool1=false;
	int ex,ey,sx,sy,bx,by;
	ex=App->m_EquipUI.GetStartX();
	ey=App->m_EquipUI.GetStartY();
	sx=App->m_SkillUI.GetStartX();
	sy=App->m_SkillUI.GetStartY();
	bx=App->m_BarterUI.GetStartX();
	by=App->m_BarterUI.GetStartY();
	//Q/E -> B -> 切换
	if(Purpose == INITPURPOSE) 
	{
		// Load the ICS to use for bartering   
		ICS.Load(g_BarterICS);//2  
		return;
	}
	// Shutdown resources used in bartering
	if(Purpose == SHUTDOWNPURPOSE)
	{
		// Free barter ICS
		ICS.Free();//3   
		return;
	}
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(App->GethWnd(),&pt);
	if(App->m_EquipUI.GridBasedUIClosed())//渲染交易界面的时候如果装备界面也打开
	{//此时如果关闭装备界面，需要弹出两帧，依次为此次BarterFrame,上次的EquipFrame
		//即相当于关闭一次交易界面及一次装备界面，再把交易界面打开
		//关闭装备界面 
		if(App->m_Keyboard.GetKeyState(KEY_Q))
		{
			App->PlaySound(14);	
			App->m_Keyboard.SetLock(KEY_Q, TRUE);	
			App->m_EquipUI.CloseEquipUI();   
			App->m_StateManager.Pop(App);//退出当前渲染帧
			App->m_StateManager.Pop(App);
			App->m_StateManager.Push(App->BarterFrame,App);
			return;
		}
		if((pt.x>=(ex+220)&&pt.y>=ey-10&&pt.x<=(ex+260)&&pt.y<=(ey+30)&&App->m_Mouse.GetButtonState(0))
			&&App->m_EquipUI.GetCurrentPick()==-1&&App->m_SkillUI.GetCurrentPick()==-1
			&&App->m_QuickUI.GetCurrentPick()==-1&&App->m_bFreeLeftButton)
		{
			App->m_Mouse.SetButtonState(0,FALSE);
			App->m_Mouse.SetLock(0,1);
			App->m_bFreeLeftButton=false;
		}
		if((pt.x>=(ex+220)&&pt.y>=ey-10&&pt.x<=(ex+260)&&pt.y<=(ey+30)&&!App->m_Mouse.GetButtonState(0))
			&&App->m_EquipUI.GetCurrentPick()==-1&&App->m_SkillUI.GetCurrentPick()==-1
			&&App->m_QuickUI.GetCurrentPick()==-1)
		{
			App->PlaySound(14);
			App->m_EquipUI.CloseEquipUI();
			App->m_StateManager.Pop(App);//退出当前渲染帧
			App->m_StateManager.Pop(App);
			App->m_StateManager.Push(App->BarterFrame,App);
			App->m_bFreeLeftButton=true;
			return;
		}
	}
	if(App->m_SkillUI.GridBasedUIClosed())
	{
		//关闭技能界面 	
		if(App->m_Keyboard.GetKeyState(KEY_E)==TRUE
			||(pt.x>=(sx+220)&&pt.y>=sy-10&&pt.x<=(sx+260)&&pt.y<=(sy+30)&&App->m_Mouse.GetButtonState(0))	
			&&App->m_EquipUI.GetCurrentPick()==-1&&App->m_SkillUI.GetCurrentPick()==-1	
			&&App->m_QuickUI.GetCurrentPick()==-1)
		{		
			App->PlaySound(14);	
			App->m_Keyboard.SetLock(KEY_E, TRUE);
			App->m_Mouse.SetButtonState(0,FALSE);
			App->m_Mouse.SetLock(0,1);
			App->m_SkillUI.CloseSkillUI();
			App->m_StateManager.Pop(App);//退出当前渲染帧	
			App->m_StateManager.Pop(App);
			App->m_StateManager.Push(App->BarterFrame,App);   
			return;
		}
	}
	//关闭交易界面	
	if(pt.x>(bx+220)&&pt.x<(bx+260)&&pt.y>by-10&&pt.y<(by+30)
		&&App->m_EquipUI.GetCurrentPick()==-1&&App->m_SkillUI.GetCurrentPick()==-1
		&&App->m_QuickUI.GetCurrentPick()==-1)							 	               	
	{
		if(App->m_Mouse.GetButtonState(0)&&App->m_bFreeLeftButton)
		{
			App->m_Mouse.SetButtonState(0,FALSE);
			App->m_Mouse.SetLock(0,1);
			App->m_bFreeLeftButton=false;
		}
		else if(!App->m_Mouse.GetButtonState(0)&&!App->m_bFreeLeftButton)
		{
			App->m_bFreeLeftButton=true;
			App->PlaySound(14);
		    App->m_BarterUI.CloseBarterUI();
		    App->m_StateManager.Pop(App);
		    bool1=false;
		    return;
		}	
	}
	// 渲染商店帧
	App->m_CharController.Update(33);
	App->m_SpellController.Update(33); 
	App->m_Graphics.Clear();
	if(App->m_Graphics.BeginScene() == TRUE)
	{	
		App->RenderFrame(33);//暂停渲染场景、角色等	
		App->RenderStats();
		if(App->GetSkillUI().GetCurrentPick()!=-1||App->GetEquipUI().GetCurrentPick()!=-1)
			App->m_QuickUI.Render(&pt);	
		POINT pt;
		GetCursorPos(&pt);   
		ScreenToClient(App->GethWnd(),&pt);
		App->m_BarterUI.Render(&pt,&ICS);
		if(App->m_EquipUI.GridBasedUIClosed())
			App->m_EquipUI.Render(&pt);		
		if(App->m_SkillUI.GridBasedUIClosed())
			App->m_SkillUI.Render(&pt);
		if(App->GetSkillUI().GetCurrentPick()==-1&&App->GetEquipUI().GetCurrentPick()==-1)	
			App->m_QuickUI.Render(&pt);
		App->m_Graphics.BeginSprite();
		if(!App->GetEquipUI().GridBasedUIClosed()&&!App->GetSkillUI().GridBasedUIClosed()
			&&App->GetQuickUI().GetMouseOnWhichGrid(&pt)==-1)
			g_Mouse1.Blit(pt.x,pt.y);
		//如果Equip/Skill都未打开,使用DefaultCursor,如果有一个打开了,就要根据是否使用标记进行
		else
		{
			if(bUseDefaultCursor)		
				g_Mouse1.Blit(pt.x,pt.y);
		}	
		App->m_Graphics.EndSprite();   
		App->m_Graphics.EndScene();
	}
	App->m_Graphics.Display();
}

void CApp::FirstCGFrame(void *Ptr,long Purpose)
{
	CApp *App=(CApp*)Ptr;
	if(Purpose==INITPURPOSE)
	{
		App->PlayAVI(0);
		return;
	}
	if(Purpose==SHUTDOWNPURPOSE)
	{
		App->StopAVI();
		return;
	}
	if(App->m_Keyboard.GetKeyState(KEY_ESC)==TRUE)
	{
		App->m_AVI.Esc();
		App->m_StateManager.PopAll(App);
		App->m_StateManager.Push(App->MenuFrame,App);
	}
	if(App->m_Keyboard.GetKeyState(KEY_F1)==TRUE)
	{
		App->m_AVI.Fast();
	}
	if(App->m_Keyboard.GetKeyState(KEY_F2)==TRUE)
	{
		App->m_AVI.Slow();
	}
	if(App->m_Keyboard.GetKeyState(KEY_F3)==TRUE)
	{
		App->m_AVI.Normal();
	}
}

void CApp::GameOverFrame(void *Ptr,long Purpose)
{
	CApp *App=(CApp*)Ptr;
	static CTexture gameover;
	if(Purpose==INITPURPOSE)
	{
		gameover.Load(&App->m_Graphics,"..\\Image\\GameOver.png",0xFFFFFFFF,D3DFMT_A8R8G8B8);
		return; 
	}
	if(Purpose==SHUTDOWNPURPOSE)
	{
		gameover.Free();
		return;
	}
	App->StopMP3();
	App->PlayMusic(10);
	//CSolidFadeTransition *SolidFadeTransition = new CSolidFadeTransition(
	//	App->m_Graphics.GetDeviceCOM(),gameover.GetTextureCOM(),D3DXCOLOR(0.6f, 0.7f, 0.0f, 1.0f));  		          					            					     					
	//SolidFadeTransition->SetDuration(2.0f);

	if(App->m_Mouse.GetButtonState(0)==TRUE)
	{
		App->m_Mouse.SetLock(0,TRUE);
		App->m_Mouse.SetButtonState(0,TRUE);
		App->m_StateManager.PopAll(App);
		App->m_StateManager.Push(App->MenuFrame,App);
	}
	App->m_Graphics.Clear();
	App->m_Graphics.BeginScene();
	App->m_Graphics.BeginSprite();
	gameover.Blit(0,0);
	g_Mouse1.Blit(App->m_Mouse.GetXPos(),App->m_Mouse.GetYPos());
	App->m_Graphics.EndSprite();
	App->m_Graphics.EndScene();
	App->m_Graphics.Display();	
}
/*打开装备界面帧的同时可以打开交易界面帧，反之亦然在打开
  先打开Q/E,然后交易,切换Q、E,发现交易和Q/E成为一体
  若关闭交易,再关闭Q/E,则退出程序,则在StatusFrame出现问题
  S:StatusFrame B:BarterFrame
  Push(S)RenderQ Push(B) * Push(S)RenderE
  *处为了在打开E时关闭Q,弹一帧,把B弹出去了,而没有弹出S,即
  没有把Q关掉,反而把B误弹出,致使出现了异常情况,
  而如果在切换Q、E的时候,上一帧是S,就不会出现问题
  于是就出现了不打开B,切换Q、E不会有问题
  先打开B,再打开S(RenderQ/E),进行Q、E切换,也不会有问题,
  因为这样总能保证在切换Q、E时Pop的是S,而不会把B Pop出来,
  此种情况下只有在切换是Pop两帧,再压入B,再压入S
  而非Pop一帧,压入S,或者限定对话时把S关掉(即如果对话且E/Q没关)*/
void CApp::StatusFrame(void *Ptr, long Purpose)//装备界面和武学界面帧
{
	CApp *App = (CApp*)Ptr;
    int ex,ey,sx,sy,bx,by;
    ex=App->m_EquipUI.GetStartX();
    ey=App->m_EquipUI.GetStartY();
    sx=App->m_SkillUI.GetStartX();
    sy=App->m_SkillUI.GetStartY();
    bx=App->m_BarterUI.GetStartX();
    by=App->m_BarterUI.GetStartY();
    sCharacter *NPCPointTo=App->GetCharacterAt(App->m_Mouse.GetXPos(),App->m_Mouse.GetYPos());
    //初始化状态
    if(Purpose == INITPURPOSE) 
    {
        return;
    }
    //目的是关闭则返回
    if(Purpose == SHUTDOWNPURPOSE)
    {
        return;
    }
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(App->GethWnd(),&pt);
    if(App->bPCControl==TRUE&&App->m_Keyboard.GetKeyState(KEY_CTRL)==FALSE)
    {
		long wheelDelta=App->m_Mouse.m_MouseState->lZ;//滚轮的相对偏移
		App->m_CameraAngle+=((float)wheelDelta*(float)33/320.0f);
		App->m_pcAtX=g_PCChar->XPos+(float)cos(App->m_CameraAngle)*CameraHeight;
		App->m_pcAtY=curCameraPosY;
		App->m_pcAtZ=g_PCChar->ZPos-(float)sin(App->m_CameraAngle)*CameraHeight;
		App->m_pcPointX=g_PCChar->XPos;
		App->m_pcPointY=g_PCChar->YPos;
		App->m_pcPointZ=g_PCChar->ZPos;
		App->m_Camera.Point(App->m_pcAtX,App->m_pcAtY,App->m_pcAtZ,
							App->m_pcPointX,App->m_pcPointY,App->m_pcPointZ);
		App->m_Graphics.SetCamera(&App->m_Camera); 
	}
	if(App->m_BarterUI.GridBasedUIClosed())//交易界面没关
    {
        if(pt.x>(bx+230)&&pt.x<(bx+250)&&pt.y>by&&pt.y<(by+20)&&App->m_Mouse.GetButtonState(0)==TRUE
			&&App->m_EquipUI.GetCurrentPick()==-1&&App->m_SkillUI.GetCurrentPick()==-1
	        &&App->m_QuickUI.GetCurrentPick()==-1)	                 
		{
			App->m_Mouse.SetButtonState(0,FALSE);
			App->m_Mouse.SetLock(0,1);
			App->PlaySound(14);
			App->m_BarterUI.CloseBarterUI();
			//如果交易界面没有被关闭，点击关闭，弹出装备界面帧及交易界面帧
			//再压入装备界面帧，以关闭交易界面
			App->m_StateManager.Pop(App);
			App->m_StateManager.Pop(App);	
			App->m_StateManager.Push(App->StatusFrame,App);	
			bool1=false;
			return;
		}
	}
	// 处理一帧
	if(EquipOrSkill&&(App->m_Keyboard.GetKeyState(KEY_Q)==TRUE||(pt.x>=(ex+220)&&
		pt.y>=ey-10&&pt.x<=(ex+260)&&pt.y<=(ey+30)&&App->m_Mouse.GetButtonState(0)))
		&&App->m_EquipUI.GetCurrentPick()==-1&&App->m_SkillUI.GetCurrentPick()==-1
		&&App->m_QuickUI.GetCurrentPick()==-1)
	{
		App->PlaySound(14);
	    App->m_Keyboard.SetLock(KEY_Q, TRUE);
	    App->m_Mouse.SetButtonState(0,FALSE);
	    App->m_Mouse.SetLock(0,1);
	    App->m_EquipUI.CloseEquipUI();
        App->m_StateManager.Pop(App);//退出当前渲染帧
        return; 
	}
	if(!EquipOrSkill&&(App->m_Keyboard.GetKeyState(KEY_E)==TRUE||pt.x>=(sx+220)&&
		pt.y>=sy-10&&pt.x<=(sx+260)&&pt.y<=(sy+30)&&App->m_Mouse.GetButtonState(0))
		&&App->m_EquipUI.GetCurrentPick()==-1&&App->m_SkillUI.GetCurrentPick()==-1
	    &&App->m_QuickUI.GetCurrentPick()==-1)
	{
		App->PlaySound(14);	
		App->m_Keyboard.SetLock(KEY_E, TRUE);
		App->m_Mouse.SetButtonState(0,FALSE);
		App->m_Mouse.SetLock(0,1);
		App->m_SkillUI.CloseSkillUI();
		App->m_StateManager.Pop(App);//退出当前渲染帧
		return;
	}
	// Render the scene
	App->m_CharController.Update(33);
	App->m_SpellController.Update(33);
	App->m_Graphics.Clear();
	if(App->m_Graphics.BeginScene() == TRUE) 
	{  	
		App->RenderFrame(33);  
		App->RenderStats();
    	if(App->GetSkillUI().GetCurrentPick()!=-1||App->GetEquipUI().GetCurrentPick()!=-1)
	    {
		    App->m_QuickUI.Render(&pt);
	    }
	    if(App->m_BarterUI.GridBasedUIClosed())
		    App->m_BarterUI.Render(&pt,&ICS);
	    if(EquipOrSkill)	
		    App->m_EquipUI.Render(&pt);
	    else if(!EquipOrSkill)
		    App->m_SkillUI.Render(&pt);
	    if(App->GetSkillUI().GetCurrentPick()==-1&&App->GetEquipUI().GetCurrentPick()==-1)
	    {
		    App->m_QuickUI.Render(&pt);
	    }
	    App->m_Graphics.BeginSprite();//
	    if(bUseDefaultCursor)
	        g_Mouse1.Blit(pt.x,pt.y);
	    App->m_Graphics.EndSprite();//End()  
		App->m_Graphics.EndScene();  
	}
	App->m_Graphics.Display();
}

D3DXVECTOR3 CApp::ScreenTo3D(int iScreenX,int iScreenY,float f3DY)
{
	D3DXVECTOR3   vPickRayDir;   
    D3DXVECTOR3   vPickRayOrig;   
    D3DXVECTOR3   vOut;   
	D3DXMATRIX    matProj;
    m_Graphics.GetDeviceCOM()->GetTransform(D3DTS_PROJECTION,&matProj);      
    POINT   ptCursor;   
    ptCursor.x=iScreenX;   
    ptCursor.y=iScreenY;     
    //Compute   the   vector   of   the   pick   ray   in   screen   space   
    D3DXVECTOR3   v;   
	long WinWidth,WinHeight;
    WinWidth=m_Graphics.GetWidth();
	WinHeight=m_Graphics.GetHeight();
    v.x = (((2.0f*ptCursor.x)/WinWidth)-1)/matProj._11;   
    v.y = -(((2.0f*ptCursor.y)/WinHeight)-1)/matProj._22;   
    v.z = 1.0f;      
    //Get   the   inverse   view   matrix   
    D3DXMATRIX   matView,m;   
    m_Graphics.GetDeviceCOM()->GetTransform(D3DTS_VIEW,&matView);   
    D3DXMatrixInverse(&m,NULL,&matView);   
    //Transform   the   screen   space   pick   ray   into   3D   space   
    vPickRayDir.x     =   v.x*m._11   +   v.y*m._21   +   v.z*m._31;   
    vPickRayDir.y     =   v.x*m._12   +   v.y*m._22   +   v.z*m._32;   
    vPickRayDir.z     =   v.x*m._13   +   v.y*m._23   +   v.z*m._33;   
    vPickRayOrig.x    =   m._41;   
    vPickRayOrig.y    =   m._42;   
    vPickRayOrig.z    =   m._43;   
    vOut.y =   f3DY;   
    vOut.x =   (f3DY-vPickRayOrig.y)*vPickRayDir.x/vPickRayDir.y + vPickRayOrig.x;   
    vOut.z =   (f3DY-vPickRayOrig.y)*vPickRayDir.z/vPickRayDir.y + vPickRayOrig.z;   
    return   vOut;   
}

long CApp::GetInput()
{
	if(CanGetInput)//根据是否在对话决定是否接受输入
	{
		long Action=0 ;
        if(m_Keyboard.GetKeyState(KEY_W) == TRUE||m_JoyStick.GetButtonState(0)==TRUE)
	        Action |= 1;
        if(m_Keyboard.GetKeyState(KEY_A) == TRUE||m_JoyStick.GetButtonState(1)==TRUE)
            Action |= 2;  
        if(m_Keyboard.GetKeyState(KEY_D) == TRUE||m_JoyStick.GetButtonState(2)==TRUE)
            Action |= 4;
        if(m_Keyboard.GetKeyState(KEY_Z)==TRUE&&m_QuickUI.ItemGrid[0].bEmpty==false)
		{   
			Action |= 8; 
		}
		if(m_Keyboard.GetKeyState(KEY_X)==TRUE&&m_QuickUI.ItemGrid[1].bEmpty==false)
		{ 
			Action |= 16;
		}
		if(m_Keyboard.GetKeyState(KEY_C)==TRUE&&m_QuickUI.ItemGrid[2].bEmpty==false) 
		{ 
			Action |= 32;
		}
		if(m_Keyboard.GetKeyState(KEY_V)==TRUE&&m_QuickUI.ItemGrid[3].bEmpty==false) 
		{ 
			Action |= 64;
		}
		if(m_Keyboard.GetKeyState(KEY_B)==TRUE&&m_QuickUI.ItemGrid[4].bEmpty==false) 
		{  
			Action |= 128;
		}
		if(m_Keyboard.GetKeyState(KEY_N)==TRUE&&m_QuickUI.ItemGrid[5].bEmpty==false)
		{
			Action|=8192;
		}
		if(m_Keyboard.GetKeyState(KEY_M)==TRUE&&m_QuickUI.ItemGrid[6].bEmpty==false)
		{
			Action|=16384;
		}
		if(m_Keyboard.GetKeyState(KEY_1)==TRUE&&m_QuickUI.ItemGrid[7].bEmpty==false)
		{
			Action |=1024;
		}
		if(m_Keyboard.GetKeyState(KEY_2)==TRUE&&m_QuickUI.ItemGrid[8].bEmpty==false)
		{ 
			Action |=2048;
		}
		if(m_Keyboard.GetKeyState(KEY_3)==TRUE&&m_QuickUI.ItemGrid[9].bEmpty==false)
		{
			Action |=4096;
		}
		TargetID=-1;
		long x,y;
		x = m_Mouse.GetXPos();
		y = m_Mouse.GetYPos();
		sCharacter* CharPointed;
		CharPointed=GetCharacterAt(x,y);
		if(CharPointed==NULL)
			TargetID=-1;
		else
	        TargetID=CharPointed->ID;
		//单击鼠标左键，若光标指向NPC/monster，并且没有被界面挡住，并且没有处于选取的状态，对话/攻击
		if(TargetID!=-1&&m_Mouse.GetButtonState(0)&&!m_EquipUI.MouseStopOnGridBasedUI(&m_Mouse)&&m_EquipUI.GetCurrentPick()==-1
			&&!m_SkillUI.MouseStopOnGridBasedUI(&m_Mouse)&&m_SkillUI.GetCurrentPick()==-1	  
			&&!m_BarterUI.MouseStopOnGridBasedUI(&m_Mouse)&&!m_BarterUI.GridBasedUIClosed()	  
			&&!m_QuickUI.MouseStopOnGridBasedUI(&m_Mouse)&&m_QuickUI.GetCurrentPick()==-1)
		{
			Action |=256;
		}
		//行走
		if(TargetID==-1&&m_Mouse.GetButtonState(0)&&!m_EquipUI.MouseStopOnGridBasedUI(&m_Mouse)&&m_EquipUI.GetCurrentPick()==-1
			&&!m_SkillUI.MouseStopOnGridBasedUI(&m_Mouse)&&m_SkillUI.GetCurrentPick()==-1	  
			&&!m_BarterUI.MouseStopOnGridBasedUI(&m_Mouse)&&!m_BarterUI.GridBasedUIClosed()
			&&!m_QuickUI.MouseStopOnGridBasedUI(&m_Mouse)&&m_QuickUI.GetCurrentPick()==-1)
		{
			Action |=512;	
			POINT ptScreen;  	  
			GetCursorPos(&ptScreen);
			ScreenToClient(GethWnd(),&ptScreen);
			vecMouse=ScreenTo3D(ptScreen.x,ptScreen.y,0.0f);
		    //vecMouse=ScreenTo3D(ptScreen.x,ptScreen.y,m_SceneMesh.GetParentMesh()->);  
			charX=m_CharController.GetCharacter(0)->XPos;
			charZ=m_CharController.GetCharacter(0)->ZPos;
		}
		return Action;
    }
    else
	    return 0;
}

CEquipUI CApp::GetEquipUI()
{
	return m_EquipUI;
}

CBarterUI CApp::GetBarterUI()
{
	return m_BarterUI;
}

CSpellController* CApp::GetSpellController()
{
	return &m_SpellController;
}

CSkillUI CApp::GetSkillUI()
{
	return m_SkillUI;
}

CQuickUI CApp::GetQuickUI()
{
	return m_QuickUI;
}

bool CApp::IfThereIsAPick()
{
	if(m_EquipUI.GetCurrentPick()==-1&&m_QuickUI.GetCurrentPick()==-1&&m_SkillUI.GetCurrentPick()==-1)
		return false;
	return true;
}

int CApp::GetCurrentStudySpell()
{
	sCharSkill *SkillPtr=g_PCChar->CharSkill->GetParentSkill();
	while(SkillPtr!=NULL)
	{
		
		if(SkillPtr->Position==25)
			return SkillPtr->Index;
		SkillPtr=SkillPtr->Next;
	}
	return -1;
}

int CApp::FindTheFirstEmptyGridOfSkillUI()
{
	sCharSkill *SkillPtr=g_PCChar->CharSkill->GetParentSkill();
	int Pos[100];
	int Num=0;
	//遍历角色技能清单,对位于技能栏纪录在数组Pos中,Num即技能栏被占用的总数
	while(SkillPtr!=NULL)
	{
		int pos=SkillPtr->Position;
		if(SkillPtr->UpDown==0&&pos>=0&&pos<25)
		{
			Pos[Num]=pos;
			Num++;
		}
		SkillPtr=SkillPtr->Next;
	}		
	int k;
	for(int m=0;m<20;m++)
	{
		k=0;
		for(int n=0;n<Num;n++)
		{
			if(m==Pos[n])
				break;
			else
				k++;
		}
		if(k==Num)			
			return m;
	}
	return -1;
}

//使用UseTime时间将Camera从一个方位移动到另一个方位 
bool CApp::MoveCamera(float beginAtX,float beginAtY,float beginAtZ,		
					  float beginPointX,float beginPointY,float beginPointZ,	
					  float endAtX,float endAtY,float endAtZ,				
					  float endPointX,float endPointY,float endPointZ,		
					  float UseTime)							
{
	if(UseTime>500)
	{
		m_oldAtX=beginAtX;
		m_oldAtY=beginAtY;
		m_oldAtZ=beginAtZ;
		m_oldPointX=beginPointX;
		m_oldPointY=beginPointY;
		m_oldPointZ=beginPointZ;	
		m_TimeAtX=(int)(UseTime/fabs(beginAtX-endAtX));
		m_TimeAtY=(int)(UseTime/fabs(beginAtY-endAtY));
		m_TimeAtZ=(int)(UseTime/fabs(beginAtZ-endAtZ));
		m_TimePointX=(int)(UseTime/fabs(beginPointX-endPointX));
		m_TimePointY=(int)(UseTime/fabs(beginPointY-endPointY));
		m_TimePointZ=(int)(UseTime/fabs(beginPointZ-endPointZ));    
		m_BeginAtX=beginAtX;
		m_BeginAtY=beginAtY;
		m_BeginAtZ=beginAtZ;
		m_BeginPointX=beginPointX;
		m_BeginPointY=beginPointY;
		m_BeginPointZ=beginPointZ;
		m_EndAtX=endAtX;
		m_EndAtY=endAtY;
		m_EndAtZ=endAtZ;
		m_EndPointX=endPointX;
		m_EndPointY=endPointY;
		m_EndPointZ=endPointZ;
		//判断各分量是否需要改变值及改变是增还是减
		if(m_BeginAtX==m_EndAtX)//AtX
		{
			m_ModifyAtX=0;
			bMoveAtX=false;
		}
		else if(m_BeginAtX<m_EndAtX)
		{
			bMoveAtX=true;
			m_ModifyAtX=1;
		}
		else
		{
			bMoveAtX=true;
			m_ModifyAtX=-1;
		}

		if(m_BeginAtY==m_EndAtY)//AtY
		{
			bMoveAtY=false;
			m_ModifyAtY=0;
		}
		else if(m_BeginAtY<m_EndAtY)
		{
			bMoveAtY=true;
			m_ModifyAtY=1;
		}
		else
		{
			bMoveAtY=true;
			m_ModifyAtY=-1;
		}
        if(m_BeginAtZ==m_EndAtZ)//AtZ
		{
			bMoveAtZ=false;
			m_ModifyAtZ=0;
		}
		else if(m_BeginAtZ<m_EndAtZ)
		{
			bMoveAtZ=true;
			m_ModifyAtZ=1;
		}
		else
		{
			bMoveAtZ=true;
			m_ModifyAtZ=-1;
		}
		if(m_BeginPointX==m_EndPointX)//PointX
		{
			bMovePointX=false;
			m_ModifyPointX=0;
		}
		else if(m_BeginPointX<m_EndPointX)
		{
			bMovePointX=true;
			m_ModifyPointX=1;
		}
		else
		{
			bMovePointX=true;
			m_ModifyPointX=-1;
		}
		if(m_BeginPointY==m_EndPointY)//PointY
		{
			bMovePointY=false;
			m_ModifyPointY=0;
		}
		else if(m_BeginPointY<m_EndPointY)
		{
			bMovePointY=true;
			m_ModifyPointY=1;
		}
		else
		{
			bMovePointY=true;
			m_ModifyPointY=-1;
		}
		if(m_BeginPointZ==m_EndPointZ)//PointZ
		{
			bMovePointZ=false;
			m_ModifyPointZ=0;
		}
		else if(m_BeginPointZ<m_EndPointZ)
		{
			bMovePointZ=true;
			m_ModifyPointZ=1;
		}
		else
		{
			bMovePointZ=true;
			m_ModifyPointZ=-1;
		}
		if(bMoveAtX||bMoveAtY||bMoveAtZ||bMovePointX||bMovePointY||bMovePointZ)		
			bMoveCamera=true;
		else
			bMoveCamera=false;
	}
	else
	{
		m_AtX=endAtX;
		m_AtY=endAtY;
		m_AtZ=endAtZ;
		m_PointX=endPointX;
		m_PointY=endPointY;
		m_PointZ=endPointZ;
	}
	return true;
}

bool CApp::ToMoveCamera()
{
	while(1)
	{
		//AtX分量
		if(bMoveAtX)
		{
			if(timeGetTime()>=UpdateTimerAtX+m_TimeAtX)
			{
				m_BeginAtX+=2*m_ModifyAtX;
				UpdateTimerAtX=timeGetTime();
				if(m_BeginAtX>=m_EndAtX-2&&m_BeginAtX<=m_EndAtX+2)
				{
					m_BeginAtX=m_EndAtX;
					m_AtX=m_EndAtX;	
					bMoveAtX=false;
				}
			}
		}
		//AtY分量
		if(bMoveAtY)
		{
			if(timeGetTime()>=UpdateTimerAtY+m_TimeAtY)
			{
				m_BeginAtY+=2*m_ModifyAtY;
				UpdateTimerAtY=timeGetTime();
				if(m_BeginAtY>=m_EndAtY-2&&m_BeginAtY<=m_EndAtY+2)
				{
					m_BeginAtY=m_EndAtY;
					m_AtY=m_EndAtY;	
					bMoveAtY=false;
				}
			}
		}
		//AtZ分量
		if(bMoveAtZ)
		{
			if(timeGetTime()>=UpdateTimerAtZ+m_TimeAtZ)
			{
				m_BeginAtZ+=2*m_ModifyAtZ;
				UpdateTimerAtZ=timeGetTime();
				if(m_BeginAtZ>=m_EndAtZ-2&&m_BeginAtZ<=m_EndAtZ+2)
				{
					m_BeginAtZ=m_EndAtZ;
					m_AtZ=m_EndAtZ;	
					bMoveAtZ=false;
				}
			}
		}
		//PointX分量
		if(bMovePointX)
		{
			if(timeGetTime()>=UpdateTimerPointX+m_TimePointX)
			{
				m_BeginPointX+=2*m_ModifyPointX;
				UpdateTimerPointX=timeGetTime();
				if(m_BeginPointX>=m_EndPointX-2&&m_BeginPointX<=m_EndPointX+2)
				{
					m_BeginPointX=m_EndPointX;
					m_PointX=m_EndPointX;	
					bMovePointX=false;
				}
			}
		}
		//PointY分量
		if(bMovePointY)
		{
			if(timeGetTime()>=UpdateTimerPointY+m_TimePointY)
			{
				m_BeginPointY+=2*m_ModifyPointY;
				UpdateTimerPointY=timeGetTime();
				if(m_BeginPointY>=m_EndPointY-2&&m_BeginPointY<=m_EndPointY+2)
				{
					m_BeginPointY=m_EndPointY;
					m_PointY=m_EndPointY;	
					bMovePointY=false;
				}
			}
		}
		//PointZ分量
		if(bMovePointZ)
		{
			if(timeGetTime()>=UpdateTimerPointZ+m_TimePointZ)
			{
				m_BeginPointZ+=2*m_ModifyPointZ;
				UpdateTimerPointZ=timeGetTime();
				if(m_BeginPointZ>=m_EndPointZ-2&&m_BeginPointZ<=m_EndPointZ+2)
				{
					m_BeginPointZ=m_EndPointZ;
					m_PointZ=m_EndPointZ;	
					bMovePointZ=false;
				}
			}
		}
		if(!bMoveAtX&&!bMoveAtY&&!bMoveAtZ&&!bMovePointX&&!bMovePointY&&!bMovePointZ) 	   
		{
			bMoveCamera=false;
			break;
		}
		else
		{
			bMoveCamera=true;
		}
		m_Camera.Point(m_BeginAtX,m_BeginAtY,m_BeginAtZ,m_BeginPointX,m_BeginPointY,m_BeginPointZ);
		m_Graphics.SetCamera(&m_Camera);
		m_CharController.Update(33);
		m_SpellController.Update(33);
		m_Graphics.Clear();
		if(m_Graphics.BeginScene()==TRUE)
		{
			
			RenderFrame(33);
			m_Graphics.BeginSprite();
			RenderStats();
			m_Graphics.EndSprite();
			m_Graphics.EndScene();
		}
		m_Graphics.Display();
	}
	return true;
}

