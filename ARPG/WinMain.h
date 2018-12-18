#ifndef WINMAIN_H
#define WINMAIN_H

class CApp : public CApplication
{
	friend class CSpells;//允许CSpells类访问CApp类的私有成员(友元类)
    friend class CChars;
    friend class CGameScript; 
private:
    // Graphics device, camera, and font
    CGraphics       m_Graphics;
    CCamera         m_Camera;
    CGameFont           m_Font;
    // Input devices
    CInput          m_Input;
    CInputDevice    m_Keyboard;
    CInputDevice    m_Mouse;
	CInputDevice    m_JoyStick;
    // Sound device and channels
    CSound          m_Sound;
    CSoundChannel   m_SoundChannel;
   // CMusicChannel   m_MusicChannel;
    CSoundData      m_SoundData;
    // The scene textures, mesh, and object
    long            m_SceneNum;
    CTexture        m_SceneTextures[6];
    CMesh           m_SceneMesh;
    CGameObject         m_SceneObject;
    // Character and spell controllers
    CChars          m_CharController;
    CSpells         m_SpellController;
	CTexture        m_StatsBar;
	CTexture        m_HPMPBar;
	CTexture        m_PCPic;
	CTexture        m_Energy;
	CGameFont           m_MessageFont;
    // A script parser object
    CGameScript     m_Script;
    // Map to teleport to on next frame (-1 = none)
    long            m_TeleportMap;
    // Flag if monsters during last frame
    BOOL            m_MonstersLastFrame;
    // Combat booty to reward at end of combat
    long            m_CombatExp;
    long            m_CombatMoney;
    long            m_CombatItems[1024];
    // The trigger and barrier classes
    CTrigger        m_Trigger;
    CBarrier        m_Barrier;
	CEquipUI        m_EquipUI;
	CBarterUI       m_BarterUI;
	CSkillUI        m_SkillUI;
	CQuickUI        m_QuickUI;
	//CTalk           m_Talk;
	CTexture        m_TalkPic,m_Black;
	CTexture		m_Head;
	CSaveFile       m_SaveFile;
	CBlackScreen    m_BlackScreen;
	CParticleEmitter m_PartSys;// The Particle System
	CStateManager m_StateManager;
	CAVI m_AVI;
	CMP3 m_MP3;

    // Intersection functions
    float GetHeightBelow(float XPos, float YPos, float ZPos);
    BOOL  CheckIntersect(float XStart,float YStart,float ZStart,float XEnd,float YEnd,float ZEnd,float *Length);                                                 
    // Read in a text floating point number
    float GetNextFloat(FILE *fp);
	long  GetNextLong(FILE *fp);
    // Setup to teleport
    BOOL SetupTeleport(long Level, float XPos, float YPos, float ZPos);                                                  
    // Setup to barter
    BOOL SetupBarter(sCharacter *Character, char *ICSFilename);
    // Functions load and free 3Din2D background images
    BOOL LoadLevel(long Num);
    BOOL FreeLevel();
    // Process win-game scenerio
    BOOL WinGame();
    // Called at start and end of combat
    BOOL StartOfCombat();
    BOOL EndOfCombat();
    // Function to render a single game frame w/out updates
    // and using elapsed animation timer.
    BOOL RenderFrame(long Elapsed=0);//Elapsed
    // Check if last point on route has been reached
    BOOL LastPointReached(sCharacter *Character);
    void RenderStats();//用于GameFrame
	D3DXVECTOR3 ScreenTo3D(int iScreenX,int iScreenY,float f3DY);
	long GetInput();
    static void MenuFrame(void *Ptr, long Purpose);//static表示不能在实例化的类对象中调用,只能用类直接调用. 
    static void GameFrame(void *Ptr, long Purpose);
    static void StatusFrame(void *Ptr, long Purpose);
    static void BarterFrame(void *Ptr, long Purpose);
	static void SkillFrame(void *Ptr,long Purpose);
	static void MinorMenuFrame(void *Ptr,long Purpose);
	static void EnsureFrame(void *Ptr,long Purpose);
	static void GameOverFrame(void *Ptr,long Purpose);
	static void FirstCGFrame(void *Ptr,long Purpose);
	
public:
    CApp();
	bool m_bFreeLeftButton;
	
	long  TargetID;
	D3DXVECTOR3 vecMouse;//每次按下鼠标左键纪录之坐标
	float charX,charZ;
	BOOL  bPCControl;
	//PC控制角色时Camera所在位置及其所指向的位置
	float m_pcAtX,m_pcAtY,m_pcAtZ,m_pcPointX,m_pcPointY,m_pcPointZ;
	//剧情推进,角色不受PC控制时Camera所在位置及其所指向的位置
	float m_AtX,m_AtY,m_AtZ,m_PointX,m_PointZ,m_PointY;
	//保存旧的Camera方位
	float m_oldAtX,m_oldAtY,m_oldAtZ,m_oldPointX,m_oldPointY,m_oldPointZ;
	//移动每一像素各分量所需的时间
	int m_TimeAtX,m_TimeAtY,m_TimeAtZ,m_TimePointX,m_TimePointY,m_TimePointZ;
	float m_BeginAtX,m_BeginAtY,m_BeginAtZ,m_BeginPointX,m_BeginPointY,m_BeginPointZ;
	float m_EndAtX,m_EndAtY,m_EndAtZ,m_EndPointX,m_EndPointY,m_EndPointZ;
	//是否需要移动各分量
	bool bMoveAtX,bMoveAtY,bMoveAtZ,bMovePointX,bMovePointY,bMovePointZ;
	//各分量移动时的移动方向,分为正向、负向(1,-1)
	int m_ModifyAtX,m_ModifyAtY,m_ModifyAtZ,m_ModifyPointX,m_ModifyPointY,m_ModifyPointZ;
	float CameraRotX,CameraRotY,CameraRotZ;
	bool bMoveCamera;
	bool ToMoveCamera();
	DWORD UpdateTimerAtX,UpdateTimerAtY,UpdateTimerAtZ,UpdateTimerPointX,UpdateTimerPointY,UpdateTimerPointZ;
	float m_CameraAngle;

    // Overridden functions
    BOOL Init();
    BOOL Shutdown();
    BOOL Frame();
	CChars* GetCharController();//获得角色控制对象指针
	CEquipUI GetEquipUI();
	 // Play a sound or music
    BOOL PlaySound(long Num);
    BOOL PlayMusic(long Num);
    BOOL StopMusic();
	bool PlayMP3(long Num);
	bool StopMP3();
	bool PlayAVI(long Num);
	bool StopAVI();
	CBarterUI GetBarterUI();
	CSpellController* GetSpellController();
	CSkillUI  GetSkillUI();
	CQuickUI  GetQuickUI();
	bool IfThereIsAPick();
	int  GetCurrentStudySpell();
	int FindTheFirstEmptyGridOfSkillUI();
	// Get the character where mouse is pointed
    sCharacter *GetCharacterAt(long XPos, long YPos);
	bool  MoveCamera(float beginAtX,float beginAtY,float beginAtZ,		
					 float beginPointX,float beginPointY,float beginPointZ,	
					 float endAtX,     float endAtY,     float endAtZ,				
					 float endPointX,  float endPointY,  float endPointZ,		
					 float UseTime);
	void ResetCamera();
	void SetCameraForParticleSystem();
};
#endif
