﻿#pragma once
#include "Border.h"
#include "Level.h"
#include "Platform.h"
#include "Ball_Set.h"
#include "Monster_Set.h"
#include "Info_Panel.h"
//------------------------------------------------------------------------------------------------------------
enum class EKey_Type: unsigned char
{
	Left,
	Right,
	Space
};
//------------------------------------------------------------------------------------------------------------
enum class EGame_State: unsigned char
{
	Test_Ball,

	Enter_User_Name,
	Cleaning_Level,
	Mopped_Level,
	Play_Level,
	Lost_Ball,
	Restart_Level,
	Finish_Level,
	Game_Over,
	Game_Won
};
//------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------
class AsEngine
{
public:
	AsEngine();
	
	void Init_Engine(HWND hwnd);
	void Draw_Frame(HDC hdc, RECT &paint_area);
	int On_Key(EKey_Type key_type, bool key_down);
	int On_Timer();
	void On_Char(wchar_t symbol);

	const int Timer_ID;

private:
	void Advance_Mover();
	void Play_Level();
	void Restart_Level();
	void Act();
	void Handle_Message();
	void On_Falling_Letter(AFalling_Letter *falling_letter);
	void Game_Over();
	void Game_Won();
	void Stop_Play();

	bool Got_Name;
	double Rest_Distance;
	

	EGame_State Game_State;
	AsBall_Set Ball_Set;
	AsLaser_Beam_Set Laser_Beam_Set;
	ALevel Level;
	AsPlatform Platform;
	AsBorder Border;
	AsMonster_Set Monster_Set;
	AsInfo_Panel Info_Panel;

	std::vector<AGame_Object *> Modules;


};
//------------------------------------------------------------------------------------------------------------
