
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

// * the manager of the battlefield
struct H3BukaCompleteCombatManager : public H3Manager
{
 INT8 unk1[4];
 INT32 action; 		// * +3C
 INT32 actionParameter; 		// * +40
 INT32 actionTarget; 		// * +44
 INT32 actionParameter2; 		// * +48
 INT8 accessibleSquares[187]; 		// * +4C
 UINT8 accessibleSquares2[187];
 INT8 unk2[2];
 INT8 H3CombatSquare[187][112]; 		// * +1C4
 INT32 landType; 		// * +5394
 INT8 unk3[8];
 INT32 absoluteObstacleId; 		// * +53A0
 INT32 siegeKind; 		// * +53A4
 BOOL hasMoat; 		// * +53A8
 INT8 unk4[4];
 void* drawBuffer; 		// * +53B0
 INT8 unk5[4];
 BOOL doNotDrawShade; 		// * +53B8
 void* mapitem; 		// * +53BC 		// * H3MapItem where combat is taking place
 INT32 specialTerrain; 		// * +53C0 		// * special terrain type used
 BOOLEAN antiMagicGarrison; 		// * +53C4
 BOOLEAN creatureBank; 		// * +53C5
 BOOLEAN boatCombat; 		// * +53C6
 INT8 unk6;
 H3Town* town; 		// * +53C8 		// * town structure where combat is taking place
 H3Hero* hero[2]; 		// * +53CC 		// * hero structures from each side
 INT32 heroSpellPower[2]; 		// * +53D4 		// * spell power from each side
 INT8 unk7[8];
 UINT32 HeroAnimation[2]; 		// * +53E4 		// * current group
 UINT32 HeroAnimationFrame[2]; 		// * +53EC
 INT8 unk8[16];
 void* heroDefLoaded[2]; 		// * +5404
 void* heroFlagLoaded[2]; 		// * +540C
 INT32 heroFlagFrame[2]; 		// * +5414
 INT8 heroUpdateRect[2][16]; 		// * +541C
 INT8 heroFlagUpdateRect[2][16]; 		// * +543C
 H3Array<INT32> eagleEyeSpells[2]; 		// * +545C 		// * eagle eye 2x vector
 INT8 unk9[40]; 		// * chain lightning?
 UINT8 isNotAI[2]; 		// * +54A4
 BOOLEAN isHuman[2]; 		// * +54A6
 INT32 heroOwner[2]; 		// * +54A8
 BOOLEAN artifactAutoCast[2]; 		// * +54B0
 INT8 unk10[2];
 BOOL heroCasted[2]; 		// * +54B4
 INT32 heroMonCount[2]; 		// * +54BC
 H3Army* army[2]; 		// * +54C4
 H3BukaCompleteCombatMonster stacks[2][21]; 		// * +54CC 		// * a two-sided array of 21 stacks for each side of combat
 INT8 unk11[4];
 INT32 turnsSinceLastEnchanterCast[2]; 		// * +132A0
 INT8 unk12[16];
 INT32 currentMonSide; 		// * +132B8
 INT32 currentMonIndex; 		// * +132BC
 INT32 currentActiveSide; 		// * +132C0
 INT32 autoCombat; 		// * +132C4
 H3CombatMonster* activeStack; 		// * +132C8
 INT8 blueHighlight; 		// * +132CC
 INT8 unk13[3];
 INT32 creature_at_mouse_pos; 		// * +132D0
 INT32 mouse_coord; 		// * +132D4
 INT32 attacker_coord; 		// * +132D8
 INT32 move_type; 		// * +132DC 		// * the icons of CRCOMBAT.def, see H3MouseManager::H3MouseBattleFieldCursorType
 INT8 unk14[20];
 INT32 siegeKind2; 		// * +132F4
 BOOL finished; 		// * +132F8
 void* dlg; 		// * +132FC
 INT8 unk15[356];
 PCHAR backgroundPcxName; 		// * +13464
 INT8 adjacentSquares[187][12]; 		// * +13468
 INT8 unk16[12];
 INT8 updateRect[16]; 		// * +13D38
 INT8 unk17[12];
 INT32 cmNumWinPcxLoaded; 		// * +13D54
 H3Array<H3Obstacle> obstacleInfo; 		// * +13D58 		// * information about obstacles on battlefield
 BOOLEAN tacticsPhase;		// * +13D68
 INT8 unk18[3];
 INT32 turn; 		// * +13D6C
 INT32 tacticsDifference; 		// * +13D70
 INT8 unk19[4]; // spell related?
 INT8 TownTowerLoaded[3][36]; 		// * +13D78
 INT32 waitPhase; 		// * +13DE4
 INT32 HeroDAttack; 		// * +13DE8
 INT32 HeroDDefence; 		// * +13DEC
 INT32 HeroDSpellPower2; 		// * +13DF0
 INT32 HeroDSpellPoints; 		// * +13DF4
 INT32 TownPicturesLoaded[90]; 		// * +13DF8		
 INT32 fort_walls_hp[18]; 		// * +13F60 		// * hit points of town walls
 INT32 fort_walls_alive[18]; 		// * +13FA8
 INT8 unk20[4];
 void* CCellGrdPcx; 		// * +13FF4 		// * pcx of grids
 void* CCellShdPcx; 		// * +13FF8 		// * pcx to shade in blue using cheat menu
 INT32 GlobalCardeIndex; 		// * +13FFC
 BOOLEAN RedrawCreatureFrame[2][20]; 		// * +14000 		// * oddly there are only 20, not 21, slots for each side
 BOOLEAN heroAnimation[2]; 		// * +14028
 BOOLEAN heroFlagAnimation[2]; 		// * +1402A
 BOOLEAN turretAnimation[3]; 		// * +1402C
 INT8 unk21;
 INT8 unk22[188];
};
