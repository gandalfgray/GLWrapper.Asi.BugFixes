
template <typename _Type> struct H3Array
{
 BOOL _init; // +0 useless
 _Type* Data; // +4 the first item, also start of list
 _Type* EndData; // +8 the end of last item
 _Type* EndMemory; // +12 end of allocated memory
};

typedef DWORD BitMaskDword;

struct H3String;
struct H3Manager;
struct H3Army;
struct H3Artifact;
struct H3Town;
struct H3HeroClass;
struct H3HeroVisited;
struct H3HeroFlags;
struct H3Hero;
struct H3CreatureInfo;
struct H3CombatMonsterSpellsData;
struct H3CombatMonster;

#define o_H3Main (*(H3Main**)aH3Main)
#define o_H3HeroClass (*(H3HeroClass**)aH3HeroClass)
#define o_Market_Hero (*(H3Hero**)aMarketHero)
#define o_Market_BackpackIndexOfFirstSlot (char)(*(INT32*)aMarketBackpackIndexOfFirstSlot)
#define o_Market_SelectedSlotIndex *(INT32*)aMarketSelectedSlotIndex
#define o_Market_SelectedBackpackSlotIndex (o_Market_SelectedSlotIndex - 18)

struct H3String
{
 BOOL _init; // useless
 PCHAR str;
 INT32 length;
 INT32 size;
};

	struct H3Manager
	{
  INT8 unk1[56];
 };

struct H3Army
{
 INT32 type[7];
 INT32 count[7];
};

struct H3Artifact
{
 INT32 type;
 INT32 subtype; // used for spell scrolls, otherwise -1
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
 BitMaskDword built_bits[2]; // +336
 BitMaskDword built_bits2[2]; // +344
 BitMaskDword buildable_bits[2]; // +352   
};

// Read from HCTRAITS.txt
// size 64 (0x40)
struct H3HeroClass
{
 INT32 town_type;
 PCHAR class_name;
 FLOAT agression;
 BYTE attack_start;
 BYTE defense_start;
 BYTE power_start;
 BYTE knowl_start;
 BYTE attack_early_prob;
 BYTE defense_early_prob;
 BYTE power_early_prob;
 BYTE knowl_early_prob;
 BYTE attack_mature_prob;
 BYTE defense_mature_prob;
 BYTE power_mature_prob;
 BYTE knowl_mature_prob;
 BYTE sec_skill_prob[28];
 BYTE town_tavern_prob[9];
 BYTE _align[3];
};

// visited objects on map
struct H3HeroVisited
{
 BitMaskDword learningStones;  // 0x57
 BitMaskDword marlettoTower; // 0x5B
 BitMaskDword gardenRevelation;  //0x5F
 BitMaskDword mercenaryCamp; 	// 0x63
 BitMaskDword starAxis;		// 0x67
 BitMaskDword treeKnowldge;  // 0x6B
 BitMaskDword libraryEnlightenment; // 0x6F
 BitMaskDword arena; 		// 0x73
 BitMaskDword schoolMagic; // 0x77
 BitMaskDword schoolWar; 	// 0x7B
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
 BYTE flags_unk; // +72
 DWORD movement_points_max; // +73
 DWORD movement_points; // +77
 DWORD experience; // +81
 WORD level; // +85
 H3HeroVisited visited_objects; // +87
 INT8 unk4[16]; // +127
 BYTE skill_tree_seed; // +143
 INT8 unk5; // +144
 H3Army army; // +145
 BYTE second_skill[28]; // +201
 BYTE second_skill_show_position[28]; // +229
 DWORD second_skill_count; // +257
 BitMaskDword temp_flags; // + 261	
 FLOAT ai_experience_effectivness; // +265
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

struct H3CreatureInfo
{
 INT32 town; // 0
 INT32 level; // +4
 PCHAR sound_name; // +8
 PCHAR def_name; // +12
 BitMaskDword flags;  // +16
 PCHAR name_single; // +20
 PCHAR name_plural; // + 24
 PCHAR description; // +28
 INT32 cost_wood; // +32
 INT32 cost_mercury; // + 36
 INT32 cost_ore; // + 40
 INT32 cost_sulfur; // +44
 INT32 cost_crystal; // +48
 INT32 cost_jems; // + 52
 INT32 cost_gold; // +56
 INT32 fight_value; // +60
 INT32 AI_value; // +64
 INT32 grow; // + 68
 INT32 horde_grow; // +72
 INT32 hit_points; // + 76
 INT32 speed; // + 80
 INT32 attack; // +84
 INT32 defence; // +88
 INT32 damage_min; // +92
 INT32 damage_max; // +96
 INT32 shots_count; // +100
 INT32 spell_charge; // +104
 INT32 advmap_min; // +108
 INT32 advmap_max; // +112
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
 H3Array<H3CombatMonster*> dendroid_binder;  // +4F4 which dendroids have binded the current target (used for animation requirement)
 H3Array<H3CombatMonster*> dendroid_binds;   // +504 a list of H3CombatMonsters binded by this dendroid
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
 INT32 hex_index; 		   // 0x38 position on battlefield
 INT32 animation_group;           // 0x3C
 INT32 animation_frame;      // 0x40
 BOOL orient_to_right; // 0x44 left or right
 INT8 unk2[4];
 DWORD count_current;          // 0x4C the number of creatures that are currently alive
 DWORD count_before_attack; 		// 0x50
 INT8 unk3[4];           // 0x54
 INT32 health_lost; 		// 0x58 the number of lost hitpoints of top creature in stack
 INT32 army_slot_index; // 0x5C ?reference to position on side?
 DWORD count_before_battle; 	// 0x60 the number of creatures in this stack to compare against resurrection
 INT8 unk4[4];
 DWORD anim_value;
 INT32 health_maximum; 		// 0x6C maximum hit points
 BOOL is_lucky;      		// 0x70	
 H3CreatureInfo info; 		// 0x74 a copy of H3CreatureInfo using combat values in some places
 INT8 unk5[4];               // 0xE8
 INT32 spell_to_apply; 		// 0xEC set in After-Hit spell subroutine 0x440220
 INT8 unk6[4];
 INT32 side_bf; 		// 0xF4 left or right
 INT32 index_on_side; // 0xF8 reference to position on side
 UINT32 last_animation_time; // 0xFC
 INT32 yOffset; 		// 0x100
 INT32 xOffset;      		// 0x104
 INT8 unk7[8];               // 0x108
 INT8 H3MonsterAnimation[84]; 		// 0x110 from cranim
 INT32 def;               		// 0x164
 INT32 def_shoot; 		// 0x168
 INT8 unk8[4];          // 0x16C
 UINT32 sound_move; 		// 0x170
 UINT32 sound_attack; 		// 0x174
 UINT32 sound_get_hit; 		// 0x178
 UINT32 sound_shoot; 		// 0x17C
 UINT32 sound_killed; 		// 0x180
 UINT32 sound_defend;     // 0x184
 UINT32 sound_extra1;     // 0x188
 UINT32 sound_extra2;     // 0x18C
 INT8 unk9[4];
 INT32 count_active_spells; 		// 0x194 the number of spells currently active
 INT32 spell_duration[81]; 		// 0x198 the remaining number of turns of any spells
 INT32 spell_sskill_level[81]; 		// 0x2DC the secondary skill level of applied spells
 INT8 unk10[52];
 INT32 count_retaliations; 		// 0x454 number of retaliations left
 H3CombatMonsterSpellsData spells_data; // 0x458 information about some spell effects
};

	struct H3TownManager : public H3Manager
	{
	 H3Town* town; // +0x38 current town structure
  INT8 unk1[224];
  H3GarrisonBar* bar_up; // +0x11C
  H3GarrisonBar* bar_down; // +0x120
  H3GarrisonBar* bar_selected; //+0x124
  INT32 slot_index_bar_selected; // +0x128
  H3GarrisonBar* bar_source; //+0x12C
  INT32 slot_index_bar_source; // +0x130
  H3GarrisonBar* bar_dest; //+0x134
  INT32 slot_index_bar_dest; // +0x138
  INT8 unk1[100]; // +0x13C
  BitMaskDword buildings[2]; //+0x1A0
  INT8 unk2[48];
 };

struct H3Main
{
 INT8 unk1[321488]; 
};

