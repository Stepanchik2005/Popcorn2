#pragma once
#include "Active_Brick.h"
//------------------------------------------------------------------------------------------------------------
class ALevel_Data
{
public:
	ALevel_Data(int level_index);

	int Get_Avaliable_Bricks_Count();

	char *Level;
	AAdvertisement *Advertisement;

	static const int Max_Level_Count = 10;
private:
	static char Test_Level[AsConfig::Level_Height][AsConfig::Level_Width];

	static char Level_01[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_02[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_03[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_04[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_05[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_06[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_07[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_08[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_09[AsConfig::Level_Height][AsConfig::Level_Width];
	static char Level_10[AsConfig::Level_Height][AsConfig::Level_Width];

	static char *Levels_Array[Max_Level_Count];
};
