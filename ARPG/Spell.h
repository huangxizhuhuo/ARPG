/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#ifndef SPELL_H
#define SPELL_H

// Character references
struct sCharacter;
class  CCharacterController;

#define NUM_SPELL_DEFINITIONS   64

enum SpellEffects 
{ 
	ALTER_HEALTH = 0,
    ALTER_MANA,
    CURE_AILMENT,
    CAUSE_AILMENT,
    RAISE_DEAD,
    INSTANT_KILL,
    DISPEL_MAGIC,
    TELEPORT
};

enum SpellTargets 
{
    TARGET_SINGLE = 0,
    TARGET_SELF,
    TARGET_AREA
};

enum AnimPositions 
{
	POSITION_NONE = 0,
    POSITION_CASTER,
    POSITION_TOTARGET,
    POSITION_TOCASTER,
    POSITION_TARGET,
    POSITION_SCALE
};

typedef struct sSpellMeshList
{ 
	char       Filename[MAX_PATH];  // Filename of mesh/anim
    long       Count;               // # Spells using mesh
    CMesh      Mesh;                // Mesh object
    CAnimation Animation;           // Animation object
    // Construct and destructor to prepare and release data
    sSpellMeshList()  { Count = 0;                     } 
    ~sSpellMeshList() { Mesh.Free(); Animation.Free(); }
} sSpellMeshList; 

typedef struct sSpellTracker
{
	long  SpellNum;                   // Spell #

    sCharacter *Caster;               // Character casting spell
    long  Type;                       // Character type to affect

    long  CurrentAnimation;           // Animation: 0-2
    float SourceX, SourceY, SourceZ;  // Source coordinates
    float TargetX, TargetY, TargetZ;  // Target coordinates

    float XPos, YPos, ZPos;           // Current coordinates
    float XAdd, YAdd, ZAdd;           // Movement values
    float Distance;                   // Distance to target

    union 
	{ 
		float Speed;                    // Speed of movement  
		long  Time;                     // Timer to continue 
	};

    CGameObject Object;                   // Graphics object
  
    sSpellTracker *Prev, *Next;       // Linked list

    sSpellTracker()  { Caster = NULL; Prev = Next = NULL; }
    ~sSpellTracker() { delete Next;                       }
} sSpellTracker;

class CSpellController
{
private:
    CGraphics  *m_Graphics;         // Parent graphics object
    CFrustum   *m_Frustum;          // Viewing frustum

    sSpell m_Spells[NUM_SPELL_DEFINITIONS];  // Spell data

    sSpellTracker *m_SpellParent;   // List of active spells

    long            m_NumMeshes;    // # meshes in use
    sSpellMeshList *m_Meshes;       // Meshes list

    char m_TexturePath[MAX_PATH];   // Mesh texture path

    CCharacterController *m_Chars;  // Character controller

    // Setup the mesh movement
    BOOL SetAnimData(sSpellTracker *SpellPtr, long Num);

    // Function to overide for playing sounds
    virtual BOOL SpellSound(long Num) { return TRUE; }

public:
    CSpellController();   // Constructor
    ~CSpellController();  // Destructor

    // Functions to init/shutdown controller class
    BOOL Init(CGraphics *Graphics, char *DefinitionFile,long NumSpellMeshes, 
		char **MeshNames,char *TexturePath,CCharacterController *Controller);                                                                                   
    BOOL Shutdown();
    // Free class
    BOOL Free();

    sSpell *GetSpell(long SpellNum);

    // Add a spell to the list
    BOOL Add(long SpellNum,sCharacter *Caster, long TargetType,float SourceX,  
		float SourceY,float SourceZ, float TargetX, float TargetY, float TargetZ);                                   
                                                  
    // Update all spells based on elapsed time
    BOOL Update(long Elapsed);

    // Render all spell meshes within viewing frustum
    BOOL Render(CFrustum *Frustum=NULL, float ZDistance = 0.0f);
};

#endif
