#pragma once
#include "Border.h"
#include "Level.h"
#include "Platform.h"
#include "Ball_Set.h"
#include "Monster_Set.h"
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

	Play_Level,
	Lost_Ball,
	Restart_Level
};
//------------------------------------------------------------------------------------------------------------
class AsInfo_Panel: public AGame_Object
{
public:
	~AsInfo_Panel();
	AsInfo_Panel();

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Init();
	
private:
	void Choose_Font();
	void Draw_String(HDC hdc, RECT &rect, const wchar_t *str, bool is_player_name);
	void Draw_Extra_Life(HDC hdc, int x_offset, int y_offset);

	HFONT Logo_Pop_Font, Logo_Corn_Font, Name_Font, Score_Font;

	AColor *Shadow_Color, *Highlight_Color, *Dark_Blue, *Dark_Red, *Platform_Inner_Color;

	AFalling_Letter Letter_P, Letter_G, Letter_M;

	static const int Score_X = 208;
	static const int Score_Y = 108;
	static const int Score_Width = 110;
	static const int Score_Height = 90;
	static const int Score_Offset = 22;
};

//------------------------------------------------------------------------------------------------------------
class AsEngine
{
public:
	AsEngine();
	
	void Init_Engine(HWND hwnd);
	void Draw_Frame(HDC hdc, RECT &paint_area);
	int On_Key(EKey_Type key_type, bool key_down);
	int On_Timer();

	const int Timer_ID;

private:
	void Advance_Mover();
	void Play_Level();
	void Restart_Level();
	void Act();
	void On_Falling_Letter(AFalling_Letter *falling_letter);
	void Add_New_Module(int &index, AGame_Object *obj);

	double Rest_Distance;
	int Life_Count;

	EGame_State Game_State;
	AsBall_Set Ball_Set;
	AsLaser_Beam_Set Laser_Beam_Set;
	ALevel Level;
	AsPlatform Platform;
	AsBorder Border;
	AsMonster_Set Monster_Set;
	AsInfo_Panel Info_Panel;

	AGame_Object *Modules[AsConfig::Max_Modules_Count];
};
//------------------------------------------------------------------------------------------------------------
