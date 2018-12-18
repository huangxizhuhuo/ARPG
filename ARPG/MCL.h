/*--------------------------------------------
Jim Adams 版权所有
2007-2008 SCY 修改
--------------------------------------------*/
#ifndef MCL_H
#define MCL_H

typedef struct sCharacterDefinition
{ 
	// Misc data
    char  Name[32];           // Name of character
    long  Class;              // Class # of character
    long  Money;              // Amount of money
    float Speed;              // Movement speed
    long  MagiCSpells[2];     // Bit flags to mark known spells
    long  MeshNum;            // Mesh/anim # to load
    // Abilities
    long  DEX;            
    long  STR;           
    long  DEF;           
    long  RES;         
    long  ENE;             
    long  HIT;             
    // Attributes
    long  HealthPoints;       // # health points (maximum)
    long  ManaPoints;         // # mana points (maximum)
    long  Level;              // Experience level
    long  Experience;         // Experience points
    // Inventory
    char  ItemFilename[MAX_PATH]; // CharICS filename 
    long  Weapon;             // Equipped Weapon
    long  Armor;              // Equipped Armor
    long  Helmet;        
    long  Boots;
    long  Adorn;      
    // Dropping item data
    long  DropChance;         // % of dropping item when killed
    long  DropItem;           // Item # to drop when killed 
    // Attack/Magic chances and effects
    float Range;              // Attack range
    float ChargeRate;         // Countdown rate to attack
    long  ToAttack;           // Percent to attack
    long  ToMagic;            // Percent to use magic
    long  EffectChance;       // Chance of attack effect occuring
    long  Effects;            // Bit flags of attack effects
} sCharacterDefinition;
#endif
