#include "..\..\include\patcher_x86.hpp"

Patcher* _P;
PatcherInstance* _PI;

static _bool_ plugin_On = 0;

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
    
            }

            // ------------------------------
            // RUS
            // ------------------------------

            // Heroes III Erathia - v1.0 Buka
            check1 = *(DWORD*)(0x004D4A5F+1);
            check2 = *(DWORD*)(0x0059308B+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix - отсутствует

                // Neutral creatures luck - не нужен

            }

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

            }

            // ------------------------------
            // ENG
            // ------------------------------

            // Heroes III Erathia - v1.0
            check1 = *(DWORD*)(0x004D3363+1);
            check2 = *(DWORD*)(0x0058E558+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix - отсутствует

                // Neutral creatures luck - не нужен

            }

            // Heroes III Erathia - v1.1
            check1 = *(DWORD*)(0x0041E573+1);
            check2 = *(DWORD*)(0x004210B8+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix - отсутствует

                // Neutral creatures luck - не нужен

            }

            // Heroes III Erathia - v1.2
            check1 = *(DWORD*)(0x0041E523+1);
            check2 = *(DWORD*)(0x00421078+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix - отсутствует

                // Neutral creatures luck - не нужен

            }

            // Heroes III Erathia - v1.3
            check1 = *(DWORD*)(0x004F58F3+1);
            check2 = *(DWORD*)(0x005D9679+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E003+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E13F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465203+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x441574, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43DE46, "90 90 90 90 90 90");  
            }

            // Heroes III Erathia - v1.4
            check1 = *(DWORD*)(0x004F5583+1);
            check2 = *(DWORD*)(0x005D8F69+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E343+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E47F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x004653D3+1, 0x4D); // fdiv -> fmul

                // Neutral creatures luck
                _PI->WriteHexPatch(0x4416B4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F7D2, "90 90 90 90 90 90");  
            }

            // Heroes III Armageddon - v2.0
            check1 = *(DWORD*)(0x004EB283+1);
            check2 = *(DWORD*)(0x005EEFD8+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041DDE3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041DF1F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00460C59+1, 0x4D); // fdiv -> fmul
		    
                // Neutral creatures luck
                _PI->WriteHexPatch(0x440850, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43E992, "90 90 90 90 90 90");  		    
            }

            // Heroes III Armageddon - v2.1
            check1 = *(DWORD*)(0x004F5C43+1);
            check2 = *(DWORD*)(0x00600299+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E343+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E47F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465A23+1, 0x4D); // fdiv -> fmul
		    
                // Neutral creatures luck
                _PI->WriteHexPatch(0x4416D4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F7F2, "90 90 90 90 90 90");  		    
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
            }

            // Heroes III Shadow - v3.0
            check1 = *(DWORD*)(0x004F7D73+1);
            check2 = *(DWORD*)(0x006027E9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E4B3+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E5EF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465D63+1, 0x4D); // fdiv -> fmul
		    
                // Neutral creatures luck
                _PI->WriteHexPatch(0x441A64, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FB82, "90 90 90 90 90 90");  		    
            }

            // Heroes III Shadow - v3.1
            check1 = *(DWORD*)(0x004F85B3+1);
            check2 = *(DWORD*)(0x006027E9+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E203+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E33F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465B23+1, 0x4D); // fdiv -> fmul
		    
                // Neutral creatures luck
                _PI->WriteHexPatch(0x4414A4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F5C2, "90 90 90 90 90 90");  		    
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
            }

            // ------------------------------
            // GER
            // ------------------------------

            // Heroes III Erathia - v1.2
            check1 = *(DWORD*)(0x004D5253+1);
            check2 = *(DWORD*)(0x00591B29+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix - отсутствует

                // Neutral creatures luck - не нужен
            }

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
                _PI->WriteHexPatch(0x4419E4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F472, "90 90 90 90 90 90");
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
		    
                _PI->WriteHexPatch(0x441914, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FA32, "90 90 90 90 90 90");		    
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
		    
                _PI->WriteHexPatch(0x441CF4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43FE12, "90 90 90 90 90 90");		    
            }

            // ------------------------------
            // POL
            // ------------------------------

            // Heroes III Armageddon - v2.1
            check1 = *(DWORD*)(0x004F5723+1);
            check2 = *(DWORD*)(0x005FED57+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E383+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E4BF+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465813+1, 0x4D); // fdiv -> fmul
		    
                _PI->WriteHexPatch(0x4414A4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F5C2, "90 90 90 90 90 90");		    
            }

            // Heroes III Shadow - v3.1
            check1 = *(DWORD*)(0x004F7AF3+1);
            check2 = *(DWORD*)(0x00600ED7+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E233+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E36F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465AE3+1, 0x4D); // fdiv -> fmul
		    
                _PI->WriteHexPatch(0x441574, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F692, "90 90 90 90 90 90");		    
            }

            // Heroes III Shadow - v3.2
            check1 = *(DWORD*)(0x004F78D3+1);
            check2 = *(DWORD*)(0x00602179+1);
            if(check1 == (WS_VISIBLE | WS_POPUP) && check2 == (WS_VISIBLE | WS_POPUP))
            {
                // Armorer fix
                _PI->WriteByte(0x0041E353+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x0041E48F+1, 0x4D); // fdiv -> fmul
                _PI->WriteByte(0x00465973+1, 0x4D); // fdiv -> fmul
		    
                _PI->WriteHexPatch(0x4414E4, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F602, "90 90 90 90 90 90");		    
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
		    
                _PI->WriteHexPatch(0x441764, "90 90 90 90 90 90");
                _PI->WriteHexPatch(0x43F883, "90 90 90 90 90 90");		    
            }

        }
    }

   return TRUE;
}

