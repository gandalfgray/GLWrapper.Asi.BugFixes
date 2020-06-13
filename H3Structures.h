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
 UINT8 visible; // +6
 DWORD mui_xyz; // +7
 INT8 field_0B; // +11
 DWORD miu_object_type; // +12
 DWORD miu_object_c_flag; // +16
 DWORD miu_setup; // +20
 UINT16 spell_points; // +24
 DWORD id; // +26
 DWORD id_wtf; // +30
 INT8 owner_id; // +34
 CHAR name[13]; // +35
 DWORD _class; // +48
 INT8 pic; // +52
 DWORD aim_x; // +53
 DWORD aim_y; // +57
 DWORD aim_z; // +61
 INT8 field_41[3]; // +65
 INT8 x_0; // +68
 INT8 y_0; // +69
 INT8 run; // +70
 INT8 field_47; // +71
 INT8 flags; // +72
 DWORD movement_points_max; // +73
 DWORD movement_points; // +77
 DWORD expa; // +81
 INT16 level; // +85
 DWORD visited[10]; // +87
 INT8 field_7F[18]; // +127
 H3Army army; // +145
 INT8 second_skill[28]; // +201
 INT8 second_skill_show[28]; // +229
 DWORD second_skill_count; // +257
 DWORD temp_mod_flags; // + 261	
 float ai_exp_eff_109;
 INT8 dd_cast_this_turn; // +269
 DWORD disguise; // +270
 DWORD fly; // +274
 DWORD waterwalk; // +278
 INT8 d_morale_1; // +282
 INT8 d_luck; // +283
 INT8 is_sleeping_byte11C; // +284
 INT8 field_11E[16]; // +284
 H3Artifact doll_art[19]; // +301
 INT8 free_add_slots; // +453
 INT8 locked_slot[14]; // +454
 H3Artifact backpack_art[64]; // +468
 INT8 backpack_arts_count; // +980
 DWORD sex; // +981
 bool has_biography; // +985
 INT8 biography[16]; // +986
 INT8 spell[70]; // +1002
 INT8 spell_level[70]; // +1072
 INT8 attack; // +1142
 INT8 defence; // +1143
 INT8 power; // +1144
 INT8 knowledge; // +1145
 DWORD ai_agress_47A; // Agressivness + 1146
 DWORD ai_power_eff_47E; // PowerEffectivness +1150
 DWORD ai_duration_eff_482; // DurationEffectivness +1154
 DWORD ai_knowl_eff_486; // KnowledgeEffectivness +1158
 DWORD ai_dmana_eff_48A; // DoubleManaEffectivness +1162
 DWORD ai_mana_eff_48E; // ManaEffectivness +1164
};
