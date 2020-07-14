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
 BOOLEAN isDead[8]; // +128550  (+0x1F626)
 WORD curr_day; // +128558 (+0x1F63E)
 WORD curr_week; // +128560 (+0x1F640)
 WORD curr_month; // +128562 (+0x1F642)
 INT8 unk3[32];
 INT8 artifactMerchant[28]; // +128596 (+0x1F664)
 INT8 pBlackMarketsArray[16]; // +128624 (+0x1F680)
 INT8 unk4[8];
 INT8 playersInfo[460]; // +128648 (+0x1F688)
 INT8 mapInfo[720]; // +129108 (+0x1F86C)
 H3String mapName; // +129828 (+0x1FB3C)
 H3String mapDescription; // +129844 +0x1FB4C
 INT8 unk5[20];
 INT8 mainSetup[3932]; // +129880 (+0x1FB70)
 INT8 unk6[4];
 H3Player players[8]; // +133816 (+0x20AD0)
 INT8 pTownsArray[16]; // +136696 (+0x21610)
 H3Hero heroes[156]; // +136712 (+0x21620)
 INT8 heroOwner[156]; // +319232 (+0x4DF18)
 BitMaskDword heroMayBeHiredBy[156]; // +319388 (+0x4DFB4)
 INT8 randomArtifacts[144]; // +320012 (+0x4E224)
 INT8 artifactsAllowed[144]; // +320156 (+0x4E2B4)
 INT8 unk7[32];
 UINT8 keymasterVisited[8]; // +320332 (+0x4E364) (bitfield for players)
 INT8 unk8[12];
 INT8 pSignpostsBottlesArray[16]; // +320352 (+0x4E378)
 INT8 pMinesLighthousesArray[16]; // +320368 (+0x4E388)
 INT8 pDwellingsArray[16]; // +320384 (+0x4E398)
 INT8 pGarrisonsArray[16]; // +320400 (+0x4E3A8)
 INT8 pBoatsArray[16]; // +320416 (+0x4E3B8)
 INT8 pUniversitiesArray[16]; // +320432 (+0x4E3C8)
 INT8 pCreatureBanksArray[16]; // +320448 (+0x4E3D8)
 INT8 obeliskCount; // +320464 (+0x4E3E8)
 UINT8 obeliskVisited[48]; // +320465 +0x4E3E9
 INT8 unk9[575];
 INT8 bannedSkills[28]; // +321088 (+0x4E658)
 INT8 unk10[4];
 INT8 pMonolithTwoWayArray[8 * 16]; // +321120 (+0x4E678) (H3Position)
 INT8 pMonolithOneWayArray[8 * 16]; // +321248 (+0x4E6F8) (H3Position)
 INT8 unk11[4];
 INT8 pWhirlPoolsArray[16]; // +321380 (+0x4E77C) (H3Position)
 INT8 pSubterraneanGatesDestinationArray[16]; // +321396 (+0x4E78C) (H3Position)
 INT8 pSubterraneanGatesIDArray[16]; // +321412 (+0x4E79C) (H3Position)
 INT8 unk_vec1[16];
 INT8 unk_vec2[16];
 INT8 unk12[4];
};
