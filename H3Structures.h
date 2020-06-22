
template <typename _Type> struct H3Array
{
 BOOL _init; // +0 useless
 _Type* Data; // +4 the first item, also start of list
 _Type* EndData; // +8 the end of last item
 _Type* EndMemory; // +12 end of allocated memory
};

struct H3String;
struct H3Army;
struct H3Artifact;
struct H3Town;
struct H3HeroProperties;
struct H3Hero;
struct H3CombatMonsterSpellsData;
struct H3CombatMonster;

struct H3String
{
 BOOL _init; // useless
 PCHAR str;
 INT32 length;
 INT32 size;
};

struct H3Army
{
 INT32 type[7];
 INT32 count[7];
};

struct H3Artifact
{
 INT32 id;
 INT32 mod;
};

struct H3Town
{
 INT8 id; // +0
 INT8 owner_id; // +1
 BOOLEAN built_this_turn; // +2
 INT8 unk1; // +3
 INT8 type; // +4
 BYTE x; // +5
 BYTE y; // +6
 BYTE z; // +7
 BYTE placed_boat_x; // +8
 BYTE placed_boat_y; // +9
 INT8 unk2[2]; // +10
 INT32 garrison_hero; // +12
 INT32 visiting_hero; // +16
 INT8 mag_level; // +20
 INT8 unk3; // +21
 WORD recruitable_creatures[2][7]; // +22
 INT8 unk4; // +50
 BOOLEAN is_mana_vortex_unused; // +51
 INT8 unk5[16]; // +52
 DWORD spells[5][6]; // +68
 BOOLEAN magic_guild[5]; // +188
 INT8 unk6[3]; // +193
 H3String name; // +196
 INT8 unk7[12]; // +212
 H3Army guards; // +224
 H3Army guards_unused; // +280
 DWORD built_bits[2]; // +336
 DWORD built_bits2[2]; // +344
 DWORD buildable_bits[2]; // +352   
};

// Read from HCTRAITS.txt
// size 64 (0x40)
struct H3HeroProperties
{
 INT32 town_type;
 PCHAR class_name;
 FLOAT agression;
 BYTE attack_start;
 BYTE defense_start;
 BYTE power_start;
 BYTE knowl_start;
 BYTE attack_prob_to9Lvl;
 BYTE defense_prob_to9Lvl;
 BYTE power_prob_to9Lvl;
 BYTE knowl_prob_to9Lvl;
 BYTE attack_prob_after10Lvl;
 BYTE defense_prob_after10Lvl;
 BYTE power_prob_after10Lvl;
 BYTE knowl_prob_after10Lvl;
 BYTE sec_skill_prob[28];
 BYTE town_tavern_prob[9];
 INT8 unk1[3];
};

struct H3Hero
{
 INT16 x; // +0
 INT16 y; // +2
 INT16 z; // +4
 BOOLEAN is_visible; // +6
 DWORD xyz; // +7
 BOOLEAN is_object_under; // +11
 INT32 object_type_under; // +12
 DWORD object_flags_under; // +16
 DWORD object_under_setup; // +20
 WORD spell_points; // +24
 DWORD id; // +26
 DWORD unk1; // +30
 INT8 owner_id; // +34
 CHAR name[13]; // +35
 DWORD hero_class; // +48
 BYTE pic; // +52
 DWORD dest_x; // +53
 DWORD dest_y; // +57
 DWORD dest_z; // +61
 INT8 unk2[3]; // +65
 BYTE patrol_x; // +68
 BYTE patrol_y; // +69
 BYTE patrol_radius; // +70
 INT8 unk3; // +71
 BYTE flags; // +72
 DWORD movement_points_max; // +73
 DWORD movement_points; // +77
 DWORD experience; // +81
 WORD level; // +85
 DWORD visited_objects[10]; // +87
 INT8 unk4[16]; // +127
 BYTE skill_tree_seed; // +143
 INT8 unk5; // +144
 H3Army army; // +145
 BYTE second_skill[28]; // +201
 BYTE second_skill_show_position[28]; // +229
 DWORD second_skill_count; // +257
 DWORD temp_mod_flags; // + 261	
 FLOAT ai_experience_effectivness;
 BYTE dimdoor_cast_count; // +269
 INT32 disguise_power; // +270
 INT32 fly_power; // +274
 INT32 waterwalk_power; // +278
 INT8 morale; // +282
 INT8 luck; // +283
 BOOLEAN is_sleeping; // +284
 INT8 unk6[12]; // +285
 INT32 vision_power; // +297
 H3Artifact body_art[19]; // +301
 BYTE free_slots_count; // +453
 BYTE locked_slot[14]; // +454
 H3Artifact backpack_art[64]; // +468
 BYTE backpack_arts_count; // +980
 DWORD gender; // +981
 BOOLEAN has_custom_biography; // +985
 H3String biography; // +986
 BYTE learned_spell[70]; // +1002
 BYTE available_spell[70]; // +1072
 BYTE attack; // +1142
 BYTE defence; // +1143
 BYTE power; // +1144
 BYTE knowledge; // +1145
 DWORD ai_agressivness; // Agressivness + 1146
 DWORD ai_power_effectivness; // PowerEffectivness +1150
 DWORD ai_duration_effectivness; // DurationEffectivness +1154
 DWORD ai_knowl_effectivness; // KnowledgeEffectivness +1158
 DWORD ai_dmana_effectivness; // DoubleManaEffectivness +1162
 DWORD ai_mana_effectivness; // ManaEffectivness +1166
};

// a substructure of H3CombatMonster related to spells
// size 1352 (0x548)
struct H3CombatMonsterSpellsData
{
 INT32 bless_damage;            // 0x458
 INT32 curse_damage;            // 0x45C
 INT32 anti_magic;              // 0x460
 INT32 bloodlust_effect;        // 0x464
 INT32 precision_effect;        // 0x468
 INT32 weakness_effect;         // 0x46C
 INT32 stone_skin_effect;       // 0x470
 INT32 unknown13;               // 0x474
 INT32 prayer_effect;           // 0x478
 INT32 mirth_effect;            // 0x47C
 INT32 sorrow_effect;           // 0x480
 INT32 fortune_effect;          // 0x484
 INT32 misfortune_effect;       // 0x488
 INT32 slayer_type;             // 0x48C - called KING_1/2/3
 INT32 unk1;                    // 0x490 - Max traversed cells before hitting?
 INT32 counterstrike_effect;    // 0x494
 FLOAT frenzyMultiplier;        // 0x498
 INT32 blind_effect;            // 0x49C - for calculating damage retaliation damage?
 FLOAT fire_shield_effect;      // 0x4A0
 INT32 unk2;                    // 0x4A4
 FLOAT protection_air_effect;   // 0x4A8 - in % as below
 FLOAT protection_fire_effect;  // 0x4AC
 FLOAT protection_water_effect; // 0x4B0
 FLOAT protection_earth_effect; // 0x4B4
 INT32 shield_effect;           // 0x4B8
 INT32 air_shield_effect;       // 0x4BC
 INT8 blinded;                  // 0x4C0 - to reduce damage?
 INT8 paralyzed;                // 0x4C1 - to reduce damage?
 INT8 unk3[2];                  // 0x4C2-0x4C3
 INT32 forgetfulness_level;     // 0x4C4
 FLOAT slow_effect;             // 0x4C8
 INT32 haste_effect;            // 0x4CC - value added/removed
 INT32 disease_attack_effect;   // 0x4D0
 INT32 disease_defense_effect;  // 0x4D4
 INT8 unk4[8];                  // 0x4D8-0x4DC
 INT32 faerie_dragon_spell;     // 0x4E0
 INT32 magic_mirror_effect;     // 0x4E4
 INT32 morale;                  // 0x4E8
 INT32 luck;                    // 0x4EC
 INT8 unk5[4];                  // 0x4F0
 H3Array<H3CombatMonster*> dendroidBinder;  // +4F4 which dendroids have binded the current target (used for animation requirement)
 H3Array<H3CombatMonster*> dendroidBinds;   // +504 a list of H3CombatMonsters binded by this dendroid
 INT8 unk6[20];                 // 0x514
 INT32 Hypnotize_528;           // 0x528
 INT32 Hypnotize_52C;           // 0x52C
 INT8 unk7[24];                 // 0x530
};

// monster on battlefield
struct H3CombatMonster
{
 INT8 unk1[52];             // 0
 INT32 type; 		       // 0x34
 INT32 position; 		   // 0x38 position on battlefield
 INT32 animation;           // 0x3C
 INT32 animationFrame;      // 0x40
 INT32 secondHexOrientation; // 0x44 left or right
 INT8 unk2[4];
 INT32 numberAlive;          // 0x4C the number of creatures that are currently alive
 INT32 previousNumber; 		// 0x50
 INT8 unk3[4];           // 0x54
 INT32 healthLost; 		// 0x58 the number of lost hitpoints of top creature in stack
 INT32 slotIndex; // 0x5C ?reference to position on side?
 INT32 numberAtStart; 	// 0x60 the number of creatures in this stack to compare against resurrection
 INT8 unk4[8];
 INT32 baseHP; 		// 0x6C maximum hit points
 INT32 isLucky;      		// 0x70	
 INT8 H3CreatureInformation[116]; 		// 0x74 a copy of H3CreatureInformation using combat values in some places
 INT8 unk5[4];               // 0xE8
 INT32 spellToApply; 		// 0xEC set in After-Hit spell subroutine 0x440220
 INT8 unk6[4];
 INT32 side; 		// 0xF4 left or right
 INT32 sideIndex; // 0xF8 reference to position on side
 UINT32 last_animation_time; // 0xFC
 INT32 yOffset; 		// 0x100
 INT32 xOffset;      		// 0x104
 INT8 unk7[8];               // 0x108
 INT8 H3MonsterAnimation[84]; 		// 0x110 from cranim
 INT32 def;               		// 0x164
 INT32 shootingDef; 		// 0x168
 INT8 unk8[4];          // 0x16C
 UINT32 moveSound; 		// 0x170
 UINT32 attackSound; 		// 0x174
 UINT32 getHitSound; 		// 0x178
 UINT32 shotSound; 		// 0x17C
 UINT32 deathSound; 		// 0x180
 UINT32 defendSound;     // 0x184
 UINT32 extraSound1;     // 0x188
 UINT32 extraSound2;     // 0x18C
 INT8 unk9[4];
 INT32 activeSpellsNumber; 		// 0x194 the number of spells currently active
 INT32 activeSpellsDuration[81]; 		// 0x198 the remaining number of turns of any spells
 INT32 activeSpellsLevel[81]; 		// 0x2DC the secondary skill level of applied spells
 INT8 unk10[52];
 INT32 retaliations; 		// 0x454 number of retaliations left
 H3CombatMonsterSpellsData spellsData; // 0x458 information about some spell effects
};
