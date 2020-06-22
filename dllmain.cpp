#include "..\..\include\patcher_x86.hpp"
#include "H3Structures.h"
#include "H3Numerations.h"

Patcher* _P;
PatcherInstance* _PI;

static _bool_ plugin_On = 0;

bool bukaComplete = false;

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
    int offsetRoots = 696;
    if(bukaComplete)
	offsetRoots += 4;

    if (*(int*)(c->ebx + offsetRoots))
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

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    if ( DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        if (!plugin_On)
        {
            plugin_On = 1;       
            _P = GetPatcher();
            _PI = _P->CreateInstance("GLWrapper.Asi.BugFixes");

            int check1, check2;

            // for HoMM SoD 3.2 (eng)
            check1 = *(int*)(0x4F8193+1);
            check2 = *(int*)(0x602149+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
		    
		// Global structures addresses
		o_H3HeroProperties = *(H3HeroProperties**)0x67DCEC;		    
		    
                // Armorer fix
                _PI->WriteByte(0x41E3A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41E4DF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x465943+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck 6 NOP
                //_PI->WriteCodePatch(0x441524, "%n", 6);
                //_PI->WriteCodePatch(0x43F642, "%n", 6);
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
		_PI->WriteDword(0x4CCC43, 0x696DE8);
				
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
		o_H3HeroProperties = *(H3HeroProperties**)0x673AFC;
		    
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
		_PI->WriteDword(0x4C8550, 0x68E77C);

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
		    		    
            }

            // Heroes III Armageddon - v2.2 Buka
            check1 = *(int*)(0x4F2863+1);
            check2 = *(int*)(0x5F9609+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
		// Global structures addresses
		o_H3HeroProperties = *(H3HeroProperties**)0x673AFC;
		    
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
		_PI->WriteDword(0x4C87E0, 0x68E7CC);

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
		    	    
            }

            // Heroes III Shadow - v3.1 Buka
            check1 = *(int*)(0x4F7EB3+1);
            check2 = *(int*)(0x602379+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
		    
		// Global structures addresses
		o_H3HeroProperties = *(H3HeroProperties**)0x67CD0C;	
		    
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
		_PI->WriteDword(0x4CCD40, 0x695E08);
		    
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
		    		    
            }

            // Heroes III Complete - v4.0 Buka
            check1 = *(int*)(0x4F7EB3+1);
            check2 = *(int*)(0x6021A9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
		bukaComplete = true;
		    
		// Global structures addresses
		o_H3HeroProperties = *(H3HeroProperties**)0x67EFDC;		    
		    
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
		_PI->WriteDword(0x4CCFE0, 0x69A538);
		    
		// RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
		refugeCampReturnAddress_Skip = 0x505DA9;
		refugeCampReturnAddress_Write = 0x505B7A;
		_PI->WriteLoHook(0x505B6E, fixRefugeCamp);
		    
		// fix ghost hero (without army)
		//_PI->WriteHiHook(0x449BE0, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
		_PI->WriteLoHook(0x5AFA69, ghostHeroFix);
		    
		// fix Harpy fly after Dendroid bind    
		fixHarpyBindsReturnAddress = 0x478568;
		_PI->WriteLoHook(0x47855E, fixHarpyBinds);
		    
		// fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
		_PI->WriteDword(0x534031+3, 0x0FFFEFBF);		    
		    
            }

            // Heroes Chronicles Warlords & Underworld & Elements & Dragons - v1.0
            check1 = *(int*)(0x4EBA34+1);
            check2 = *(int*)(0x5AF329+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
		// Global structures addresses
		o_H3HeroProperties = *(H3HeroProperties**)0x61C194;
		    
                // Armorer fix
                _PI->WriteByte(0x41D5F3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41D731+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x461206+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x43E0F6, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FFB5, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus
		castleOwnerCheckReturnAddress = 0x4D9132;		    
		_PI->WriteLoHook(0x4D9107, castleOwnerCheck);

		// Prevents AI from casting Fly if they don't have it.
		Ai_WaterwalkFlyReturnAddress_Cast = 0x42ED9B;
		Ai_WaterwalkFlyReturnAddress_Skip = 0x42F0BB;
		_PI->WriteLoHook(0x42ED78, Ai_WaterwalkFly);	

		// Tavern rumors bug fix
		_PI->WriteDword(0x4C1150, 0x636154);

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
		    
            }

            // Heroes Chronicles Beastmaster & Sword - v1.0
            check1 = *(int*)(0x4EB494+1);
            check2 = *(int*)(0x5AF2D9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
		// Global structures addresses
		o_H3HeroProperties = *(H3HeroProperties**)0x61C1BC;
		    
                // Armorer fix
                _PI->WriteByte(0x41D6A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x41D7E1+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x461126+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x43FD05, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43DE46, "90 90 90 90 90 90");      
		    
		// The Castle's Lighthouse building bonus
		castleOwnerCheckReturnAddress = 0x4D8EF2;		    
		_PI->WriteLoHook(0x4D8EC7, castleOwnerCheck);

		// Prevents AI from casting Fly if they don't have it.
		Ai_WaterwalkFlyReturnAddress_Cast = 0x42EE8B;
		Ai_WaterwalkFlyReturnAddress_Skip = 0x42F1AB;
		_PI->WriteLoHook(0x42EE68, Ai_WaterwalkFly);	

		// Tavern rumors bug fix
		_PI->WriteDword(0x4C1410, 0x636174);
		    
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
		    
            }

            // ------------------------------
            // ENG
            // ------------------------------

            // Heroes III Erathia - v1.4
            check1 = *(int*)(0x4F5583+1);
            check2 = *(int*)(0x5D8F69+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4CA6C0, 0x668D38);

		// AI moat damage fix
		_PI->WriteByte(0x4B2A79, 0xEB);
		    
		// RefugeCamp fix for random maps (it gave only 1 Pikeman each week)
		refugeCampReturnAddress_Skip = 0x502B5F;
		refugeCampReturnAddress_Write = 0x502931;
		_PI->WriteLoHook(0x502925, fixRefugeCamp);
		    
		// fix ghost hero (without army)
		//_PI->WriteHiHook(0x449D30, SPLICE_, EXTENDED_, THISCALL_, ghostHeroFix);
		_PI->WriteLoHook(0x58C5D9, ghostHeroFix);
		    
		// fix Harpy fly after Dendroid bind
		fixHarpyBindsReturnAddress = 0x4780C5;
		_PI->WriteLoHook(0x4780BB, fixHarpyBinds);
		    
		// fix Witch Huts for random maps (it gave only secondary skills with number 15 or lesser)
		// не нужен		    
		    		    
            }

            // Heroes III Armageddon - v2.2
            check1 = *(int*)(0x4F5963+1);
            check2 = *(int*)(0x5FFBF9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4CB020, 0x693C78);

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
		    		    
            }

            // Heroes III Complete - v4.0
            check1 = *(int*)(0x4F7B03+1);
            check2 = *(int*)(0x601B89+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4CCF80, 0x696D98);
		    
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
		    		    
            }

            // Heroes Chronicles Warlords & Underworld - v1.0
            check1 = *(int*)(0x4F0033+1);
            check2 = *(int*)(0x5B5129+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4C5160, 0x638F40);
		    
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
		    		    
            }

            // Heroes Chronicles Elements & Dragons - v1.0
            check1 = *(int*)(0x4EFE04+1);
            check2 = *(int*)(0x5B5469+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4C4B90, 0x638F40);
		    
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
		    		    
            }

            // Heroes Chronicles WorldTree - v1.0
            check1 = *(int*)(0x4EFA84+1);
            check2 = *(int*)(0x5B51B9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4C4AF0, 0x638F40);
		    
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
		    		    
            }

            // Heroes Chronicles FieryMoon - v1.0
            check1 = *(int*)(0x4EF824+1);
            check2 = *(int*)(0x5B5249+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4C4810, 0x63B338);
		    
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
		    		   
            }

            // Heroes Chronicles Beastmaster & Sword - v1.0
            check1 = *(int*)(0x4EF874+1);
            check2 = *(int*)(0x5B4C09+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4C4520, 0x63C3A8);
		    
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
		    		    
            }

            // ------------------------------
            // USA
            // ------------------------------

            // Heroes Chronicles Beastmaster & Sword - v1.0
            check1 = *(int*)(0x4EF914+1);
            check2 = *(int*)(0x5B51B9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4C4B40, 0x63D3A0);
		    
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
		    		    
            }

            // ------------------------------
            // GER
            // ------------------------------

            // Heroes Chronicles Dragons - GOG - v1.0
            check1 = *(int*)(0x4EFA04+1);
            check2 = *(int*)(0x5B51C9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4C43C0, 0x63B380);
		    
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
		    		    
            }

            // ------------------------------
            // FRA
            // ------------------------------

            // Heroes III Armageddon - v2.1
            check1 = *(int*)(0x4F61C3+1);
            check2 = *(int*)(0x6003D9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4CB170, 0x693D98);

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
		    		    
            }

            // Heroes III Shadow - v3.1
            check1 = *(int*)(0x4F8163+1);
            check2 = *(int*)(0x6028F9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4CD040, 0x697EC8);
		    
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
		    		    
            }

            // ------------------------------
            // POL
            // ------------------------------

            // Heroes III Shadow - v3.2
            check1 = *(int*)(0x4F78D3+1);
            check2 = *(int*)(0x602179+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4CC630, 0x699E20);
		    
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
		    		    
            }

            // Heroes III Shadow - v3.2 / Armageddon - v2.2
            check1 = *(int*)(0x4F5993+1);
            check2 = *(int*)(0x5FE337+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
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
		_PI->WriteDword(0x4CAB10, 0x695CB8);
		    
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
		    		    
            }

        }
    }

   return TRUE;
}

