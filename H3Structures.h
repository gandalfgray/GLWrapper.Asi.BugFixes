
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
 bool built_this_turn; // +2
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
 bool is_mana_vortex_unused; // +51
 INT8 unk5[16]; // +52
 DWORD spells[5][6]; // +68
 bool magic_guild[5]; // +188
 INT8 unk6[3]; // +193
 H3String name; // +196
 INT8 unk7[12]; // +212
 H3Army guards; // +224
 H3Army guards_unused; // +280
 DWORD built_bits[2]; // +336
 DWORD built_bits2[2]; // +344
 DWORD buildable_bits[2]; // +352   
};

struct H3Hero
{
 INT16 x; // +0
 INT16 y; // +2
 INT16 z; // +4
 bool is_visible; // +6
 DWORD xyz; // +7
 bool is_object_under; // +11
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
 bool is_sleeping; // +284
 INT8 unk6[12]; // +284
 INT32 vision_power;
 H3Artifact body_art[19]; // +301
 BYTE free_add_slots; // +453
 BYTE locked_slot[14]; // +454
 H3Artifact backpack_art[64]; // +468
 BYTE backpack_arts_count; // +980
 DWORD gender; // +981
 bool has_custom_biography; // +985
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
 DWORD ai_mana_effectivness; // ManaEffectivness +1164
};
