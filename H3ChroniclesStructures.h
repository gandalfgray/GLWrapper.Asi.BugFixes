struct H3CHRMain
{
 h3unk _f_0000[4];
 INT8 disabledShrines[70]; // * +4
 INT8 disabledSpells[70]; // * +4A
 UINT32 cs_bink; // * +90
 INT8 pTownPreSetupArray[16]; // * +94
 INT8 heroSetup[156 * 820]; // * +A4
 INT8 unk1[4];
 INT8 campaignInfo[114]; // * +1F458
 INT8 unk2[354];
 BOOLEAN isDead[8]; // !!!!!!!!!!!  * +1F626
 WORD curr_day; // * +1F63E
 WORD curr_week; // * +1F640
 WORD curr_month; // * +1F642
 INT8 unk3[32];
 INT8 artifactMerchant[28]; // * +1F664
 INT8 pBlackMarketsArray[16]; // * +1F680
 INT8 unk4[10];
 INT8 playersInfo[460]; // * +1F688
 INT8 mapInfo[720]; // * +1F86C 
 H3String mapName; // * +1FB3C
 H3String mapDescription; / * +1FB4C
 INT8 unk5[20];
 INT8 mainSetup[3932]; // * +1FB70
 INT8 unk6[4];
 H3Player players[8]; // * +20AD0
 INT8 pTownsArray[16]; // * +21610
 H3Hero heroes[156]; // * +21620
 INT8 heroOwner[156]; // * +4DF18
 BitMaskDword heroMayBeHiredBy[156]; // * +4DFB4
 INT8 randomArtifacts[144]; // * +4E224
 INT8 artifactsAllowed[144]; // * +4E2B4
 INT8 unk7[32];
 UINT8 keymasterVisited[8]; // * +4E364 (bitfield for players)
 INT8 unk8[12];
 INT8 pSignpostsBottlesArray[16]; // * +4E378
 INT8 pMinesLighthousesArray[16]; // * +4E388
 INT8 pDwellingsArray[16]; // * +0x4E398
 INT8 pGarrisonsArray[16]; // * +0x4E3A8
 INT8 pBoatsArray[16]; // * +0x4E3B8
 INT8 pUniversitiesArray[16]; // * +0x4E3C8
 INT8 pCreatureBanksArray[16]; // * +0x4E3D8
 INT8 obeliskCount; // * +4E3E8
 UINT8 obeliskVisited[48]; // * +4E3E9
 INT8 unk9[575];
 INT8 bannedSkills[28]; // * +4E658
 INT8 unk10[4];
 INT8 pMonolithTwoWayArray[8 * 16]; // * +4E678 (H3Position)
 INT8 pMonolithOneWayArray[8 * 16]; // * +4E6F8 (H3Position)
 INT8 unk11[4];
 INT8 pWhirlPoolsArray[16]; // * +4E77C (H3Position)
 INT8 pSubterraneanGatesDestinationArray[16]; // * +4E78C (H3Position)
 INT8 pSubterraneanGatesIDArray[16]; // * +4E79C (H3Position)
 INT8 unk_vec1[16];
 INT8 unk_vec2[16];
 INT8 unk12[4];
};
