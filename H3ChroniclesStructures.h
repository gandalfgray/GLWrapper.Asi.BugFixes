struct H3CHRMain
{
 INT8 unk0[4];
 INT8 disabledShrines[70]; // +4 (+0x4)
 INT8 disabledSpells[70]; // +74 (+0x4A)
 UINT32 cs_bink; // +144 (+0x90)
 INT8 pTownPreSetupArray[16]; // +148 (+0x94)
 INT8 heroSetup[156 * 820]; // +164 (+0xA4)
 INT8 unk1[4];
 INT8 campaignInfo[108]; // +128088 (+0x1F458)
 INT8 unk2[354];
 BOOLEAN isDead[8]; // +128550  (+0x1F616)
 WORD curr_day; // +128558 (+0x1F62E)
 WORD curr_week; // +128560 (+0x1F630)
 WORD curr_month; // +128562 (+0x1F632)
 INT8 unk3[32];
 INT8 artifactMerchant[28]; // +128596 (+0x1F654)
 INT8 pBlackMarketsArray[16]; // +128624 (+0x1F670)
 INT8 unk4[8];
 INT8 playersInfo[460]; // +128648 (+0x1F688)
 INT8 mapInfo[720]; // +129108 (+0x1F854)
 H3String mapName; // +129828 (+0x1FB24)
 H3String mapDescription; // +129844 +0x1FB34
 INT8 unk5[20];
 INT8 mainSetup[3932]; // +129880 (+0x1FB58)
 INT8 unk6[4];
 H3Player players[8]; // +133816 (+0x20AB8)
 INT8 pTownsArray[16]; // +136696 (+0x215F8)
 H3Hero heroes[156]; // +136712 (+0x21608)
 INT8 heroOwner[156]; // +319232 (+0x4DF00)
 BitMaskDword heroMayBeHiredBy[156]; // +319388 (+0x4DF9C)
 INT8 randomArtifacts[144]; // +320012 (+0x4E20C)
 INT8 artifactsAllowed[144]; // +320156 (+0x4E29C)
 INT8 unk7[32];
 UINT8 keymasterVisited[8]; // +320332 (+0x4E34C) (bitfield for players)
 INT8 unk8[12];
 INT8 pSignpostsBottlesArray[16]; // +320352 (+0x4E360)
 INT8 pMinesLighthousesArray[16]; // +320368 (+0x4E370)
 INT8 pDwellingsArray[16]; // +320384 (+0x4E380)
 INT8 pGarrisonsArray[16]; // +320400 (+0x4E390)
 INT8 pBoatsArray[16]; // +320416 (+0x4E3A0)
 INT8 pUniversitiesArray[16]; // +320432 (+0x4E3B0)
 INT8 pCreatureBanksArray[16]; // +320448 (+0x4E3C0)
 INT8 obeliskCount; // +320464 (+0x4E3D0)
 UINT8 obeliskVisited[48]; // +320465 (+0x4E3D1)
 INT8 unk9[575];
 INT8 bannedSkills[28]; // +321088 (+0x4E640)
 INT8 unk10[4];
 INT8 pMonolithTwoWayArray[8 * 16]; // +321120 (+0x4E660) (H3Position)
 INT8 pMonolithOneWayArray[8 * 16]; // +321248 (+0x4E6E0) (H3Position)
 INT8 unk11[4];
 INT8 pWhirlPoolsArray[16]; // +321380 (+0x4E764) (H3Position)
 INT8 pSubterraneanGatesDestinationArray[16]; // +321396 (+0x4E774) (H3Position)
 INT8 pSubterraneanGatesIDArray[16]; // +321412 (+0x4E784) (H3Position)
 INT8 unk_vec1[16];
 INT8 unk_vec2[16];
 INT8 unk12[4];
};
