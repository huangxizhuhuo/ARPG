#ifndef GLOBAL_H
#define GLOBAL_H

#include "../Core/Core_Global.h"

#include "Barrier.h"
#include "CharICS.h"
#include "Chars.h"
#include "Frustum.h"
#include "MCL.h"
#include "MSL.h"
#include "Script.h"
#include "Spell.h"
#include "Trigger.h"
#include "BlackScreen.h"
#include "Talk.h"
#include "Game_Chars.h"
#include "Game_Script.h"
#include "Game_Spells.h"


#include "EquipUI.h"
#include "BarterUI.h"
#include "SkillUI.h"
#include "QuickUI.h"
#include "SaveFile.h"

#include "Particle.h"
#include "ParticleEmitter.h"
#include "WinMain.h"

//#include "SolidFade.h"

//声明全局变量，WinMain.cpp等文件中定义
extern sCharacter *g_PCChar;
extern sCharacter *g_BarterChar;
extern sItem      g_MIL[1024];
extern CTexture   g_Tex1,g_Tex2,g_Tex3,g_Tex4,g_Texture0,g_Texture1,g_Hand,g_Hand1;
extern CGameFont  g_Mid,g_Big,g_Small;
extern bool bUseDefaultCursor;
extern char *g_Cursor[];
extern char *g_HandCursor[];
extern bool GameIsOver;
extern bool EquipOrSkill;
extern bool bool1;
extern int curKillMonster;
extern bool MonstersInLevel;
extern bool CanGetInput;
extern int CameraHeight;

extern bool bKillMonster;
extern int curUseSpellIDInCharSkill;

extern long SkillLevelUp[8];
#endif