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
 DWORD disguise; // +270
 DWORD fly; // +274
 DWORD waterwalk; // +278
 INT8 morale; // +282
 INT8 luck; // +283
 BOOL8 is_sleeping; // +284
 INT8 unk6[12]; // +284
 DWORD vision_power;
 H3Artifact body_art[19]; // +301
 BYTE free_add_slots; // +453
 BYTE locked_slot[14]; // +454
 H3Artifact backpack_art[64]; // +468
 BYTE backpack_arts_count; // +980
 DWORD gender; // +981
 BOOL8 has_custom_biography; // +985
 INT8 biography[16]; // +986
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
