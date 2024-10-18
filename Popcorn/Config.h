#pragma once

#include "Tools.h"
class AFont
{
public:
	AFont();
	AFont(int height, int weight, int family, const wchar_t *name, bool is_italic = false);

	void Select(HDC hdc) const;

private:
	HFONT Content;
};

//------------------------------------------------------------------------------------------------------------
class AColor
{
public:
	~AColor();
	AColor();
	AColor(unsigned char r, unsigned char g, unsigned char b);
	AColor(const AColor &color,int pen_size);
	AColor(const AColor &pen_color, int pen_size, const AColor &brush_color);
	AColor(unsigned char r, unsigned char g, unsigned char b,int pen_size);

	void operator=(const AColor &other_color);
	void Set_As(unsigned char r, unsigned char g, unsigned char b); // вместо оператора копирования
	int Get_RGB() const;
	void Select(HDC hdc) const;
	HBRUSH Get_Brush() const;
	HPEN Get_Pen() const;
	void Set_Brush(HBRUSH brush);
	void Set_Pen(HPEN pen);
	void Select_Pen(HDC hdc) const;

	

	unsigned char R, G, B;
private:
	HPEN Pen;
	HBRUSH Brush;
	
};




//------------------------------------------------------------------------------------------------------------
class AsConfig
{
public:
	static void Throw();

	static bool Level_Has_Floor;
	static int Current_Timer_Tick;
	static HWND Hwnd;

	static const AColor BG_Color, Red_Color, Blue_Color, White_Color, Monster_Dark_Red_Color,  Letter_Color, Laser_Color,Paraschute_Color;
	static const AColor Teleport_Color, Advertisement_Blue_Table_Color, Unbreakable_Blue_Hightlight_Color, Unbreakable_Red_Hightlight_Color;;
	static const AColor Cornea_Color, Iris_Color, Cornea_Arc_Color, Explodive_Red_Color, Explodive_Blue_Color, Comet_Color;
	static const AColor Shadow_Color, Highlight_Color;

	static const AFont Name_Font, Score_Font, Logo_Pop_Font, Logo_Corn_Font, Final_Title_Font;

	static const double Moving_Size_Step;
	static const double D_Global_Scale;
	static const double Accelerate_Ball_Speed;
	static const double Initial_Ball_Speed;
	static const double Min_Ball_Angle;


	static const int Global_Scale = 3;
	static const int Border_X_Offset = 6;
	static const int Border_Y_Offset = 4;
	static const int Level_Width = 12;  // Ширина уровня в ячейках
	static const int Level_Height = 14;  // Высота уровня в ячейках
	static const int Level_X_Offset = 8;
	static const int Level_Y_Offset = 6;
	static const int Cell_Width = 16;
	static const int Cell_Height = 8;
	static const int Brick_Width = 15;
	static const int Brick_Height = 7;
	static const int Max_X_Pos = Level_X_Offset + Cell_Width * Level_Width;
	static const int Max_Y_Pos = 199;
	static const int Platform_Y_Pos = 185;
	static const int FPS = 20;
	static const int Max_Active_Brick_Size = 10;
	static const int Max_Falling_Letters_Size = 10;
	static const int Max_Random_Number_For_Letter = 1; // вероятность	1 / Max_Random_Number_For_Letter выпадения буквы
	static const int Max_Balls_Count = 15;
	static const int Max_Life_Count = 12;
	static const int Floor_Y_Pos = AsConfig::Max_Y_Pos - 1;
	static const int Initiall_Extra_Lives = 5; // !!! ДОЛЖНО БЫТЬ 5
	
	static const double Ball_Radius;

	// константы платформы
	static const int Platform_Height = 7;
	static const int Platform_Normal_Width = 28;
	static const int Platform_Circle_Size = 7; // диаметр
	static const int Platform_Normal_Inner_Width = Platform_Normal_Width - Platform_Circle_Size;

	static const int Gate_Width = 6;
	static const int Gate_Height = 19;
	static const int Gates_Count = 8;
};




//------------------------------------------------------------------------------------------------------------
class AColor_Fade
{
public:
	~AColor_Fade();
	AColor_Fade();
	AColor_Fade(const AColor &origin_color, int max_fade_step);
	AColor_Fade(const AColor &origin_color, int max_fade_step, const AColor& dest_color);
	AColor* Get_Color(int index);

   void Init(const AColor &origin_color, const AColor& dest_color, int max_fade_step);
private:
	int Max_Fade_Step;

	std::vector<AColor *> Fading_Colors;
};
//------------------------------------------------------------------------------------------------------------


class AsCommon
{
public:
	static int  Rand(int range);
	static void Round_Rect(HDC hdc, RECT &brick_rect, int corner_radius = 2);
	static void Rect(HDC hdc, int x, int y, int width, int height, const AColor &color);
	static void Rect(HDC hdc, RECT &rect, const AColor &color);
	static void Ellipse(HDC hdc, RECT &rect, const AColor &color);

	static void Invalidate_Rect(RECT &rect);

	static unsigned char Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step, int max_fade_step);
	static AColor* Get_Fading_Color(const AColor &origin_color, int step, int max_fade_step);
	static AColor* Get_Fading_Color(const AColor &origin_color,const AColor &dest_color,  int step, int max_fade_step);
	

	static bool Reflect_On_Circle(double next_x_pos, double next_y_pos, double x_pos, double y_pos, double width, ABall_Object *ball);
};


enum class EMessage_Type 
{
	Floor_Is_Over,
	Unfreeze_Monsters,
	Level_Done
};

class AMessage
{
public:
	AMessage(EMessage_Type message_type);

	const EMessage_Type Message_Type;
private:
	

};



class AsMessage_Manager
{
public:
	static void Add_Message(AMessage *message);
	static bool Get_Message(AMessage **message);
private:
	static std::queue<AMessage *> Messages;
};





class AString
{
public:
	AString();
	AString(const wchar_t *str);

	const wchar_t *Get_Content();
	void Append(int val, int digits);
	void Append(wchar_t symbol);
	void Delete_Symbol();
	int Get_Length();
private:
	std::wstring Content;

};

