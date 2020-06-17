#include "..\..\include\patcher_x86.hpp"
#include "H3Structures.h"
#include "H3Numerations.h"

Patcher* _P;
PatcherInstance* _PI;

static _bool_ plugin_On = 0;

// Prevents AI from casting Fly if they don't have it.
DWORD Ai_WaterwalkFlyReturnAddress_Cast;
DWORD Ai_WaterwalkFlyReturnAddress_Skip;
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
DWORD castleOwnerCheckReturnAddress;
int __stdcall castleOwnerCheck(LoHook *h, HookContext *c)
{
   H3Town *town = (H3Town*)(c->ecx);
   H3Hero *hero = *(H3Hero**)(c->ebp - 4); // _Hero_ is stored in temp variable [LOCAL.1]

   if (hero->owner_id == town->owner_id) // normal
      return EXEC_DEFAULT;
  
   c->return_address = castleOwnerCheckReturnAddress; // skip procedure
   return NO_EXEC_DEFAULT;
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

            DWORD check1, check2;

            // for HoMM SoD 3.2 (eng)
            check1 = *(DWORD*)(0x004F8193+1);
            check2 = *(DWORD*)(0x00602149+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E3A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E4DF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465943+1, 0x4D); // fdiv -> fmul

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
    
            }

            // ------------------------------
            // RUS
            // ------------------------------

            // Heroes III Armageddon - v2.1 Buka
            check1 = *(DWORD*)(0x004F2533+1);
            check2 = *(DWORD*)(0x005F9649+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041D5B3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041D6F1+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x004640FD+1, 0x4D); // fdiv -> fmul
		    
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
		    
            }

            // Heroes III Armageddon - v2.2 Buka
            check1 = *(DWORD*)(0x004F2863+1);
            check2 = *(DWORD*)(0x005F9609+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041D553+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041D691+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00463D9D+1, 0x4D); // fdiv -> fmul
		    
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
		    
            }

            // Heroes III Shadow - v3.1 Buka
            check1 = *(DWORD*)(0x004F7EB3+1);
            check2 = *(DWORD*)(0x00602379+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E2D3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E40F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465EF3+1, 0x4D); // fdiv -> fmul
		    
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
		    
            }

            // Heroes III Complete - v4.0 Buka
            check1 = *(DWORD*)(0x004F7EB3+1);
            check2 = *(DWORD*)(0x006021A9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E456+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E595+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0046592C+1, 0x4D); // fdiv -> fmul

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

            }

            // Heroes Chronicles Warlords & Underworld & Elements & Dragons - v1.0
            check1 = *(DWORD*)(0x004EBA34+1);
            check2 = *(DWORD*)(0x005AF329+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041D5F3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041D731+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00461206+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x43E0F6, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FFB5, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus
		castleOwnerCheckReturnAddress = 0x4D9132;		    
		_PI->WriteLoHook(0x4D9107, castleOwnerCheck);
		    
		// Prevents AI from casting Fly if they don't have it.
		Ai_WaterwalkFlyReturnAddress_Cast = 0x42ED9B;
		Ai_WaterwalkFlyReturnAddress_Skip = 0x42F0BB;
		_PI->WriteLoHook(0x42E978, Ai_WaterwalkFly);		    

            }

            // Heroes Chronicles Beastmaster & Sword - v1.0
            check1 = *(DWORD*)(0x004EB494+1);
            check2 = *(DWORD*)(0x005AF2D9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041D6A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041D7E1+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00461126+1, 0x4D); // fdiv -> fmul

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

            }

            // ------------------------------
            // ENG
            // ------------------------------

            // Heroes III Erathia - v1.4
            check1 = *(DWORD*)(0x004F5583+1);
            check2 = *(DWORD*)(0x005D8F69+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E343+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E47F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x004653D3+1, 0x4D); // fdiv -> fmul

		// Neutral creatures luck - не нужен
		    
		// The Castle's Lighthouse building bonus
		castleOwnerCheckReturnAddress = 0x4E261C;		    
		_PI->WriteLoHook(0x4E25F0, castleOwnerCheck);
		    
		// Prevents AI from casting Fly if they don't have it.
		Ai_WaterwalkFlyReturnAddress_Cast = 0x4301A1;
		Ai_WaterwalkFlyReturnAddress_Skip = 0x4304B0;
		_PI->WriteLoHook(0x43017E, Ai_WaterwalkFly);		    
		    
            }

            // Heroes III Armageddon - v2.2
            check1 = *(DWORD*)(0x004F5963+1);
            check2 = *(DWORD*)(0x005FFBF9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E033+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E16F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465A53+1, 0x4D); // fdiv -> fmul
		    
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
		    
            }

            // Heroes III Complete - v4.0
            check1 = *(DWORD*)(0x004F7B03+1);
            check2 = *(DWORD*)(0x00601B89+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E223+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E35F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465D63+1, 0x4D); // fdiv -> fmul
		    
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
		    
            }

            // Heroes Chronicles Warlords & Underworld - v1.0
            check1 = *(DWORD*)(0x004F0033+1);
            check2 = *(DWORD*)(0x005B5129+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E1A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E2DF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0046364C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441D34, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FE52, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus - другой код 0x4C0BF6
		    
		// Prevents AI from casting Fly if they don't have it.
		Ai_WaterwalkFlyReturnAddress_Cast = 0x42FF31;
		Ai_WaterwalkFlyReturnAddress_Skip = 0x430240;
		_PI->WriteLoHook(0x42FF0E, Ai_WaterwalkFly);		    
		    
            }

            // Heroes Chronicles Elements & Dragons - v1.0
            check1 = *(DWORD*)(0x004EFE04+1);
            check2 = *(DWORD*)(0x005B5469+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E1C3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E2FF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x004632CC+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441744, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F862, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus - другой код 0x4C0626
		    
            }

            // Heroes Chronicles WorldTree - v1.0
            check1 = *(DWORD*)(0x004EFA84+1);
            check2 = *(DWORD*)(0x005B51B9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E163+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E29F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0046326C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x4418F4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FA12, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus - не нужно
		    
            }

            // Heroes Chronicles FieryMoon - v1.0
            check1 = *(DWORD*)(0x004EF824+1);
            check2 = *(DWORD*)(0x005B5249+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E183+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E2BF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00462CDC+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441424, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F542, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus - не нужно
		    
            }

            // Heroes Chronicles Beastmaster & Sword - v1.0
            check1 = *(DWORD*)(0x004EF874+1);
            check2 = *(DWORD*)(0x005B4C09+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E213+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E34F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00462E5C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441514, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F632, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus - другой код 0x4BFF96
		    
            }

            // ------------------------------
            // USA
            // ------------------------------

            // Heroes Chronicles Beastmaster & Sword - v1.0
            check1 = *(DWORD*)(0x004EF914+1);
            check2 = *(DWORD*)(0x005B51B9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E433+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E56F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0046345C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x4419E4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FB02, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus - другой код 0x4C05A6
		    
            }

            // ------------------------------
            // GER
            // ------------------------------

            // Heroes Chronicles Dragons - GOG - v1.0
            check1 = *(DWORD*)(0x004EFA04+1);
            check2 = *(DWORD*)(0x005B51C9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E0F3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E22F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00462C0C+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441354, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F472, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus - другой код 0x4BFE26
		    
            }

            // ------------------------------
            // FRA
            // ------------------------------

            // Heroes III Armageddon - v2.1
            check1 = *(DWORD*)(0x004F61C3+1);
            check2 = *(DWORD*)(0x006003D9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E143+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E27F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465CB3+1, 0x4D); // fdiv -> fmul
		    
		// Neutral creatures luck
                _PI->WriteHexPatch(0x441914, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FA32, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus
		castleOwnerCheckReturnAddress = 0x4E307C;		    
		_PI->WriteLoHook(0x4E3050, castleOwnerCheck);
		    
            }

            // Heroes III Shadow - v3.1
            check1 = *(DWORD*)(0x004F8163+1);
            check2 = *(DWORD*)(0x006028F9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E413+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E54F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x004661B3+1, 0x4D); // fdiv -> fmul
		    
		// Neutral creatures luck
                _PI->WriteHexPatch(0x441CF4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FE12, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus
		castleOwnerCheckReturnAddress = 0x4E4A4C;		    
		_PI->WriteLoHook(0x4E4A20, castleOwnerCheck);
		    
            }

            // ------------------------------
            // POL
            // ------------------------------

            // Heroes III Shadow - v3.2
            check1 = *(DWORD*)(0x004F78D3+1);
            check2 = *(DWORD*)(0x00602179+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E353+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E48F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465973+1, 0x4D); // fdiv -> fmul
		   
		// Neutral creatures luck
                _PI->WriteHexPatch(0x4414E4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F602, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus
		castleOwnerCheckReturnAddress = 0x4E44AC;		    
		_PI->WriteLoHook(0x4E4480, castleOwnerCheck);
		    
            }

            // Heroes III Shadow - v3.2 / Armageddon - v2.2
            check1 = *(DWORD*)(0x004F5993+1);
            check2 = *(DWORD*)(0x005FE337+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E1A3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E2DF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465903+1, 0x4D); // fdiv -> fmul
		    
		// Neutral creatures luck
                _PI->WriteHexPatch(0x441764, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F883, "90 90 90 90 90 90");
		    
		// The Castle's Lighthouse building bonus
		castleOwnerCheckReturnAddress = 0x4E2B4C;		    
		_PI->WriteLoHook(0x4E2B20, castleOwnerCheck);
		    
            }

        }
    }

   return TRUE;
}

