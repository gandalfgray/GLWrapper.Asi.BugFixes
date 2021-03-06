#include "patcher_x86.hpp"
#include "H3Numerations.h"
#include "H3Structures.h"
#include "H3BukaCompleteStructures.h"
#include "H3ChroniclesStructures.h"
#include "H3GUI.h"

// Global structures
int aHeroesCount;
int aH3Main;
int aActivePlayer;
int aH3HeroClass;
int aMarketHero;
int aMarketBackpackIndexOfFirstSlot;
int aMarketSelectedSlotIndex;
int aH3Spell;
int aNetworkGame;

// Important proc addresses
int isHumanProcAddress;
int getMeProcAddress;
int getDlgItemProcAddress;
int beforeGameAddress;
int canShootProcAddress;

Patcher* _P;
PatcherInstance* _PI;

static _bool_ plugin_On = 0;

// Выбранные в качестве стартовых перед генерацией герои.
int preselectedHeroesRmg[8];
// Маска выбранных игроком героев для генератора карт.
char heroSelectedByPlayerRmg[156];
int randintProcAddress;

// Подготавливаем информацию о стартовых героях игроков.
int __stdcall preselectHeroesRmg(LoHook* h, HookContext* c)
{
  // Если текущим игроком выбран герой, записываем его номер.
  if (*(int*)(c->eax + 32) >= 0)
  {
    preselectedHeroesRmg[c->ecx] = *(int*)(c->eax + 44 + 4* *(int*)(c->eax + 32));
  }
  // Иначе - номера нет.
  else
  {
    preselectedHeroesRmg[c->ecx] = -1;
  }
  
  return EXEC_DEFAULT;
}

// Убираем стартовых героев из доступных для генерации в тюрьму.
int __stdcall excludeStartingHeroesRmg(LoHook* h, HookContext* c)
{
  // Изначально все неиспользованы.
  for(int i = 0; i < 156; i++)
  {
      heroSelectedByPlayerRmg[i] = 0;
  }
  
  // Проходим по всем игрокам.
  for (int i = 0; i < 8; i++)
  {
    // Если у игрока настроен стартовый герой, учитываем его как использованного.
    if (preselectedHeroesRmg[i] != -1)
    {
        heroSelectedByPlayerRmg[preselectedHeroesRmg[i]] = true;
    }
  }
  
  return EXEC_DEFAULT;
}

// Генерация случайного героя для случайной карты.
int __stdcall getRandHeroRmg(HiHook* hook, int rmgMain)
{
  // Количество доступных героев.
  int heroes_count = 0;
  
  // Считаем количество доступных героев.
  for (int i = 0; i < 156; i++)
  {
    // Если герой ещё неиспользован и не выбран игроком, учитываем его в общем количестве.
    if (!(*(char*)(rmgMain + 0xF88 + i)) && !heroSelectedByPlayerRmg[i])
    {
      heroes_count++;
    }
  }
  
  // Герои есть - генерируем.
  if (heroes_count > 0)
  {
    // Случайный герой.
    int r_hero_ix = CALL_2(int, __fastcall, randintProcAddress, 0, heroes_count - 1);
    
    for (int i = 0; i < 156; i++)
    {
      // Если герой ещё неиспользован и не выбран игроком, учитываем его.
      if (!(*(char*)(rmgMain + 0xF88 + i)) && !heroSelectedByPlayerRmg[i])
      {
        if (r_hero_ix <= 0)
        {
          // Сгенерировали.
          return i;
        }
        else
        {
          r_hero_ix--;
        }
      }
    }
    
    // Не удалось сгенерировать - возвращаем пусто.
    return -1;
  }
  // Героев нет - возвращаем пусто.
  else
  {
    return -1;
  }
}

bool gChat;
int send2ChatProcAddress;
int __stdcall send2Chat(LoHook* h, HookContext* c) 
{
    char* mes = "Z10_BugFixes_vA0.9";

    if(o_NetworkGame && gChat == 1)
    if(gChat == 1)
    {
        gChat = false;
        CALL_2(char, __fastcall, send2ChatProcAddress, mes, 127);
    }

    return EXEC_DEFAULT;
}

int __stdcall setChatTrigger(LoHook* h, HookContext* c) 
{
    gChat = true;

    return EXEC_DEFAULT;
}

// Prevents AI from casting Fly if they don't have it.
int Ai_WaterwalkFlyReturnAddress_Cast;
int Ai_WaterwalkFlyReturnAddress_Skip;
int __stdcall Ai_WaterwalkFly(LoHook *h, HookContext *c)
{
    if (c->eax == 0) // no angel wings
    {
        H3Hero* hero = (H3Hero*)(c->esi);
        if (hero->learned_spell[SPL_FLY] == 0 && hero->available_spell[SPL_FLY] == 0) // this AI hero does not have the means to cast fly (id = 6)
        {
            if (hero->learned_spell[SPL_WATER_WALK] != 0 || hero->available_spell[SPL_WATER_WALK] != 0) // this AI hero has access to waterwalk (id = 7)
            {
                if (hero->waterwalk_power == -1) // waterwalk is not cast ~ waterwalk field is *(&hero + 0x116) (see 0x4E6040 Cast_waterwalk function)
                    c->return_address = Ai_WaterwalkFlyReturnAddress_Cast; // try to cast waterwalk instead (code checks for Boots of Levitation first...)
                else
                    c->return_address = Ai_WaterwalkFlyReturnAddress_Skip; // skip procedure
                return NO_EXEC_DEFAULT;
            }
        }
    }
    return EXEC_DEFAULT;
}

// The Castle's Lighthouse building bonus
int castleOwnerCheckReturnAddress;
int __stdcall castleOwnerCheck(LoHook *h, HookContext *c)
{
   H3Town *town = (H3Town*)(c->ecx);
   H3Hero *hero = *(H3Hero**)(c->ebp - 4); // _Hero_ is stored in temp variable [LOCAL.1]

   if (hero->owner_id == town->owner_id) // normal
      return EXEC_DEFAULT;
  
   c->return_address = castleOwnerCheckReturnAddress; // skip procedure
   return NO_EXEC_DEFAULT;
}

int refugeCampReturnAddress_Skip;
int refugeCampReturnAddress_Write;
int __stdcall fixRefugeCamp(LoHook* hook, HookContext* c)
{
    if ((BYTE)c->eax == 0)
    {
        c->return_address = refugeCampReturnAddress_Skip;
        return NO_EXEC_DEFAULT;
    }
    else
    {
        c->return_address = refugeCampReturnAddress_Write;
        return EXEC_DEFAULT;
    }
}

// fix ghost hero (without army)
int __stdcall ghostHeroFix(LoHook* h, HookContext* c)
{
    H3Army* army = (H3Army*)(*(int*)(c->esi + 4 * *(int*)(c->esi + 72) + 64) + 145);
    int cell = *(int*)(c->esi + 80);
    H3Army* destArmy = (H3Army*)(*(int*)(c->esi + 4 * *(int*)(c->esi + 76) + 64) + 145);
    int destCell = *(int*)(c->esi + 84);

    if(army != destArmy)
    {
        int creaturesCount = 0;
        for(int i=0; i<7; i++)
        {
            if(army->type[i] >= 0)
                creaturesCount += army->count[i];
        }

        if(creaturesCount <= 1 && destArmy->type[destCell] < 0)
        {
            //c->return_address = h->GetAddress() + 0x26;
            c->return_address += 30;
            return NO_EXEC_DEFAULT;
        }
    }
    
    return EXEC_DEFAULT;
}

// fix Harpy fly after Dendroid bind
int fixHarpyBindsReturnAddress;
int __stdcall fixHarpyBinds(LoHook* h, HookContext* c)
{
    H3CombatMonster* battleStack = (H3CombatMonster*)c->ebx;

    if (battleStack->spell_duration[SPL_BIND])
    {
        c->return_address = fixHarpyBindsReturnAddress;
        return NO_EXEC_DEFAULT;
    }
    
    return EXEC_DEFAULT;
}

int __stdcall fixHarpyBindsBukaComplete(LoHook* h, HookContext* c)
{
    H3BukaCompleteCombatMonster* battleStack = (H3BukaCompleteCombatMonster*)c->ebx;

    if (battleStack->spell_duration[SPL_BIND])
    {
        c->return_address = fixHarpyBindsReturnAddress;
        return NO_EXEC_DEFAULT;
    }
    
    return EXEC_DEFAULT;
}

// fix double cast during 1 round in battle
int fixDoubleCastReturnAddress;
int __stdcall fixDoubleCast(LoHook* hook, HookContext* c)
{
    if(*(int*)(c->esi + c->eax * 4 + 0x54B4) && !(*(BYTE*)(c->esi + 0x13D74)))
    {
        c->return_address = fixDoubleCastReturnAddress;
        return NO_EXEC_DEFAULT;
    }

    return EXEC_DEFAULT;
}


int __stdcall fixArtMerchantPrice(LoHook* hook, HookContext* c)
{
    int i = (o_Market_BackpackIndexOfFirstSlot + o_Market_SelectedBackpackSlotIndex) % o_Market_Hero->backpack_arts_count;
    c->ecx = o_Market_Hero->backpack_art[i].type;
    c->eax = c->edx = o_Market_Hero->backpack_art[i].subtype;
    c->return_address += 7;
    return NO_EXEC_DEFAULT;
}

BOOL __stdcall disableCloneOverlay(HiHook* h, H3CombatMonster* battleStack, INT32 newHexIndex, BOOL tryAnotherHex, INT32* anotherHexIndex)
{
    if(newHexIndex == battleStack->hex_index)
        return false;

    if(battleStack->info.flags & BCF_DOUBLE_WIDE)
    {
        if(battleStack->orient_to_right)
        {
            if(battleStack->hex_index - 1 == newHexIndex)
                return false;

            if(battleStack->hex_index == newHexIndex - 1)
                return false;
        }
        else
        {
            if(battleStack->hex_index + 1 == newHexIndex)
                return false;

            if(battleStack->hex_index == newHexIndex + 1)
                return false;
        }
    }

    return CALL_4(BOOL, __thiscall, h->GetDefaultFunc(), battleStack, newHexIndex, tryAnotherHex, anotherHexIndex);
}

BOOL __stdcall disableCloneOverlayBukaComplete(HiHook* h, H3BukaCompleteCombatMonster* battleStack, INT32 newHexIndex, BOOL tryAnotherHex, INT32* anotherHexIndex)
{
    if(newHexIndex == battleStack->hex_index)
        return false;

    if(battleStack->info.flags & BCF_DOUBLE_WIDE)
    {
        if(battleStack->orient_to_right)
        {
            if(battleStack->hex_index - 1 == newHexIndex)
                return false;

            if(battleStack->hex_index == newHexIndex - 1)
                return false;
        }
        else
        {
            if(battleStack->hex_index + 1 == newHexIndex)
                return false;

            if(battleStack->hex_index == newHexIndex + 1)
                return false;
        }
    }

    return CALL_4(BOOL, __thiscall, h->GetDefaultFunc(), battleStack, newHexIndex, tryAnotherHex, anotherHexIndex);
}

// fix AI + Angelic Alliance + neutral creatures
int __stdcall fixAngelicAllianceAI(LoHook* h, HookContext* c)
{
    if (c->edi == -1)
        c->edi = 8;
    
    return EXEC_DEFAULT;
}

// fix of terrible bug in QuickBattle
int isHiddenBattleProcAddress;
int removeStackFromHexProcAddress;
void __stdcall fixQuickBattleStackFly(HiHook* h, void* battleManager, H3CombatMonster* stack, INT32 newHexIndex)
{
    if (CALL_1(bool, __thiscall, isHiddenBattleProcAddress, battleManager))
    {
        CALL_2(void, __thiscall, removeStackFromHexProcAddress, battleManager, stack);
    }

    CALL_3(void, __thiscall, h->GetDefaultFunc(), battleManager, stack, newHexIndex);
}

// fix AI Necromancy
int afterTheorBattleProcAddress;
void __stdcall fixAINecromancy(HiHook *h, void* winner, void* loser, void* town)
{
        CALL_3(void, __thiscall, h->GetDefaultFunc(), winner, loser, town);
        CALL_2(void, __thiscall, afterTheorBattleProcAddress, loser, 1); // delete army after
}

// Update max. MP
int calcMovementProcAddress;
int calcMovementWrapProcAddress;
int __stdcall updateMaxLandMovement(LoHook* hook, HookContext* c)
{
    H3Hero* hero = (H3Hero*)c->esi;
    hero->movement_points_max = CALL_2(INT32, __thiscall, calcMovementProcAddress, hero, 0);

    return EXEC_DEFAULT;
}

int __stdcall recalculateMovementAfterVisitObject(LoHook* hook, HookContext* c)
{
    H3Hero* hero = (H3Hero*)c->edi;
    hero->movement_points_max = CALL_1(INT32, __thiscall, calcMovementWrapProcAddress, hero);

    return EXEC_DEFAULT;
}

void compressHeroBackpack(H3Hero* hero)
{
    H3Artifact compressedBackpack[64];
    int cb_index = 0;

    for(int i = 0; i < 64; i++)
    {
        if (hero->backpack_art[i].type != -1)
        {
            compressedBackpack[cb_index].type = hero->backpack_art[i].type;
            compressedBackpack[cb_index].subtype = hero->backpack_art[i].subtype;
            cb_index++;
        }
    }

    for(int i = cb_index; i < 64; i++)
    {
        compressedBackpack[i].type = -1;
        compressedBackpack[i].subtype = 0;
    }

    memcpy(hero->backpack_art, compressedBackpack, sizeof(H3Artifact) * 64);
}

void* __stdcall fixBackpackArtMerchantDlg(HiHook* hook, void* dlg, int x, int y)
{
    compressHeroBackpack(o_Market_Hero);
    CALL_3(void*, __thiscall, hook->GetDefaultFunc(), dlg, x, y);
    return dlg;
}

int __stdcall fixAllianceTown(HiHook* hook, H3TownManager* townMgr, H3EventMsg* msg)
{
    H3Player* mePlayer = CALL_1(H3Player*, __thiscall, getMeProcAddress, o_H3Main);
    if(mePlayer->id != townMgr->town->owner_id || mePlayer != o_ActivePlayer)
    {
        if (msg->type == MT_MOUSEBUTTON)
        {
            int item_id = msg->item_id;
            if (msg->subtype == MST_LBUTTONDOWN)
                if ((item_id == 17) || (item_id == 14)) // art merchants || market
                    return 1;
        }
    }

    if (((msg->type == 1) || (msg->type == 2)) && (msg->subtype == 57)) //SPACE key pressed
    {
        if (townMgr->town->garrison_hero_id != -1 && townMgr->town->visiting_hero_id != -1)
        {
            if(o_H3Main->heroes[townMgr->town->garrison_hero_id].owner_id != o_H3Main->heroes[townMgr->town->visiting_hero_id].owner_id)
                return 1;
        }
    }

    return CALL_2(int, __thiscall, hook->GetDefaultFunc(), townMgr, msg);
}

// disguise bug fix
H3Hero heroRMCopy;

struct heroSharedFieldStruct
{
    char duration : 4;
    char disguisePower : 4;
        unsigned short groundMaxMP;
    char unusedByte;
} *heroSharedField;

int __stdcall setDisguise(LoHook* h, HookContext* c)
{
    H3Hero* hero = (H3Hero*)c->esi;
    heroSharedField = (heroSharedFieldStruct*)&hero->disguise_power;
    heroSharedField->disguisePower = c->edi;
    heroSharedField->duration = 1;
    
    return NO_EXEC_DEFAULT;
}

int __stdcall decDisguise(LoHook* h, HookContext* c)
{
    for (int i = 0; i < o_HEROES_COUNT; i++)
    {
        heroSharedField = (heroSharedFieldStruct*)&o_H3Main->heroes[i].disguise_power;
    heroSharedField->groundMaxMP = USHRT_MAX;
        
        if (o_H3Main->heroes[i].owner_id == o_ActivePlayer->id )
        {
            if ( heroSharedField->duration != -1 ) 
            heroSharedField->duration -= 1;         

        if (heroSharedField->duration == -1)
        o_H3Main->heroes[i].disguise_power = -1;
        }
    }

    return EXEC_DEFAULT;
}

int __stdcall makeHeroRMCopy(LoHook* h, HookContext* c)
{
        H3Hero* hero = *(H3Hero**)(c->ebp + 8);

    heroRMCopy = *hero;
    H3Player* mePlayer = CALL_1(H3Player*, __thiscall, getMeProcAddress, o_H3Main);
    heroSharedField = (heroSharedFieldStruct*)&hero->disguise_power;

    if(mePlayer->id == hero->owner_id && heroSharedField->duration == 0)
        heroRMCopy.disguise_power = -1;
    else
        heroRMCopy.disguise_power = heroSharedField->disguisePower;

    *(H3Hero**)(c->ebp + 8) = &heroRMCopy;

    return EXEC_DEFAULT;
}

// Admiral's hat bug fix + movement fixes
int __stdcall mpSeaToGround(LoHook *h, HookContext *c)
{
    H3Hero* hero = (H3Hero*)c->esi;
    heroSharedField = (heroSharedFieldStruct*)&hero->disguise_power;

    if(heroSharedField->groundMaxMP != USHRT_MAX)
            c->eax = heroSharedField->groundMaxMP;
    else
        heroSharedField->groundMaxMP = c->eax;
    
    return EXEC_DEFAULT;
}

int __stdcall mpGroundToSea(LoHook *h, HookContext *c)
{
    H3Hero* hero = (H3Hero*)c->esi;
    heroSharedField = (heroSharedFieldStruct*)&hero->disguise_power;
    if(heroSharedField->groundMaxMP != USHRT_MAX)
            c->ecx = heroSharedField->groundMaxMP;
    else
        heroSharedField->groundMaxMP = *(int*)(c->esi + 0x49);
    
    return EXEC_DEFAULT;
}

int __stdcall killedHeroSeaToGround(LoHook *h, HookContext *c)
{
    H3Hero* hero = (H3Hero*)c->esi;
    int currentMaxWaterMovement = hero->movement_points_max;

    heroSharedField = (heroSharedFieldStruct*)&hero->disguise_power;
    if(heroSharedField->groundMaxMP != USHRT_MAX)
            hero->movement_points_max = heroSharedField->groundMaxMP;
    else
            hero->movement_points_max = CALL_2(INT32, __thiscall, calcMovementProcAddress, hero, 0);
    
        hero->movement_points = hero->movement_points * hero->movement_points_max / currentMaxWaterMovement;

    return EXEC_DEFAULT;
}

// not me bugs
int __stdcall fixNotMeID_ecx(LoHook* hook, HookContext* c)
{
    H3Player* mePlayer = CALL_1(H3Player*, __thiscall, getMeProcAddress, o_H3Main);
    c->ecx = mePlayer->id;
    
    return NO_EXEC_DEFAULT;
}

int __stdcall fixNotMeID_eax(LoHook* hook, HookContext* c)
{
    H3Player* mePlayer = CALL_1(H3Player*, __thiscall, getMeProcAddress, o_H3Main);
    c->eax = mePlayer->id;
    
    return NO_EXEC_DEFAULT;
}

int __stdcall fixNotMePlayer_edx(LoHook* hook, HookContext* c)
{
    //(H3Player*)c->edx = CALL_1(H3Player*, __thiscall, getMeProcAddress, o_H3Main);
    c->edx = CALL_1(int, __thiscall, getMeProcAddress, o_H3Main);
    return NO_EXEC_DEFAULT;
}

int __stdcall fixNotMePlayer_eax(LoHook* hook, HookContext* c)
{
    c->eax = CALL_1(int, __thiscall, getMeProcAddress, o_H3Main);
    return NO_EXEC_DEFAULT;
}


H3Dlg* __stdcall fixBuyCreaturesDlg(HiHook* hook, H3Dlg* dlg, INT32 x, INT32 y, INT32 a4, _ptr_ a5)
{
    CALL_5(H3Dlg*, __thiscall, hook->GetDefaultFunc(), dlg, x, y, a4, a5);

    H3Player* mePlayer = CALL_1(H3Player*, __thiscall, getMeProcAddress, o_H3Main);
    if(mePlayer != o_ActivePlayer)
    {
        int itemId = 526;
        H3DlgItem* dlgItem = CALL_2(H3DlgItem*, __thiscall, getDlgItemProcAddress, dlg, itemId);
        CALL_2(DWORD, __thiscall, dlgItem->v_table[9], dlgItem, false);
    }

    return dlg;
}


int __stdcall fixAdvMgrButton(HiHook* hook, void* advMgr, H3EventMsg* msg, int a3, int a4, int a5)
{
    if (msg->item_id == 11)
    {
        H3Player* mePlayer = CALL_1(H3Player*, __thiscall, getMeProcAddress, o_H3Main);
        if(mePlayer != o_ActivePlayer)
            return 1;
    }

    return CALL_5(int, __thiscall, hook->GetDefaultFunc(), advMgr, msg, a3,a4,a5);
}

int __stdcall preserveMonsterNumber(LoHook* hook, HookContext* c)
{
    if (*(int*)(c->ebp + 0x1C) == -1 && *(int*)(c->ebp + 0x28) == -1)
    {
        c->esp += 4; // pop edi
        H3Army* army = (H3Army*)c->ecx;
        int mon_number = 0;

        for (int i = 0; i < 7; i++)
            mon_number += army->count[i];  // restore count no matter if it is an upgraded creature or not

        c->eax = mon_number;
        return NO_EXEC_DEFAULT;
    }

    return EXEC_DEFAULT;
}

void __stdcall fixRemoveObstacle(HiHook* h, H3CombatManager* bm, int hex_id)
{
    CALL_2(void, __thiscall,  h->GetDefaultFunc(), bm, hex_id);
    bm->obstacleInfo[hex_id].unk_10 = -1;
}

void __stdcall fixBukaCompleteRemoveObstacle(HiHook* h, H3BukaCompleteCombatManager* bm, int hex_id)
{
    CALL_2(void, __thiscall,  h->GetDefaultFunc(), bm, hex_id);
    bm->obstacleInfo[hex_id].unk_10 = -1;
}

int __stdcall fixForgetfullnessDamage(LoHook* hook, HookContext* c)
{
    H3CombatMonster* battleStack = (H3CombatMonster*)c->ecx;
    
    if(CALL_2(bool, __thiscall, canShootProcAddress, battleStack, 0) == false)
        c->flags.ZF = 1;

    return EXEC_DEFAULT;
}

int __stdcall changeStatsFromTxt(LoHook* h, HookContext* c)
{
    o_H3Spell[SPL_FORGETFULNESS].flags.expert_mass_version = 1;

    //o_H3Spell[SPL_TITANS_LIGHTNING_BOLT].flags.AImisc = 0;

    o_H3HeroClass[HC_KNIGHT].agression = (float)1.0; // Knight
    o_H3HeroClass[HC_CLERIC].agression = (float)0.8; // Cleric
    o_H3HeroClass[HC_RANGER].agression = (float)1.0; // Ranger
    o_H3HeroClass[HC_DRUID].agression = (float)0.8; // Druid
    o_H3HeroClass[HC_ALCHEMIST].agression = (float)1.0; // Alchemist
    o_H3HeroClass[HC_WIZARD].agression = (float)1.0; // Wizard
    o_H3HeroClass[HC_DEMONIAC].agression = (float)1.2; // Demoniac
    o_H3HeroClass[HC_HERETIC].agression = (float)1.1; // Heretic
    o_H3HeroClass[HC_DEATH_KNIGHT].agression = (float)1.2; // Death Knight  
    o_H3HeroClass[HC_NECROMANCER].agression = (float)1.0; // Necromancer
    o_H3HeroClass[HC_OVERLORD].agression = (float)1.0; // Overlord
    o_H3HeroClass[HC_WARLOCK].agression = (float)1.0; // Warlock
    o_H3HeroClass[HC_BARBARIAN].agression = (float)1.1; // Barbarian
    o_H3HeroClass[HC_BATTLE_MAGE].agression = (float)1.1; // Battle Mage
    o_H3HeroClass[HC_BEASTMASTER].agression = (float)0.9; // Beastmaster
    o_H3HeroClass[HC_WITCH].agression = (float)1.0; // Witch
    o_H3HeroClass[HC_PLANESWALKER].agression = (float)0.9; // Planeswalker
    o_H3HeroClass[HC_ELEMENTALIST].agression = (float)1.0; // Elementalist  

    return EXEC_DEFAULT;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    if ( DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        if (!plugin_On)
        {
            plugin_On = 1;       
            _P = GetPatcher();
            _PI = _P->CreateInstance("Z10_BugFixes_vA");

            int check1, check2;

            // for HoMM SoD 3.2 (eng)
            check1 = *(int*)(0x4F8193+1);
            check2 = *(int*)(0x602149+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {  

                // Global structures addresses
                aHeroesCount = 0x4BD145;
                aH3Main = 0x699538;
                aActivePlayer = 0x69CCFC;
                aH3HeroClass = 0x67DCEC;
                aMarketHero = 0x6AAAE0;
                aMarketBackpackIndexOfFirstSlot = 0x6AAAD8;
                aMarketSelectedSlotIndex= 0x6AAAF8;
                aH3Spell = 0x687FA8;
                aNetworkGame = 0x69959C;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4BAA60;
                getMeProcAddress = 0x4CE670;
                getDlgItemProcAddress = 0x5FF5B0;
                beforeGameAddress = 0x4EEE0B;
                canShootProcAddress = 0x442610;
                send2ChatProcAddress = 0x554BB0;
                randintProcAddress = 0x50C7C0;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406F60, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BEFF0, setChatTrigger); 
                // Для Новой игры
                _PI->WriteLoHook(0x4C018D, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E3A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E4DF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x465943+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck 6 NOP
                _PI->WriteHexPatch(0x441524, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F642, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E4D6C;           
                _PI->WriteLoHook(0x4E4D40, castleOwnerCheck);   

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x430231;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430540;
                _PI->WriteLoHook(0x43020E, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CCC40+3, 0x696DE8);
                
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x50604F;
                refugeCampReturnAddress_Write = 0x505E21;
                _PI->WriteLoHook(0x505E15, fixRefugeCamp);
                   
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449B60, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5AF8E9, ghostHeroFix);
            
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x478365;
                _PI->WriteLoHook(0x47835B, fixHarpyBinds);
                
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x534621+3, 0x0FFFEFBF);
                 
                // fix art merchants price bug
                _PI->WriteLoHook(0x5EE619, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EE756, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EE8C0, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EC7F5, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ED1E7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x5A70C1, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                _PI->WriteLoHook(0x42C89E, fixAngelicAllianceAI);
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x46A080;
                removeStackFromHexProcAddress = 0x468310;
                _PI->WriteHiHook(0x4B4B84, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426F9E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426F9E+3, 0xDA89);  // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424880;
                _PI->WriteWord(0x426FE4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426EE0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                _PI->WriteWord(0x53C47F, 0x1874);
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4E4C00;
                calcMovementWrapProcAddress = 0x4E5000;
                //_PI->WriteLoHook(0x4A0D08, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AA76B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49E340, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5E5B30, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                _PI->WriteHiHook(0x5D3640, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
            
                // disguise bug fix
                _PI->WriteLoHook(0x41C7C6, setDisguise);
                _PI->WriteLoHook(0x4C6CD9, decDisguise);
                _PI->WriteLoHook(0x52EFB5, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C7DA5, "90 90 90");
                    
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A0CE6, mpGroundToSea);
                _PI->WriteLoHook(0x49E334, mpSeaToGround);
                _PI->WriteLoHook(0x4DA268, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x46133D, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5D4643, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D883, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DC74, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DD7F, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x4154E6, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x415653, fixNotMeID_ecx);     //могила воина      
                _PI->WriteLoHook(0x4156C3, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x4158B0, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x452989, fixNotMePlayer_eax);     //advmgr panel 2
                _PI->WriteLoHook(0x452989 + 0x17, fixNotMePlayer_edx);  //advmgr panel 2
                _PI->WriteHiHook(0x54EC50, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x4099D0, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5D4659, "EB 5B");  // fix crash
          
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AC5F5, preserveMonsterNumber);
                
                // fix crash
                _PI->WriteHiHook(0x466710, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                
                // fix wrong half damage
                _PI->WriteLoHook(0x442E9B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x586634, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты.
                _PI->WriteHiHook(0x54B5F0, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x54C4DB, excludeStartingHeroesRmg);
                       
            }

            // ------------------------------
            // RUS
            // ------------------------------

            // Heroes III Armageddon - v2.1 Buka
            check1 = *(int*)(0x4F2533+1);
            check2 = *(int*)(0x5F9649+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B9144;
                aH3Main = 0x690ED8;
                aActivePlayer = 0x69467C;
                aH3HeroClass = 0x673AFC;
                aMarketHero = 0x6A2450;
                aMarketBackpackIndexOfFirstSlot = 0x6A2448;
                aMarketSelectedSlotIndex= 0x6A2468;
                aH3Spell = 0x67D990;
                aNetworkGame = 0x690F3C;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B6BE0;
                getMeProcAddress = 0x4C9F40;
                getDlgItemProcAddress = 0x5F6750;
                beforeGameAddress = 0x4E985B;
                canShootProcAddress = 0x4478E0;
                send2ChatProcAddress = 0x54CDC0;
                randintProcAddress = 0x5063F0;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406AD0, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BAF70, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4BC103, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41D5B3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41D6F1+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x4640FD+1, 0x4D); // fdiv -> fmul
            
                // Neutral creatures luck 6 NOP
                _PI->WriteHexPatch(0x440545, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43E686, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4DFE5C;           
                _PI->WriteLoHook(0x4DFE30, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42EDEB;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x42F10B;
                _PI->WriteLoHook(0x42EDC8, Ai_WaterwalkFly);    

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C8550+3, 0x68E77C);

                // AI moat damage fix
                _PI->WriteByte(0x4B11E9, 0xEB); 
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x4FFDF6;
                refugeCampReturnAddress_Write = 0x4FFBC4;
                _PI->WriteLoHook(0x4FFBB8, fixRefugeCamp);
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x448B10, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5A7EE9, ghostHeroFix);
            
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x4766AD;
                _PI->WriteLoHook(0x4766A4, fixHarpyBinds);
                    
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x52D661+3, 0x0FFFEFBF);
                
                // fix double cast
                fixDoubleCastReturnAddress = 0x59761C;
                _PI->WriteLoHook(0x597367, fixDoubleCast);
            
                // fix size for Hero creature labels in Russian versions (adv. map right click)
                _PI->WriteByte(0x52895C+1, 12);
                _PI->WriteByte(0x5289E5+1, 12); 
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5E5E7D, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5E5F97, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5E6101, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5E4164, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5E4B37, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x59F6EE, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // не нужно
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x468740;
                removeStackFromHexProcAddress = 0x466A20;
                _PI->WriteHiHook(0x4B2BC7, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x425D6E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x425D6E+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x423750;
                _PI->WriteWord(0x425DB4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x425CB0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                _PI->WriteWord(0x5352C5, 0x1874);
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4DFCF0;
                calcMovementWrapProcAddress = 0x4E00F0;
                //_PI->WriteLoHook(0x49F068, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A884B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49C821, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5DD6B0, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);   
                    
                // alliance town fixes
                _PI->WriteHiHook(0x5CB840, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
            
                // disguise bug fix     
                _PI->WriteLoHook(0x41B9D9, setDisguise);
                _PI->WriteLoHook(0x4C2809, decDisguise);
                _PI->WriteLoHook(0x5281E5, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C387F, "90 90 90");
            
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x49F04A, mpGroundToSea);
                _PI->WriteLoHook(0x49C814, mpSeaToGround);
                _PI->WriteLoHook(0x4D53DB, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x45FB9B, fixNotMeID_eax); //townhall          
                _PI->WriteLoHook(0x5CC696, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D1C2, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40D593, fixNotMeID_eax); //witch hut
                _PI->WriteLoHook(0x41495E, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x414ACB, fixNotMeID_ecx);     //могила воина              
                _PI->WriteLoHook(0x414B2C, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x414CE0, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x4515C9, fixNotMePlayer_edx);     //advmgr panel 2
                _PI->WriteLoHook(0x4515C9 + 0x28, fixNotMePlayer_eax);  //advmgr panel 2
                _PI->WriteHiHook(0x5470C0, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x4095E0, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5CC6AC, "EB 55");  // fix crash
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AA5CA, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x464E70, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x441EAB, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x57DAB4, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x543F90, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x544E5B, excludeStartingHeroesRmg);
            
            }

            // Heroes III Armageddon - v2.2 Buka
            check1 = *(int*)(0x4F2863+1);
            check2 = *(int*)(0x5F9609+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B93B4;
                aH3Main = 0x690F28;
                aActivePlayer = 0x6946CC;
                aH3HeroClass = 0x673AFC;
                aMarketHero = 0x6A27B8;
                aMarketBackpackIndexOfFirstSlot = 0x6A27B0;
                aMarketSelectedSlotIndex= 0x6A27D0;
                aH3Spell = 0x67D9B8;
                aNetworkGame = 0x690F8C;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B6E40;
                getMeProcAddress = 0x4CA1D0;
                getDlgItemProcAddress = 0x5F6710;
                beforeGameAddress = 0x4E9B6B;
                canShootProcAddress = 0x4473E0;
                send2ChatProcAddress = 0x54D700;
                randintProcAddress = 0x505670;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406AA0, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BB1E0, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4BC373, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41D553+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41D691+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x463D9D+1, 0x4D); // fdiv -> fmul
            
                // Neutral creatures luck 6 NOP
                _PI->WriteHexPatch(0x440045, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43E186, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E031C;           
                _PI->WriteLoHook(0x4E02F0, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42EDDD;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x42F0DB;
                _PI->WriteLoHook(0x42ED98, Ai_WaterwalkFly);    

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C87E0+3, 0x68E7CC);

                // AI moat damage fix
                _PI->WriteByte(0x4B0C59, 0xEB);
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x500126;
                refugeCampReturnAddress_Write = 0x4FFEF4;
                _PI->WriteLoHook(0x4FFEE8, fixRefugeCamp);
            
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x448600, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5A7F19, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x47636D;
                _PI->WriteLoHook(0x476364, fixHarpyBinds);
                    
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x52D971+3, 0x0FFFEFBF);            
            
                // fix size for Hero creature labels in Russian versions (adv. map right click)
                _PI->WriteByte(0x528C4C+1, 12);
                _PI->WriteByte(0x528CD5+1, 12); 
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5E5E5D, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5E5F77, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5E60E1, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5E4144, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5E4B17, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x59F6FE, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // не нужно     
        
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x4683C0;
                removeStackFromHexProcAddress = 0x4666A0;
                _PI->WriteHiHook(0x4B2637, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
                    
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x425D0E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x425D0E+3, 0xDA89); // MOV EDX,EBX
            
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x4236F0;
                _PI->WriteWord(0x425D54, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x425C50, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
                    
                // RMG zone type bug fix
                _PI->WriteWord(0x535595, 0x1874);
            
                // Update max. movement points
                calcMovementProcAddress = 0x4E01B0;
                calcMovementWrapProcAddress = 0x4E05B0;
                //_PI->WriteLoHook(0x49EA58, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A823B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49C211, updateMaxLandMovement);
                    
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5DD690, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
            
                // alliance town fixes
                _PI->WriteHiHook(0x5CB800, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
                    
                // disguise bug fix
                _PI->WriteLoHook(0x41B979, setDisguise);
                _PI->WriteLoHook(0x4C2A79, decDisguise);
                _PI->WriteLoHook(0x5284D5, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C3AEF, "90 90 90");
            
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x49EA3A, mpGroundToSea);
                _PI->WriteLoHook(0x49C204, mpSeaToGround);
                _PI->WriteLoHook(0x4D586B, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x45F83B, fixNotMeID_eax); //townhall          
                _PI->WriteLoHook(0x5CC656, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D162, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40D533, fixNotMeID_eax); //witch hut
                _PI->WriteLoHook(0x4148FE, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x414A6B, fixNotMeID_ecx);     //могила воина          
                _PI->WriteLoHook(0x414ACC, fixNotMeID_eax); //мельница
                _PI->WriteLoHook(0x414C80, fixNotMeID_eax); //мельница
                _PI->WriteLoHook(0x4510C9, fixNotMePlayer_edx);     //advmgr panel 2
                _PI->WriteLoHook(0x4510C9 + 0x28, fixNotMePlayer_eax);  //advmgr panel 2
                _PI->WriteHiHook(0x547A20, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x409580, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5CC66C, "EB 55");  // fix crash
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AA03A, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x464AF0, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x4419AB, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x57E034, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x544260, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x54512B, excludeStartingHeroesRmg);
                    
            }

            // Heroes III Shadow - v3.1 Buka
            check1 = *(int*)(0x4F7EB3+1);
            check2 = *(int*)(0x602379+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {       
                // Global structures addresses
                aHeroesCount = 0x4BD245;
                aH3Main = 0x698560;
                aActivePlayer = 0x69BD20;
                aH3HeroClass = 0x67CD0C;
                aMarketHero = 0x6A9B20;
                aMarketBackpackIndexOfFirstSlot = 0x6A9B18;
                aMarketSelectedSlotIndex= 0x6A9B38;
                aH3Spell = 0x686FC8;
                aNetworkGame = 0x6985C4;
            
                // Important proc addresses
                isHumanProcAddress = 0x4BAB70;
                getMeProcAddress = 0x4CE770;
                getDlgItemProcAddress = 0x5FF7E0;
                beforeGameAddress = 0x4EEB0B;
                canShootProcAddress = 0x442E90;
                send2ChatProcAddress = 0x554CA0;
                randintProcAddress = 0x50B0F0;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406F80, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BF0F0, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4C028D, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E2D3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E40F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x465EF3+1, 0x4D); // fdiv -> fmul
            
                // Neutral creatures luck 6 NOP
                _PI->WriteHexPatch(0x441DA4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FEC2, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E4A4C;           
                _PI->WriteLoHook(0x4E4A20, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x430101;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430410;
                _PI->WriteLoHook(0x4300DE, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CCD40+3, 0x695E08);
            
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x505D3F;
                refugeCampReturnAddress_Write = 0x505B11;
                _PI->WriteLoHook(0x505B05, fixRefugeCamp);
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x44A3F0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5AF9A9, ghostHeroFix);
            
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x4789C5;
                _PI->WriteLoHook(0x4789BB, fixHarpyBinds);
                    
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x534471+3, 0x0FFFEFBF);

                // fix double cast
                fixDoubleCastReturnAddress = 0x59EE0F;
                _PI->WriteLoHook(0x59EDD8, fixDoubleCast);
            
                // fix art merchants price bug
                _PI->WriteLoHook(0x5EE9F9, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EEB36, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EECA0, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ECBD5, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ED5C7, fixArtMerchantPrice);
                    
                // Clone bug fix
                _PI->WriteHiHook(0x5A7201, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);

                // fix AI + Angelic Alliance + neutral creatures
                _PI->WriteLoHook(0x42C76E, fixAngelicAllianceAI);
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x46A630;
                removeStackFromHexProcAddress = 0x4688C0;
                _PI->WriteHiHook(0x4B4C54, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426E6E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426E6E+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424750;
                _PI->WriteWord(0x426EB4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426DB0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);

                // RMG zone type bug fix
                _PI->WriteWord(0x53C2DF, 0x1874);
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4E48E0;
                calcMovementWrapProcAddress = 0x4E4CE0;
                //_PI->WriteLoHook(0x4A0E38, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AA89B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49E470, updateMaxLandMovement);

                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5E5F10, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                _PI->WriteHiHook(0x5D3A20, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets

                // disguise bug fix
                _PI->WriteLoHook(0x41C6F6, setDisguise);
                _PI->WriteLoHook(0x4C6DD9, decDisguise);
                _PI->WriteLoHook(0x52EE05, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C7EA5, "90 90 90");

                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A0E16, mpGroundToSea);
                _PI->WriteLoHook(0x49E464, mpSeaToGround);
                _PI->WriteLoHook(0x4D9F78, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x4618ED, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5D4A23, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D983, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DD74, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DE7F, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x415611, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x41577E, fixNotMeID_ecx);     //могила воина          
                _PI->WriteLoHook(0x4157EE, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x4159DB, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x452E79, fixNotMePlayer_eax);     //advmgr panel 2
                _PI->WriteLoHook(0x452E79 + 0x17, fixNotMePlayer_edx);  //advmgr panel 2
                _PI->WriteHiHook(0x54ED30, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x409A30, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5D4A39, "EB 5B");  // fix crash           
        
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AC725, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x466CC0, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x44371B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x5862B4, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x54B450, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x54C33B, excludeStartingHeroesRmg);
                                                
            }

            // Heroes III Complete - v4.0 Buka
            check1 = *(int*)(0x4F7EB3+1);
            check2 = *(int*)(0x6021A9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {  
                // Global structures addresses
                aHeroesCount = 0x4BD495;
                aH3Main = 0x69CC88;
                aActivePlayer = 0x6A0448;
                aH3HeroClass = 0x67EFDC;
                aMarketHero = 0x6AE2A0;
                aMarketBackpackIndexOfFirstSlot = 0x6AE298;
                aMarketSelectedSlotIndex= 0x6AE2B8;
                aH3Spell = 0x689318;
                aNetworkGame = 0x69CCEC;
                   
                // Important proc addresses
                isHumanProcAddress = 0x4BADB0;
                getMeProcAddress = 0x4CEA20;
                getDlgItemProcAddress = 0x5FF610;
                beforeGameAddress = 0x4EE9DB;
                canShootProcAddress = 0x4426F0;
                send2ChatProcAddress = 0x5547A0;
                randintProcAddress = 0x50B680;

                // Chat message
                _PI->WriteLoHook(0x406F50, send2Chat); 
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BF330, setChatTrigger); 
                // Для Новой игры
                _PI->WriteLoHook(0x4C04BD, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E456+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E595+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x46592C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441626, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F7B2, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E4EFC;           
                _PI->WriteLoHook(0x4E4ED0, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x430209;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430519;
                _PI->WriteLoHook(0x4301E6, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CCFE0+3, 0x69A538);

                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x505DA9;
                refugeCampReturnAddress_Write = 0x505B7A;
                _PI->WriteLoHook(0x505B6E, fixRefugeCamp);
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449BE0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5AFA69, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind    
                fixHarpyBindsReturnAddress = 0x478568;
                _PI->WriteLoHook(0x47855E, fixHarpyBindsBukaComplete);
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x534031+3, 0x0FFFEFBF);
            
                // fix art merchants price bug
                _PI->WriteLoHook(0x5EE909, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EEA46, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EEBB0, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ECB15, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ED4F7, fixArtMerchantPrice);
                
                // Clone bug fix
                _PI->WriteHiHook(0x5A71F1, CALL_, EXTENDED_, THISCALL_, disableCloneOverlayBukaComplete);
                    
                // fix AI + Angelic Alliance + neutral creatures
                _PI->WriteLoHook(0x42C8E7, fixAngelicAllianceAI);
            
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x46A080;
                removeStackFromHexProcAddress = 0x4682D0;
                _PI->WriteHiHook(0x4B45F4, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
                    
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x42705E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x42705E+3, 0xDA89); // MOV EDX,EBX
            
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424950;
                _PI->WriteWord(0x4270A4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426FA0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
                    
                // RMG zone type bug fix
                _PI->WriteWord(0x53BE3F, 0x1874);
            
                // Update max. movement points
                calcMovementProcAddress = 0x4E4D90;
                calcMovementWrapProcAddress = 0x4E5190;
                //_PI->WriteLoHook(0x4A08A8, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AA2FB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49DF10, updateMaxLandMovement);
                    
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5E5E50, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
            
                // alliance town fixes
                _PI->WriteHiHook(0x5D39C0, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
                    
                // disguise bug fix
                _PI->WriteLoHook(0x41C876, setDisguise);
                _PI->WriteLoHook(0x4C7079, decDisguise);
                _PI->WriteLoHook(0x52EA25, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C8145, "90 90 90");
                    
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A0886, mpGroundToSea);
                _PI->WriteLoHook(0x49DF04, mpSeaToGround);
                _PI->WriteLoHook(0x4DA348, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x46123D, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5D49C3, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D923, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DD14, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DE1F, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x415586, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x4156F3, fixNotMeID_ecx);     //могила воина          
                _PI->WriteLoHook(0x415763, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x415950, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x452979, fixNotMePlayer_eax);     //advmgr panel 2
                _PI->WriteLoHook(0x452979 + 0x17, fixNotMePlayer_edx);  //advmgr panel 2
                _PI->WriteHiHook(0x54E740, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x409A30, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5D49D9, "EB 5B");  // fix crash           
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AC12E, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x4666E0, SPLICE_, EXTENDED_, THISCALL_, fixBukaCompleteRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x442F7B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x586564, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x54AF90, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x54BE7B, excludeStartingHeroesRmg);

            }

            // Heroes Chronicles Warlords & Underworld & Elements & Dragons - v1.0
            check1 = *(int*)(0x4EBA34+1);
            check2 = *(int*)(0x5AF329+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B26B4;
                aH3Main = 0x6388D8;
                aActivePlayer = 0x63C050;
                aH3HeroClass = 0x61C194;
                aMarketHero = 0x647F18;
                aMarketBackpackIndexOfFirstSlot = 0x647F10;
                aMarketSelectedSlotIndex= 0x647F30;
                aH3Spell = 0x625398;
            
                // Important proc addresses
                isHumanProcAddress = 0x4B0220;
                beforeGameAddress = 0x4E37F2;
                canShootProcAddress = 0x447350;
            
                // Armorer fix
                _PI->WriteByte(0x41D5F3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41D731+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x461206+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x43E0F6, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FFB5, "90 90 90 90 90 90");
            
                // что-то неверно
                // The Castle's Lighthouse building bonus
                //castleOwnerCheckReturnAddress = 0x4D9132;         
                //_PI->WriteLoHook(0x4D9107, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42ED9B;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x42F0BB;
                _PI->WriteLoHook(0x42ED78, Ai_WaterwalkFly);    

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C1150+3, 0x636154);

                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                //refugeCampReturnAddress_Skip = 0x4F83C6;
                //refugeCampReturnAddress_Write = 0x4F8194;
                //_PI->WriteLoHook(0x4F8188, fixRefugeCamp);
                // не нужен
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x4486B0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5650D9, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x47367D;
                _PI->WriteLoHook(0x47855E, fixHarpyBinds);
                    
                // fix size for Hero creature labels in Russian versions (adv. map right click)
                _PI->WriteByte(0x51C4E3+1, 12);
                _PI->WriteByte(0x51C56C+1, 12);
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x59D89D, fixArtMerchantPrice);
                _PI->WriteLoHook(0x59D9B7, fixArtMerchantPrice);
                _PI->WriteLoHook(0x59DB21, fixArtMerchantPrice);
                _PI->WriteLoHook(0x59BB84, fixArtMerchantPrice);
                _PI->WriteLoHook(0x59C557, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x55C91E, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // нету 
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x4657A0;
                removeStackFromHexProcAddress = 0x463A90;
                _PI->WriteHiHook(0x4AA957, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x425D8E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x425D8E+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x423770;
                _PI->WriteWord(0x425DD4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x425CD0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                // не нужен
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4D8FD0;
                calcMovementWrapProcAddress = 0x4D93A0;
                //_PI->WriteLoHook(0x497438, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A0BBB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x494BF1, updateMaxLandMovement);
                    
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5950D0, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
            
                // alliance town fixes
                // не нужно
                    
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4A28E5, preserveMonsterNumber);          
                    
                // fix crash
                _PI->WriteHiHook(0x461EC0, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x44191B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
            
            }

            // Heroes Chronicles Beastmaster & Sword - v1.0
            check1 = *(int*)(0x4EB494+1);
            check2 = *(int*)(0x5AF2D9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B2934;
                aH3Main = 0x6388F8;
                aActivePlayer = 0x63C074;
                aH3HeroClass = 0x61C1BC;
                aMarketHero = 0x647F38;
                aMarketBackpackIndexOfFirstSlot = 0x647F30;
                aMarketSelectedSlotIndex= 0x647F50;
                aH3Spell = 0x6253C0;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B04A0;
                beforeGameAddress = 0x4E323C;
                canShootProcAddress = 0x4470A0;             
            
                // Armorer fix
                _PI->WriteByte(0x41D6A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41D7E1+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x461126+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x43FD05, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43DE46, "90 90 90 90 90 90");      

                // что-то неверно
                // The Castle's Lighthouse building bonus
                //castleOwnerCheckReturnAddress = 0x4D8EF2;         
                //_PI->WriteLoHook(0x4D8EC7, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42EE8B;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x42F1AB;
                _PI->WriteLoHook(0x42EE68, Ai_WaterwalkFly);    

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C1410+3, 0x636174);
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                //refugeCampReturnAddress_Skip = 0x4F7D26;
                //refugeCampReturnAddress_Write = 0x4F7AF4;
                //_PI->WriteLoHook(0x4F7AE8, fixRefugeCamp);
                // не нужен
            
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x4483E0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5650C9, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x47363D;
                _PI->WriteLoHook(0x473634, fixHarpyBinds);
                        
                // fix size for Hero creature labels in Russian versions (adv. map right click)
                _PI->WriteByte(0x51C303+1, 12);
                _PI->WriteByte(0x51C38C+1, 12); 
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x59D81D, fixArtMerchantPrice);
                _PI->WriteLoHook(0x59D937, fixArtMerchantPrice);
                _PI->WriteLoHook(0x59DAA1, fixArtMerchantPrice);
                _PI->WriteLoHook(0x59BB04, fixArtMerchantPrice);
                _PI->WriteLoHook(0x59C4D7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x55C8FE, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // нету
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x4656C0;
                removeStackFromHexProcAddress = 0x4639B0;
                _PI->WriteHiHook(0x4AAC27, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x425E9E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x425E9E+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x423880;
                _PI->WriteWord(0x425EE4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x425DE0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                // не нужен
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4D8D90;
                calcMovementWrapProcAddress = 0x4D9160;
                //_PI->WriteLoHook(0x497748, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A0ECB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x494F01, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x595050, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                // не нужно
                    
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4A2BF5, preserveMonsterNumber);          
                    
                // fix crash
                _PI->WriteHiHook(0x461DE0, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
            
                // fix wrong half damage
                _PI->WriteLoHook(0x44166B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
                
            }

            // ------------------------------
            // ENG
            // ------------------------------

            // Heroes III Erathia - v1.4
            check1 = *(int*)(0x4F5583+1);
            check2 = *(int*)(0x5D8F69+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {       
                // Global structures addresses
                aHeroesCount = 0x4BABF5;
                aH3Main = 0x66B490;
                aActivePlayer = 0x66EC44;
                aMarketHero = 0x67CA20;
                aMarketBackpackIndexOfFirstSlot = 0x67CA18;
                aMarketSelectedSlotIndex= 0x67CA38;
                aH3Spell = 0x659F48;
                aNetworkGame = 0x66B4F0;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B8510;
                getMeProcAddress = 0x4CC110;
                getDlgItemProcAddress = 0x5D63C0;
                beforeGameAddress = 0x4EC20B;
                canShootProcAddress = 0x4427A0;
                send2ChatProcAddress = 0x535220;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406E80, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BCAA0, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4BDC0E, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E343+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E47F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x4653D3+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck - не нужен

                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E261C;           
                _PI->WriteLoHook(0x4E25F0, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x4301A1;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x4304B0;
                _PI->WriteLoHook(0x43017E, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CA6C0+3, 0x668D38);

                // AI moat damage fix
                _PI->WriteByte(0x4B2A79, 0xEB);
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                //refugeCampReturnAddress_Skip = 0x502B5F;
                //refugeCampReturnAddress_Write = 0x502931;
                //_PI->WriteLoHook(0x502925, fixRefugeCamp);
            
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449D30, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x58C5D9, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x4780C5;
                _PI->WriteLoHook(0x4780BB, fixHarpyBinds);
                    
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                // не нужен
            
                // fix art merchants price bug
                _PI->WriteLoHook(0x5C6669, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5C67A6, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5C6910, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5C4845, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5C5237, fixArtMerchantPrice);
                    
                // Clone bug fix
                _PI->WriteHiHook(0x583E41, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // не нужен
            
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x469AF0;
                removeStackFromHexProcAddress = 0x467DA0;
                _PI->WriteHiHook(0x4B4424, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
                    
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426F0E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426F0E+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x4247F0;
                _PI->WriteWord(0x426F54, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426E50, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                // не нужен
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4E24B0;
                calcMovementWrapProcAddress = 0x4E28B0;
                //_PI->WriteLoHook(0x4A0608, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AA06B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49DC40, updateMaxLandMovement);
                    
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5BDB80, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);   
                
                // alliance town fixes
                _PI->WriteHiHook(0x5AB690, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
            
                // disguise bug fix
                _PI->WriteLoHook(0x41C766, setDisguise);
                _PI->WriteLoHook(0x4C4759, decDisguise);
                _PI->WriteLoHook(0x52C275, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C5825, "90 90 90");
                    
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A05E6, mpGroundToSea);
                _PI->WriteLoHook(0x49DC34, mpSeaToGround);
                _PI->WriteLoHook(0x4D7B18, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x460DCD, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5AC693, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D7B3, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DBA4, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DCAF, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x415416, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x415583, fixNotMeID_ecx);     //могила воина          
                _PI->WriteLoHook(0x4155F3, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x4157E0, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x452B59, fixNotMePlayer_eax);     //advmgr panel 2
                _PI->WriteLoHook(0x452B59 + 0x17, fixNotMePlayer_edx);  //advmgr panel 2
                _PI->WriteHiHook(0x52F2C0, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x409900, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5AC6A9, "EB 5B");  // fix crash
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4ABEF5, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x4661A0, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x44302B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
                        
            }

            // Heroes III Armageddon - v2.2
            check1 = *(int*)(0x4F5963+1);
            check2 = *(int*)(0x5FFBF9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {           
                // Global structures addresses
                aHeroesCount = 0x4BB535;
                aH3Main = 0x6963C8;
                aActivePlayer = 0x699B88;
                aMarketHero = 0x6A7958;
                aMarketBackpackIndexOfFirstSlot = 0x6A7950;
                aMarketSelectedSlotIndex= 0x6A7970;
                aH3Spell = 0x684E90;
                aNetworkGame = 0x69642C;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B8E30;
                getMeProcAddress = 0x4CCA70;
                getDlgItemProcAddress = 0x5FD060;
                beforeGameAddress = 0x4EC41B;
                canShootProcAddress = 0x4429F0;
                send2ChatProcAddress = 0x552020;
                randintProcAddress = 0x508D20;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406E10, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BD3E0, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4BE56A, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E033+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E16F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x465A53+1, 0x4D); // fdiv -> fmul
            
                // Neutral creatures luck
                _PI->WriteHexPatch(0x441904, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FA22, "90 90 90 90 90 90"); 
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E296C;           
                _PI->WriteLoHook(0x4E2940, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42FE91;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x4301A0;
                _PI->WriteLoHook(0x42FE6E, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CB020+3, 0x693C78);

                // AI moat damage fix
                _PI->WriteByte(0x4B3399, 0xEB);
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x50380F;
                refugeCampReturnAddress_Write = 0x5035E1;
                _PI->WriteLoHook(0x5035D5, fixRefugeCamp);
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449F40, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5AD5F9, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x4789D5;
                _PI->WriteLoHook(0x4789CB, fixHarpyBinds);
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x531DC1+3, 0x0FFFEFBF);
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5EC299, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EC3D6, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EC540, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EA475, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EAE67, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x5A4DD1, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
            
                // fix AI + Angelic Alliance + neutral creatures
                // не нужен
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x46A170;
                removeStackFromHexProcAddress = 0x468420;
                _PI->WriteHiHook(0x4B4D44, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
                    
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426BFE+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426BFE+3, 0xDA89); // MOV EDX,EBX
            
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x4244E0;
                _PI->WriteWord(0x426C44, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426B40, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
                    
                // RMG zone type bug fix
                _PI->WriteWord(0x539C0F, 0x1874);
            
                // Update max. movement points
                calcMovementProcAddress = 0x4E2800;
                calcMovementWrapProcAddress = 0x4E2C00;
                //_PI->WriteLoHook(0x4A0F78, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AA9DB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49E5B0, updateMaxLandMovement);
                    
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5E37B0, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
            
                // alliance town fixes
                _PI->WriteHiHook(0x5D12C0, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
                    
                // disguise bug fix
                _PI->WriteLoHook(0x41C456, setDisguise);
                _PI->WriteLoHook(0x4C50B9, decDisguise);
                _PI->WriteLoHook(0x52C765, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C6185, "90 90 90");
            
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A0F56, mpGroundToSea);
                _PI->WriteLoHook(0x49E5A4, mpSeaToGround);
                _PI->WriteLoHook(0x4D7E68, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x46144D, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5D22C3, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D733, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DB24, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DC2F, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x415396, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x415503, fixNotMeID_ecx);     //могила воина              
                _PI->WriteLoHook(0x415573, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x415760, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x452D69, fixNotMePlayer_eax);   //advmgr panel 2
                _PI->WriteLoHook(0x452D69 + 0x17, fixNotMePlayer_edx);   //advmgr panel 2
                _PI->WriteHiHook(0x54C0B0, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x409880, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5D22D9, "EB 5B");  // fix crash
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AC865, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x466820, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x44327B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x584314, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x548D80, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x549C6B, excludeStartingHeroesRmg);
                                
            }

            // Heroes III Complete - v4.0
            check1 = *(int*)(0x4F7B03+1);
            check2 = *(int*)(0x601B89+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {    
                // Global structures addresses
                aHeroesCount = 0x4BD485;
                aH3Main = 0x6994E8;
                aActivePlayer = 0x69CCB0;
                aMarketHero = 0x6AAA78;
                aMarketBackpackIndexOfFirstSlot = 0x6AAA70;
                aMarketSelectedSlotIndex= 0x6AAA90;
                aH3Spell = 0x687F58;
                aNetworkGame = 0x69954C;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4BADA0;
                getMeProcAddress = 0x4CE9B0;
                getDlgItemProcAddress = 0x5FEFF0;
                beforeGameAddress = 0x4EE77B;
                canShootProcAddress = 0x4428F0;
                send2ChatProcAddress = 0x5547C0;
                randintProcAddress = 0x50B230;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406FD0, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BF330, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4C04CD, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E223+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E35F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x465D63+1, 0x4D); // fdiv -> fmul
            
                // Neutral creatures luck
                _PI->WriteHexPatch(0x441804, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F922, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E4AFC;           
                _PI->WriteLoHook(0x4E4AD1, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x430231;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430540;
                _PI->WriteLoHook(0x43020E, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CCF80+3, 0x696D98);
            
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x5059FF;
                refugeCampReturnAddress_Write = 0x5057D1;
                _PI->WriteLoHook(0x5057C5, fixRefugeCamp);
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449E90, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5AF559, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x478A35;
                _PI->WriteLoHook(0x478A2B, fixHarpyBinds);
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x534141+3, 0x0FFFEFBF);
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5EE219, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EE356, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EE4C0, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EC3F5, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ECDE7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x5A6DB1, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                _PI->WriteLoHook(0x42C89E, fixAngelicAllianceAI);
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x46A4A0;
                removeStackFromHexProcAddress = 0x468730;
                _PI->WriteHiHook(0x4B4E24, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426F9E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426F9E+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424880;
                _PI->WriteWord(0x426FE4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426EE0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
                    
                // RMG zone type bug fix
                _PI->WriteWord(0x53BF8F, 0x1874);
            
                // Update max. movement points
                calcMovementProcAddress = 0x4E4990;
                calcMovementWrapProcAddress = 0x4E4D90;
                //_PI->WriteLoHook(0x4A1098, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AAAFB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49E6D0, updateMaxLandMovement);
                    
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5E5730, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
            
                // alliance town fixes
                _PI->WriteHiHook(0x5D3240, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
                    
                // disguise bug fix
                _PI->WriteLoHook(0x41C643, setDisguise);
                _PI->WriteLoHook(0x4C7019, decDisguise);
                _PI->WriteLoHook(0x52EAD5, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C80E5, "90 90 90");
                    
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A1076, mpGroundToSea);
                _PI->WriteLoHook(0x49E6C4, mpSeaToGround);
                _PI->WriteLoHook(0x4D9FF8, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x46175D, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5D4243, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D923, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DD14, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DE1F, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x415586, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x4156F3, fixNotMeID_ecx);     //могила воина              
                _PI->WriteLoHook(0x415763, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x415950, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x452CB9, fixNotMePlayer_eax);     //advmgr panel 2
                _PI->WriteLoHook(0x452CB9 + 0x17, fixNotMePlayer_edx);  //advmgr panel 2
                _PI->WriteHiHook(0x54E850, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x409A70, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5D4259, "EB 5B");  // fix crash
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AC905, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x466B30, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x44317B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x586364, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x54B100, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x54BFEB, excludeStartingHeroesRmg);
                                
            }

            // Heroes Chronicles Warlords & Underworld - v1.0
            check1 = *(int*)(0x4F0033+1);
            check2 = *(int*)(0x5B5129+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B6094;
                aH3Main = 0x63B6C0;
                aActivePlayer = 0x63EE60;
                aMarketHero = 0x64AD18;
                aMarketBackpackIndexOfFirstSlot = 0x64AD10;
                aMarketSelectedSlotIndex= 0x64AD30;
                aH3Spell = 0x62A198;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B3B10;
                beforeGameAddress = 0x4E75EB;
                canShootProcAddress = 0x442E20;         
            
                // Armorer fix
                _PI->WriteByte(0x41E1A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E2DF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x46364C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441D34, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FE52, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus - другой код 0x4C0BF6

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42FF31;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430240;
                _PI->WriteLoHook(0x42FF0E, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C5160+3, 0x638F40);
            
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                // не нужен
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x44A4C0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x569F39, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x476445;
                _PI->WriteLoHook(0x47643B, fixHarpyBinds);
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                // не нужен
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5A35B9, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A36F6, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3860, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A1795, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A2187, fixArtMerchantPrice);
                    
                // Clone bug fix
                _PI->WriteHiHook(0x561771, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
            
                // fix AI + Angelic Alliance + neutral creatures
                // нету 
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x467CA0;
                removeStackFromHexProcAddress = 0x465F70;
                _PI->WriteHiHook(0x4ADDE4, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
                    
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426D1E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426D1E+3, 0xDA89); // MOV EDX,EBX
            
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424880;
                _PI->WriteWord(0x426D64, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x424600, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
                    
                // RMG zone type bug fix
                // не нужен
            
                // Update max. movement points
                calcMovementProcAddress = 0x4DCA30;
                calcMovementWrapProcAddress = 0x4DCE00;
                //_PI->WriteLoHook(0x49A4D8, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A3EDB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x497B10, updateMaxLandMovement);
                    
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x59AAD0, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                // не нужен
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4A5D12, preserveMonsterNumber);          
                    
                // fix crash
                _PI->WriteHiHook(0x464330, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x4436AB, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
                        
            }

            // Heroes Chronicles Elements & Dragons - v1.0
            check1 = *(int*)(0x4EFE04+1);
            check2 = *(int*)(0x5B5469+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B5AC4;
                aH3Main = 0x63B6B8;
                aActivePlayer = 0x63EE5C;
                aMarketHero = 0x64AD30;
                aMarketBackpackIndexOfFirstSlot = 0x64AD28;
                aMarketSelectedSlotIndex= 0x64AD48;
                aH3Spell = 0x62A198;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B3540;
                beforeGameAddress = 0x4E753B;
                canShootProcAddress = 0x442830;             
            
                // Armorer fix
                _PI->WriteByte(0x41E1C3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E2FF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x4632CC+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441744, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F862, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus - другой код 0x4C0626

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x430011;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430320;
                _PI->WriteLoHook(0x42FFEE, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C4B90+3, 0x638F40);
            
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                // не нужен
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449EE0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x56A1F9, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x475F05;
                _PI->WriteLoHook(0x475EFB, fixHarpyBinds);  
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                // не нужен
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5A38E9, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3A26, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3B90, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A1AC5, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A24B7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x561A11, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // нету
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x467920;
                removeStackFromHexProcAddress = 0x465BF0;
                _PI->WriteHiHook(0x4AD854, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426DFE+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426DFE+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x4246E0;
                _PI->WriteWord(0x426E44, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426D40, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                // не нужен
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4DC9F0;
                calcMovementWrapProcAddress = 0x4DCDC0;
                //_PI->WriteLoHook(0x499FF8, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A39FB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x497630, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x59AE00, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                // не нужен
                    
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4A57B2, preserveMonsterNumber);          
            
                // fix crash
                _PI->WriteHiHook(0x463FB0, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x4430BB, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
                        
            }

            // Heroes Chronicles WorldTree - v1.0
            check1 = *(int*)(0x4EFA84+1);
            check2 = *(int*)(0x5B51B9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B59E4;
                aH3Main = 0x63B6C0;
                aActivePlayer = 0x63EE64;
                aMarketHero = 0x64AD30;
                aMarketBackpackIndexOfFirstSlot = 0x64AD28;
                aMarketSelectedSlotIndex= 0x64AD48;
                aH3Spell = 0x62A190;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B3460;
                beforeGameAddress = 0x4E703B;
                canShootProcAddress = 0x4429E0;         
            
                // Armorer fix
                _PI->WriteByte(0x41E163+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E29F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x46326C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x4418F4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FA12, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus - не нужно

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42FEE1;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x4301F0;
                _PI->WriteLoHook(0x42FEBE, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C4AF0+3, 0x638F40);
            
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                // не нужен
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x44A0B0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x569F19, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x475D95;
                _PI->WriteLoHook(0x475D8B, fixHarpyBinds);  
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                // не нужен 
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5A3629, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3766, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A38D0, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A1805, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A21F7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x561751, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // нету 
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x4678C0;
                removeStackFromHexProcAddress = 0x465B90;
                _PI->WriteHiHook(0x4AD7E4, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426CDE+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426CDE+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x4245C0;
                _PI->WriteWord(0x426D24, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426C20, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                // не нужен
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4DC4A0;
                calcMovementWrapProcAddress = 0x4DC870;
                //_PI->WriteLoHook(0x499D88, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A378B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x4973C0, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x59AB40, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                // не нужен
                    
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4A5542, preserveMonsterNumber);          
                    
                // fix crash
                _PI->WriteHiHook(0x463F50, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
            
                // fix wrong half damage
                _PI->WriteLoHook(0x44326B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
                        
            }

            // Heroes Chronicles FieryMoon - v1.0
            check1 = *(int*)(0x4EF824+1);
            check2 = *(int*)(0x5B5249+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B56D4;
                aH3Main = 0x63DAC0;
                aActivePlayer = 0x641260;
                aMarketHero = 0x64D130;
                aMarketBackpackIndexOfFirstSlot = 0x64D128;
                aMarketSelectedSlotIndex= 0x64D148;
                aH3Spell = 0x62C588;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B3150;
                beforeGameAddress = 0x4E6F4B;
                canShootProcAddress = 0x442510;         
            
                // Armorer fix
                _PI->WriteByte(0x41E183+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E2BF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x462CDC+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441424, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F542, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus - не нужно

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42FF01;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430210;
                _PI->WriteLoHook(0x42FEDE, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C4810+3, 0x63B338);
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                // не нужен
            
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449BB0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x56A1C9, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x4758A5;
                _PI->WriteLoHook(0x47589B, fixHarpyBinds);  
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                // не нужен 
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5A3869, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A39A6, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3B10, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A1A45, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A2437, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x5619E1, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // нету
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x467330;
                removeStackFromHexProcAddress = 0x465600;
                _PI->WriteHiHook(0x4AD494, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426CFE+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426CFE+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x4245E0;
                _PI->WriteWord(0x426D44, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426C40, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                // не нужен
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4DC620;
                calcMovementWrapProcAddress = 0x4DC9F0;
                //_PI->WriteLoHook(0x4997A8, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A31AB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x496DE0, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x59AD80, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                // не нужен
                    
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4A4FE2, preserveMonsterNumber);          
            
                // fix crash
                _PI->WriteHiHook(0x4639C0, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x442D9B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
                       
            }

            // Heroes Chronicles Beastmaster & Sword - v1.0
            check1 = *(int*)(0x4EF874+1);
            check2 = *(int*)(0x5B4C09+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B5514;
                aH3Main = 0x63EB28;
                aActivePlayer = 0x6422CC;
                aMarketHero = 0x64E1A8;
                aMarketBackpackIndexOfFirstSlot = 0x64E1A0;
                aMarketSelectedSlotIndex= 0x64E1C0;
                aH3Spell = 0x62B5C8;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B2F90;
                beforeGameAddress = 0x4E6E0B;
                canShootProcAddress = 0x442600;         
            
                // Armorer fix
                _PI->WriteByte(0x41E213+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E34F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x462E5C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441514, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F632, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus - другой код 0x4BFF96

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42FF69;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430279;
                _PI->WriteLoHook(0x42FF46, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C4520+3, 0x63C3A8);
            
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                // не нужен
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449CB0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);          
                _PI->WriteLoHook(0x569BB9, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x475B25;
                _PI->WriteLoHook(0x475B1B, fixHarpyBinds);  
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                // не нужен 
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5A3229, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3366, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A34D0, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A1405, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A1DF7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x5612E1, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // нету 
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x4674E0;
                removeStackFromHexProcAddress = 0x465780;
                _PI->WriteHiHook(0x4AD614, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426D8E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426D8E+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424670;
                _PI->WriteWord(0x426DD4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426CD0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                // не нужен
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4DC530;
                calcMovementWrapProcAddress = 0x4DC900;
                //_PI->WriteLoHook(0x499908, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A332B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x496F40, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x59A740, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                // не нужен
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4A5162, preserveMonsterNumber);          
                    
                // fix crash
                _PI->WriteHiHook(0x463B40, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x442E8B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
                                            
            }

            // ------------------------------
            // USA
            // ------------------------------

            // Heroes Chronicles Beastmaster & Sword - v1.0
            check1 = *(int*)(0x4EF914+1);
            check2 = *(int*)(0x5B51B9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B5A44;
                aH3Main = 0x63FB20;
                aActivePlayer = 0x6432C0;
                aMarketHero = 0x64F190;
                aMarketBackpackIndexOfFirstSlot = 0x64F188;
                aMarketSelectedSlotIndex= 0x64F1A8;
                aH3Spell = 0x62C5C0;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B3480;
                beforeGameAddress = 0x4E703B;
                canShootProcAddress = 0x442AD0;             
            
                // Armorer fix
                _PI->WriteByte(0x41E433+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E56F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x46345C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x4419E4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FB02, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus - другой код 0x4C05A6

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x4301B1;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x4304C0;
                _PI->WriteLoHook(0x43018E, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C4B40+3, 0x63D3A0);
            
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                // не нужен
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x44A170, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);          
                _PI->WriteLoHook(0x56A149, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x476035;
                _PI->WriteLoHook(0x47602B, fixHarpyBinds);  
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                // не нужен
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5A37D9, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3916, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3A80, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A19B5, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A23A7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x5619B1, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // нету
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x467AB0;
                removeStackFromHexProcAddress = 0x465D80;
                _PI->WriteHiHook(0x4AD764, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426FAE+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426FAE+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424890;
                _PI->WriteWord(0x426FF4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426EF0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                // не нужен
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4DC710;
                calcMovementWrapProcAddress = 0x4DCAE0;
                //_PI->WriteLoHook(0x499C78, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A367B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x4972B0, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x59ACF0, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
            
                // alliance town fixes
                // не нужен
                    
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4A54B2, preserveMonsterNumber);          
                    
                // fix crash
                _PI->WriteHiHook(0x464140, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x44335B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
                        
            }

            // ------------------------------
            // GER
            // ------------------------------

            // Heroes Chronicles Dragons - GOG - v1.0
            check1 = *(int*)(0x4EFA04+1);
            check2 = *(int*)(0x5B51C9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4B5284;
                aH3Main = 0x63DB08;
                aActivePlayer = 0x6412B0;
                aMarketHero = 0x64D180;
                aMarketBackpackIndexOfFirstSlot = 0x64D178;
                aMarketSelectedSlotIndex= 0x64D198;
                aH3Spell = 0x62C5D0;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B2D00;
                beforeGameAddress = 0x4E712B;
                canShootProcAddress = 0x442440;         
            
                // Armorer fix
                _PI->WriteByte(0x41E0F3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E22F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x462C0C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441354, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F472, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus - другой код 0x4BFE26

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42FE71;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430180;
                _PI->WriteLoHook(0x42FE4E, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4C43C0+3, 0x63B380);
            
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                // не нужен
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449AE0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x56A149, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x475795;
                _PI->WriteLoHook(0x47578B, fixHarpyBinds);  
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                // не нужен
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5A37E9, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3926, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A3A90, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A19C5, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5A23B7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x561961, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // нету
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x467260;
                removeStackFromHexProcAddress = 0x465530;
                _PI->WriteHiHook(0x4AD044, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426C6E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426C6E+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424550;
                _PI->WriteWord(0x426CB4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426BB0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                // не нужен
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4DC800;
                calcMovementWrapProcAddress = 0x4DCBD0;
                //_PI->WriteLoHook(0x499768, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4A316B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x496DA0, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x59AD00, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                // не нужен
                    
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4A4FA2, preserveMonsterNumber);          
            
                // fix crash
                _PI->WriteHiHook(0x4638F0, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x442CCB, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);
                        
            }

            // ------------------------------
            // FRA
            // ------------------------------

            // Heroes III Armageddon - v2.1
            check1 = *(int*)(0x4F61C3+1);
            check2 = *(int*)(0x6003D9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4BB685;
                aH3Main = 0x6964E8;
                aActivePlayer = 0x699CAC;
                aMarketHero = 0x6A7A90;
                aMarketBackpackIndexOfFirstSlot = 0x6A7A88;
                aMarketSelectedSlotIndex= 0x6A7AA8;
                aH3Spell = 0x684F68;
                aNetworkGame = 0x69654C;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B8F90;
                getMeProcAddress = 0x4CCBC0;
                getDlgItemProcAddress = 0x5FD840;
                beforeGameAddress = 0x4ECE0B;
                canShootProcAddress = 0x442A00;
                send2ChatProcAddress = 0x552960;
                randintProcAddress = 0x5095D0;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406EB0, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BD530, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4BE6BA, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E143+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E27F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x465CB3+1, 0x4D); // fdiv -> fmul
            
                // Neutral creatures luck
                _PI->WriteHexPatch(0x441914, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FA32, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E307C;           
                _PI->WriteLoHook(0x4E3050, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42FF81;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430290;
                _PI->WriteLoHook(0x42FF5E, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CB170+3, 0x693D98);

                // AI moat damage fix
                _PI->WriteByte(0x4B34D9, 0xEB);
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x50402F;
                refugeCampReturnAddress_Write = 0x503E01;
                _PI->WriteLoHook(0x503DF5, fixRefugeCamp);
                    
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449F50, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5ADD19, ghostHeroFix);
            
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x478995;
                _PI->WriteLoHook(0x47898B, fixHarpyBinds);
                    
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x532691+3, 0x0FFFEFBF);
            
                // fix double cast
                fixDoubleCastReturnAddress = 0x59D13F;
                _PI->WriteLoHook(0x59D108, fixDoubleCast);
                    
                // fix art merchants price bug
                _PI->WriteLoHook(0x5ECA19, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ECB56, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ECCC0, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EABF5, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EB5E7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x5A5521, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                // нету
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x46A3D0;
                removeStackFromHexProcAddress = 0x468680;
                _PI->WriteHiHook(0x4B4E84, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426CDE+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426CDE+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x4245C0;
                _PI->WriteWord(0x426D24, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426C20, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
            
                // RMG zone type bug fix
                _PI->WriteWord(0x53A4CF, 0x1874);
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4E2F10;
                calcMovementWrapProcAddress = 0x4E3310;
                //_PI->WriteLoHook(0x4A0F48, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AA9AB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49E580, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5E3F30, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                _PI->WriteHiHook(0x5D1A40, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
                    
                // disguise bug fix
                _PI->WriteLoHook(0x41C566, setDisguise);
                _PI->WriteLoHook(0x4C5209, decDisguise);
                _PI->WriteLoHook(0x52D035, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C62D5, "90 90 90");
            
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A0F26, mpGroundToSea);
                _PI->WriteLoHook(0x49E574, mpSeaToGround);
                _PI->WriteLoHook(0x4D85A8, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x4616AD, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5D2A43, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D7D3, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DBC4, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DCCF, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x415436, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x4155A3, fixNotMeID_ecx);     //могила воина          
                _PI->WriteLoHook(0x415613, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x415800, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x452DA9, fixNotMePlayer_eax);     //advmgr panel 2
                _PI->WriteLoHook(0x452DA9 + 0x17, fixNotMePlayer_edx);  //advmgr panel 2
                _PI->WriteHiHook(0x54CA00, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x409920, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5D2A59, "EB 5B");  // fix crash
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AC7B5, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x466A80, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x44328B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x5845A4, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x549640, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x54A52B, excludeStartingHeroesRmg);
                        
            }

            // Heroes III Shadow - v3.1
            check1 = *(int*)(0x4F8163+1);
            check2 = *(int*)(0x6028F9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4BD545;
                aH3Main = 0x69A618;
                aActivePlayer = 0x69DDD8;
                aMarketHero = 0x6ABBB0;
                aMarketBackpackIndexOfFirstSlot = 0x6ABBA8;
                aMarketSelectedSlotIndex= 0x6ABBC8;
                aH3Spell = 0x689068;
                aNetworkGame = 0x69A67C;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4BAE70;
                getMeProcAddress = 0x4CEA70;
                getDlgItemProcAddress = 0x5FFD50;
                beforeGameAddress = 0x4EEDDB;
                canShootProcAddress = 0x442DE0;
                send2ChatProcAddress = 0x554F60;
                randintProcAddress = 0x50CAC0;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x407010, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BF3F0, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4C058D, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E413+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E54F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x4661B3+1, 0x4D); // fdiv -> fmul
            
                // Neutral creatures luck
                _PI->WriteHexPatch(0x441CF4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FE12, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E4A4C;           
                _PI->WriteLoHook(0x4E4A20, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x430251;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430560;
                _PI->WriteLoHook(0x43022E, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CD040+3, 0x697EC8);
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x505FEF;
                refugeCampReturnAddress_Write = 0x505DC1;
                _PI->WriteLoHook(0x505DB5, fixRefugeCamp);
            
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x44A340, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5B02C9, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x478BF5;
                _PI->WriteLoHook(0x478BEB, fixHarpyBinds);
            
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x534EA1+3, 0x0FFFEFBF);
                    
                // fix double cast
                fixDoubleCastReturnAddress = 0x59F72F;
                _PI->WriteLoHook(0x59F6F8, fixDoubleCast);
            
                // fix art merchants price bug
                _PI->WriteLoHook(0x5EEFD9, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EF116, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EF280, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ED1B5, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EDBA7, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x5A7B21, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                _PI->WriteLoHook(0x42C8BE, fixAngelicAllianceAI);
            
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x46A8F0;
                removeStackFromHexProcAddress = 0x468B80;
                _PI->WriteHiHook(0x4B4F24, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
                    
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426FAE+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426FAE+3, 0xDA89); // MOV EDX,EBX
            
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424890;
                _PI->WriteWord(0x426FF4, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426EF0, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
                    
                // RMG zone type bug fix
                _PI->WriteWord(0x53CCEF, 0x1874);
                    
                // Update max. movement points
                calcMovementProcAddress = 0x4E48E0;
                calcMovementWrapProcAddress = 0x4E4CE0;
                //_PI->WriteLoHook(0x4A11D8, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AAC3B, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49E810, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5E64F0, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
            
                // alliance town fixes
                _PI->WriteHiHook(0x5D4000, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
                    
                // disguise bug fix
                _PI->WriteLoHook(0x41C836, setDisguise);
                _PI->WriteLoHook(0x4C70D9, decDisguise);
                _PI->WriteLoHook(0x52F835, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C81A5, "90 90 90");
            
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A11B6, mpGroundToSea);
                _PI->WriteLoHook(0x49E804, mpSeaToGround);
                _PI->WriteLoHook(0x4D9F78, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x461BAD, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5D5003, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D933, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DD24, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DE2F, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x415596, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x415703, fixNotMeID_ecx);     //могила воина          
                _PI->WriteLoHook(0x415773, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x415960, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x453199, fixNotMePlayer_eax);     //advmgr panel 2
                _PI->WriteLoHook(0x453199 + 0x17, fixNotMePlayer_edx);  //advmgr panel 2
                _PI->WriteHiHook(0x54F000, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x409A80, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5D5019, "EB 5B");  // fix crash
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4ACA45, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x466F80, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x44366B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x586B14, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x54BE60, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x54CD4B, excludeStartingHeroesRmg);
                        
            }

            // ------------------------------
            // POL
            // ------------------------------

            // Heroes III Shadow - v3.2
            check1 = *(int*)(0x4F78D3+1);
            check2 = *(int*)(0x602179+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4BCA44;
                aH3Main = 0x69C570;
                aActivePlayer = 0x69FD30;
                aMarketHero = 0x6ADB18;
                aMarketBackpackIndexOfFirstSlot = 0x6ADB10;
                aMarketSelectedSlotIndex= 0x6ADB30;
                aH3Spell = 0x688FB0;
                aNetworkGame = 0x69C5D4;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4BA360;
                getMeProcAddress = 0x4CE050;
                getDlgItemProcAddress = 0x5FF5F0;
                beforeGameAddress = 0x4EE53B;
                canShootProcAddress = 0x4425D0;
                send2ChatProcAddress = 0x554040;
                randintProcAddress = 0x50B070;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406FD0, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BE8E0, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4BFA7D, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E353+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E48F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x465973+1, 0x4D); // fdiv -> fmul
           
                // Neutral creatures luck
                _PI->WriteHexPatch(0x4414E4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F602, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E44AC;           
                _PI->WriteLoHook(0x4E4480, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x430159;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x430469;
                _PI->WriteLoHook(0x430136, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CC630+3, 0x699E20);
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x505668;
                refugeCampReturnAddress_Write = 0x505661;
                _PI->WriteLoHook(0x505659, fixRefugeCamp);
            
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449B70, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
                _PI->WriteLoHook(0x5AF999, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x478215;
                _PI->WriteLoHook(0x47820B, fixHarpyBinds);
                    
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x533D71+3, 0x0FFFEFBF);
            
                // fix art merchants price bug
                _PI->WriteLoHook(0x5EE879, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EE9B6, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EEB20, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ECA55, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5ED447, fixArtMerchantPrice);
                    
                // Clone bug fix
                _PI->WriteHiHook(0x5A70B1, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
            
                // fix AI + Angelic Alliance + neutral creatures
                _PI->WriteLoHook(0x42C7EE, fixAngelicAllianceAI);
                    
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x46A0E0;
                removeStackFromHexProcAddress = 0x468340;
                _PI->WriteHiHook(0x4B4794, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
            
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426EEE+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426EEE+3, 0xDA89); // MOV EDX,EBX
                    
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x4247D0;
                _PI->WriteWord(0x426F34, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426E30, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
                    
                // RMG zone type bug fix
                _PI->WriteWord(0x53BBCF, 0x1874);
            
                // Update max. movement points
                calcMovementProcAddress = 0x4E4340;
                calcMovementWrapProcAddress = 0x4E4740;
                //_PI->WriteLoHook(0x4A0968, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AA3EB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49DF90, updateMaxLandMovement);
                    
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5E5D90, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
            
                // alliance town fixes
                _PI->WriteHiHook(0x5D38A0, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
                    
                // disguise bug fix
                _PI->WriteLoHook(0x41C776, setDisguise);
                _PI->WriteLoHook(0x4C6699, decDisguise);
                _PI->WriteLoHook(0x52E725, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C7765, "90 90 90");
            
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A0946, mpGroundToSea);
                _PI->WriteLoHook(0x49DF84, mpSeaToGround);
                _PI->WriteLoHook(0x4D9968, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x46128D, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5D48A3, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40DA43, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DE34, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DF3F, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x4156A6, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x415813, fixNotMeID_ecx);     //могила воина              
                _PI->WriteLoHook(0x415883, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x415A70, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x4529B9, fixNotMePlayer_eax);   //advmgr panel 2
                _PI->WriteLoHook(0x4529B9 + 0x17, fixNotMePlayer_edx);   //advmgr panel 2
                _PI->WriteHiHook(0x54E0D0, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x409AC0, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5D48B9, "EB 5B");  // fix crash
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AC275, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x466740, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x442E5B, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x5860A4, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x54AD20, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x54BC0B, excludeStartingHeroesRmg);
                        
            }

            // Heroes III Shadow - v3.2 / Armageddon - v2.2
            check1 = *(int*)(0x4F5993+1);
            check2 = *(int*)(0x5FE337+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Global structures addresses
                aHeroesCount = 0x4BAF75;
                aH3Main = 0x698410;
                aActivePlayer = 0x69BBD0;
                aMarketHero = 0x6A9998;
                aMarketBackpackIndexOfFirstSlot = 0x6A9990;
                aMarketSelectedSlotIndex= 0x6A99B0;
                aH3Spell = 0x684E98;
                aNetworkGame = 0x698474;
                    
                // Important proc addresses
                isHumanProcAddress = 0x4B8870;
                getMeProcAddress = 0x4CC530;
                getDlgItemProcAddress = 0x5FCE00;
                beforeGameAddress = 0x4EC5DB;
                canShootProcAddress = 0x442850;
                send2ChatProcAddress = 0x551A10;
                randintProcAddress = 0x50A610;

                // Chat message - info about Mod and options used
                _PI->WriteLoHook(0x406E00, send2Chat);
                // Для Загрузить игру
                _PI->WriteLoHook(0x4BCE10, setChatTrigger);
                // Для Новой игры
                _PI->WriteLoHook(0x4BDF9A, setChatTrigger);
            
                // Armorer fix
                _PI->WriteByte(0x41E1A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E2DF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x465903+1, 0x4D); // fdiv -> fmul
            
                // Neutral creatures luck
                _PI->WriteHexPatch(0x441764, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F883, "90 90 90 90 90 90");
            
                // The Castle's Lighthouse building bonus
                castleOwnerCheckReturnAddress = 0x4E2B4C;           
                _PI->WriteLoHook(0x4E2B20, castleOwnerCheck);

                // Prevents AI from casting Fly if they don't have it.
                Ai_WaterwalkFlyReturnAddress_Cast = 0x42FFA9;
                Ai_WaterwalkFlyReturnAddress_Skip = 0x4302B9;
                _PI->WriteLoHook(0x42FF86, Ai_WaterwalkFly);

                // Tavern rumors bug fix
                _PI->WriteDword(0x4CAB10+3, 0x695CB8);
                    
                // RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
                refugeCampReturnAddress_Skip = 0x503859;
                refugeCampReturnAddress_Write = 0x50362A;
                _PI->WriteLoHook(0x50361E, fixRefugeCamp);
            
                // fix ghost hero (without army)
                //_PI->WriteHiHook(0x449DA0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);          
                _PI->WriteLoHook(0x5AD119, ghostHeroFix);
                    
                // fix Harpy fly after Dendroid bind
                fixHarpyBindsReturnAddress = 0x478275;
                _PI->WriteLoHook(0x47826B, fixHarpyBinds);
                    
                // fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
                _PI->WriteDword(0x5317A1+3, 0x0FFFEFBF);
            
                // fix art merchants price bug
                _PI->WriteLoHook(0x5EC069, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EC1A6, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EC310, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EA245, fixArtMerchantPrice);
                _PI->WriteLoHook(0x5EAC37, fixArtMerchantPrice);
            
                // Clone bug fix
                _PI->WriteHiHook(0x5A4831, CALL_, EXTENDED_, THISCALL_, disableCloneOverlay);
                    
                // fix AI + Angelic Alliance + neutral creatures
                _PI->WriteLoHook(0x42C63E, fixAngelicAllianceAI);
            
                // fix of terrible bug in QuickBattle
                isHiddenBattleProcAddress = 0x46A050;
                removeStackFromHexProcAddress = 0x4682D0;
                _PI->WriteHiHook(0x4B47C4, CALL_, EXTENDED_, THISCALL_, fixQuickBattleStackFly);
                    
                // fix artifacts transfer from loser to winner after AI vs AI battle
                _PI->WriteByte(0x426D3E+1, 0x4E); // MOV ECX,DWORD PTR DS:[ESI+24]
                _PI->WriteWord(0x426D3E+3, 0xDA89); // MOV EDX,EBX
            
                // fix necromancy bug AI vs AI battle
                afterTheorBattleProcAddress = 0x424620;
                _PI->WriteWord(0x426D84, 0x07EB); // doesn't delete loser army
                _PI->WriteHiHook(0x426C80, SPLICE_, EXTENDED_, THISCALL_, fixAINecromancy);
                    
                // RMG zone type bug fix
                _PI->WriteWord(0x53960F, 0x1874);
            
                // Update max. movement points
                calcMovementProcAddress = 0x4E29E0;
                calcMovementWrapProcAddress = 0x4E2DE0;
                //_PI->WriteLoHook(0x4A0928, updateMaxSeaMovement);
                _PI->WriteLoHook(0x4AA3AB, recalculateMovementAfterVisitObject);
                _PI->WriteLoHook(0x49DF60, updateMaxLandMovement);
            
                // fix artifact merchants critical bug
                _PI->WriteHiHook(0x5E3580, SPLICE_, EXTENDED_, THISCALL_, fixBackpackArtMerchantDlg);
                    
                // alliance town fixes
                _PI->WriteHiHook(0x5D1090, SPLICE_, EXTENDED_, THISCALL_, fixAllianceTown); // markets
                    
                // disguise bug fix
                _PI->WriteLoHook(0x41C5C6, setDisguise);
                _PI->WriteLoHook(0x4C4B79, decDisguise);
                _PI->WriteLoHook(0x52C155, makeHeroRMCopy);
                _PI->WriteHexPatch(0x4C5C45, "90 90 90");
            
                // Admiral's hat bug fix + movement fixes
                _PI->WriteLoHook(0x4A0906, mpGroundToSea);
                _PI->WriteLoHook(0x49DF54, mpSeaToGround);
                _PI->WriteLoHook(0x4D8008, killedHeroSeaToGround);
            
                // not me bugs
                _PI->WriteLoHook(0x46121D, fixNotMeID_eax); //townhall
                _PI->WriteLoHook(0x5D2093, fixNotMePlayer_eax); //townqvbk          
                _PI->WriteLoHook(0x40D863, fixNotMeID_ecx); //shrine of magic
                _PI->WriteLoHook(0x40DC54, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x40DD5F, fixNotMeID_ecx); //witch hut
                _PI->WriteLoHook(0x4154C6, fixNotMeID_ecx);     //вагон
                _PI->WriteLoHook(0x415633, fixNotMeID_ecx);     //могила воина          
                _PI->WriteLoHook(0x4156A3, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x415890, fixNotMeID_ecx);     //мельница
                _PI->WriteLoHook(0x452BA9, fixNotMePlayer_eax);   //advmgr panel 2
                _PI->WriteLoHook(0x452BA9 + 0x17, fixNotMePlayer_edx);   //advmgr panel 2
                _PI->WriteHiHook(0x54BA90, SPLICE_, EXTENDED_, THISCALL_, fixBuyCreaturesDlg); //buy creatures
                _PI->WriteHiHook(0x4098E0, SPLICE_, EXTENDED_, THISCALL_, fixAdvMgrButton);
                _PI->WriteCodePatch(0x5D20A9, "EB 5B");  // fix crash
            
                // prevent upgraded stack disappearing
                _PI->WriteLoHook(0x4AC235, preserveMonsterNumber);
                    
                // fix crash
                _PI->WriteHiHook(0x4666D0, SPLICE_, EXTENDED_, THISCALL_, fixRemoveObstacle);
                    
                // fix wrong half damage
                _PI->WriteLoHook(0x4430DB, fixForgetfullnessDamage);

                // Data from TXTs corrections
                _PI->WriteLoHook(beforeGameAddress, changeStatsFromTxt);

                // Подготавливаем информацию о стартовых героях игроков.
                _PI->WriteLoHook(0x583B54, preselectHeroesRmg);
                // Генерация случайного героя для случайной карты
                _PI->WriteHiHook(0x548760, SPLICE_, EXTENDED_, THISCALL_, getRandHeroRmg);
                // Убираем стартовых героев из доступных для генерации в тюрьму.
                _PI->WriteLoHook(0x54964B, excludeStartingHeroesRmg);
                        
            }

        }
    }

   return TRUE;
}

