
struct H3BukaCompleteCreatureInfo
{
 INT32 town; // 0
 INT32 level; // +4
 PCHAR sound_name; // +8
 PCHAR def_name; // +12
 BitMaskDword flags;  // +16
 PCHAR name_single; // +20
 PCHAR name_plural; // + 24
 PCHAR name_plural2; // + 24
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

struct H3BukaCompleteCombatMonster
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
 H3BukaCompleteCreatureInfo info; 		// 0x74 a copy of H3CreatureInfo using combat values in some places
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
