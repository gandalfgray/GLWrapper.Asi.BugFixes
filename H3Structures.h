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
 UINT8 id;
 INT8 owner_id;
 INT8 built_this_turn;
 INT8 field_03;
 INT8 type;
 INT8 x;
 INT8 y;
 INT8 z;
 INT8 boat_x;
 INT8 boat_y;
 INT16 field_0A;
 INT32 up_hero_id;
 INT32 down_hero_id;
 INT8 mag_level;
 INT8 field_15;
 INT16 available_creatures[14];
 INT8 fields_32[6];
 DWORD field_38;
 DWORD field_3C;
 INT16 field_40;
 INT16 field_42;
 DWORD spells[5][6];
 INT8 magic_hild[5];
 INT8 fields_C1[7];
 CHAR name[12];
 INT32 _u8[3];
 H3Army guards; //+E0 = охрана замка
 H3Army guards0; //+118 = охрана замка
 DWORD built_bits; //*B +150h = уже построенные здания (0400)
 DWORD built_bits2;
 DWORD bonus_bits;//*B +158h = бонус на существ, ресурсы и т.п., вызванный строениями
 DWORD bonus_bits2;
 DWORD available_bits;      //*B- +160h = маска доступных для строения строений
 DWORD available_bits2;     
};

struct H3Hero
{
 INT16 x; // +0
 INT16 y; // +2
 INT16 z; // +4
 BOOL8 is_visible; // +6
 DWORD xyz; // +7
 BOOL8 is_object_under; // +11
 INT32 object_type_under; // +12
 DWORD object_flags_under; // +16
 DWORD object_under_setup; // +20
 UINT16 spell_points; // +24
 DWORD id; // +26
 DWORD unk1; // +30
 INT8 owner_id; // +34
 CHAR name[13]; // +35
 DWORD hero_class; // +48
 UINT8 pic; // +52
 INT32 dest_x; // +53
 INT32 dest_y; // +57
 INT32 dest_z; // +61
 INT8 unk2[3]; // +65
 UINT8 patrol_x; // +68
 UINT8 patrol_y; // +69
 UINT8 patrol_radius; // +70
 INT8 unk3; // +71
 UINT8 flags; // +72
 DWORD movement_points_max; // +73
 DWORD movement_points; // +77
 DWORD experience; // +81
 UINT16 level; // +85
 DWORD visited_objects[10]; // +87
 INT8 unk4[16]; // +127
 UINT8 skill_tree_seed;
 INT8 unk5;
 H3Army army; // +145
 INT8 second_skill[28]; // +201
 INT8 second_skill_show_position[28]; // +229
 DWORD second_skill_count; // +257
 DWORD temp_mod_flags; // + 261	
 FLOAT ai_experience_effectivness;
 INT8 dimdoor_cast_count; // +269
 DWORD disguise; // +270
 DWORD fly; // +274
 DWORD waterwalk; // +278
 INT8 morale; // +282
 INT8 luck; // +283
 BOOL8 is_sleeping; // +284
 INT8 unk6[12]; // +284
 DWORD vision_power;
 H3Artifact body_art[19]; // +301
 UINT8 free_add_slots; // +453
 UINT8 locked_slot[14]; // +454
 H3Artifact backpack_art[64]; // +468
 INT8 backpack_arts_count; // +980
 DWORD gender; // +981
 BOOL8 has_custom_biography; // +985
 INT8 biography[16]; // +986
 INT8 learned_spell[70]; // +1002
 INT8 available_spell[70]; // +1072
 UINT8 attack; // +1142
 UINT8 defence; // +1143
 UINT8 power; // +1144
 UINT8 knowledge; // +1145
 DWORD ai_agressivness; // Agressivness + 1146
 DWORD ai_power_effectivness; // PowerEffectivness +1150
 DWORD ai_duration_effectivness; // DurationEffectivness +1154
 DWORD ai_knowl_effectivness; // KnowledgeEffectivness +1158
 DWORD ai_dmana_effectivness; // DoubleManaEffectivness +1162
 DWORD ai_mana_effectivness; // ManaEffectivness +1164
};
