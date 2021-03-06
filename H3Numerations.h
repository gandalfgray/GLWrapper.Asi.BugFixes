//
// SPELLS
//
// Adv.Map
#define SPL_SUMMON_BOAT  0
#define SPL_SCUTTLE_BOAT 1
#define SPL_VISIONS   2
#define SPL_VIEW_EARTH  3
#define SPL_DISGUISE  4
#define SPL_VIEW_AIR  5
#define SPL_FLY    6
#define SPL_WATER_WALK  7
#define SPL_DIMENSION_DOOR 8
#define SPL_TOWN_PORTAL  9
// Battle
#define SPL_QUICKSAND  10
#define SPL_LAND_MINE  11
#define SPL_FORCE_FIELD  12
#define SPL_FIRE_WALL  13
#define SPL_EARTHQUAKE  14
#define SPL_MAGIC_ARROW  15
#define SPL_ICE_BOLT  16
#define SPL_LIGHTNING_BOLT 17
#define SPL_IMPLOSION  18
#define SPL_CHAIN_LIGHTNING 19
#define SPL_FROST_RING  20
#define SPL_FIREBALL  21
#define SPL_INFERNO   22
#define SPL_METEOR_SHOWER 23
#define SPL_DEATH RIPPLE 24
#define SPL_DESTROY_UNDEAD 25
#define SPL_ARMAGEDDON  26
#define SPL_SHIELD   27
#define SPL_AIR_SHIELD  28
#define SPL_FIRE_SHIELD  29
#define SPL_PROTECTION_FROM_AIR  30
#define SPL_PROTECTION_FROM_FIRE 31
#define SPL_PROTECTION_FROM_WATER 32
#define SPL_PROTECTION_FROM_EARTH 33
#define SPL_ANTI_MAGIC  34
#define SPL_DISPEL   35
#define SPL_MAGIC_MIRROR 36
#define SPL_CURE   37
#define SPL_RESURRECTION 38
#define SPL_ANIMATE_DEAD 39
#define SPL_SACRIFICE  40
#define SPL_BLESS   41
#define SPL_CURSE   42
#define SPL_BLOODLUST  43
#define SPL_PRECISION  44
#define SPL_WEAKNESS  45
#define SPL_STONE_SKIN  46
#define SPL_DISRUPTING_RAY 47
#define SPL_PRAYER   48
#define SPL_MIRTH   49
#define SPL_SORROW   50
#define SPL_FORTUNE   51
#define SPL_MISFORTUNE  52
#define SPL_HASTE   53
#define SPL_SLOW   54
#define SPL_SLAYER   55
#define SPL_FRENZY   56
#define SPL_TITANS_LIGHTNING_BOLT 57
#define SPL_COUNTERSTRIKE 58
#define SPL_BERSERK   59
#define SPL_HYPNOTIZE  60
#define SPL_FORGETFULNESS 61
#define SPL_BLIND   62
#define SPL_TELEPORT  63
#define SPL_REMOVE_OBSTACLE 64
#define SPL_CLONE   65
#define SPL_FIRE_ELEMENTAL 66
#define SPL_EARTH_ELEMENTAL 67
#define SPL_WATER_ELEMENTAL 68
#define SPL_AIR_ELEMENTAL 69
// not available for Hero
#define SPL_STONE   70
#define SPL_POISON   71
#define SPL_BIND   72
#define SPL_DESEASE   73
#define SPL_PARALYZE  74
#define SPL_AGING   75
#define SPL_DEATH_CLOUD  76
#define SPL_THUNDERBOLT  77
#define SPL_DISPEL_HELPFUL  78
#define SPL_DEATH_STARE  79
#define SPL_ACID_BREATH  80

// Hero Classes
#define HC_KNIGHT 0
#define HC_CLERIC 1
#define HC_RANGER 2
#define HC_DRUID 3
#define HC_ALCHEMIST 4
#define HC_WIZARD 5
#define HC_DEMONIAC 6
#define HC_HERETIC 7
#define HC_DEATH_KNIGHT 8
#define HC_NECROMANCER 9
#define HC_OVERLORD 10
#define HC_WARLOCK 11
#define HC_BARBARIAN 12
#define HC_BATTLE_MAGE 13
#define HC_BEASTMASTER 14
#define HC_WITCH 15
#define HC_PLANESWALKER 16
#define HC_ELEMENTALIST 17

// Battle Creature flags
#define BCF_DOUBLE_WIDE 0x1
#define BCF_FLYER 0x2
#define BCF_SHOOTER 0x4
#define BCF_EXT_ATTACK 0x8
#define BCF_ALIVE 0x10
#define BCF_DESTROY_WALLS 0x20
#define BCF_SIEGE_WEAPON 0x40
#define BCF_SLAYERED_1 0x80
#define BCF_SLAYERED_2 0x100
#define BCF_SLAYERED_3 0x200
#define BCF_MIND_IMMUN 0x400
#define BCF_NO_OBSTACLE_PENALTY 0x800
#define BCF_NO_MELEE_PENALTY 0x1000
#define BCF_UNK2000 0x2000
#define BCF_FIRE_IMMUN 0x4000
#define BCF_DOUBLE_ATTACK 0x8000
#define BCF_NO_RETALIATION 0x10000
#define BCF_NO_MORALE 0x20000
#define BCF_UNDEAD 0x40000
#define BCF_ATTACK_ALL_AROUND 0x80000
#define BCF_MAGOG 0x100000
#define BCF_CANNOT_MOVE 0x200000
#define BCF_SUMMON 0x400000
#define BCF_CLONE 0x800000
#define BCF_MORALE 0x1000000
#define BCF_WAITING 0x2000000
#define BCF_DONE 0x4000000
#define BCF_DEFENDING 0x8000000
#define BCF_SACRIFICED 0x10000000
#define BCF_NO_COLORING 0x20000000
#define BCF_GRAY 0x40000000
#define BCF_DRAGON 0x80000000

// Hero temp flags
#define HTF_WELL 0x1
#define HTF_STABLES 0x2
#define HTF_BOUY 0x4
#define HTF_SWAN_POND 0x8
#define HTF_IDOL_FORTUNE_MORAL 0x10
#define HTF_FOUNTAIN_FORTUNE_1 0x20
#define HTF_WATERING_HOLE 0x40
#define HTF_OASIS 0x80
#define HTF_TEMPLE 0x100
#define HTF_SHIPWRECK 0x200
#define HTF_CRYPT 0x400
#define HTF_DERELICT_SHIP 0x800
#define HTF_PIRAMYD 0x1000
#define HTF_FAERIE_RING 0x2000
#define HTF_FOUNTAIN_OF_YOUTH 0x4000
#define HTF_MERMAIDS 0x8000
#define HTF_RALLY_FLAG 0x10000
#define HTF_IN_TAVERN 0x20000
#define HTF_IN_BOAT 0x40000
#define HTF_UNK1 0x80000
#define HTF_SIRENS 0x100000
#define HTF_WARRIOR_TOMB 0x200000
#define HTF_LUCK_CHEAT 0x400000
#define HTF_MORALE_CHEAT 0x800000
#define HTF_SPEED_CHEAT 0x1000000
#define HTF_IDOL_FORTUNE_LUCK 0x2000000
#define HTF_TEMPLE_2 0x4000000
#define HTF_FOUNTAIN_FORTUNE_2 0x8000000
#define HTF_FOUNTAIN_FORTUNE_3 0x10000000
#define HTF_FOUNTAIN_FORTUNE_4 0x20000000
#define HTF_UNK2 0x40000000
#define HTF_UNK3 0x80000000

// Battle actions
#define	BA_CANCEL 0
#define	BA_CAST_SPELL 1
#define	BA_WALK 2
#define	BA_DEFEND 3
#define	BA_RETREAT 4
#define	BA_SURRENDER 5
#define	BA_WALK_ATTACK 6
#define	BA_SHOOT 7
#define	BA_WAIT 8
#define	BA_CATAPULT 9
#define	BA_MONSTER_SPELL 10
#define	BA_FIRST_AID_TENT 11
#define	BA_NOTHING 12
