#pragma once
#include "Config.h"


enum class EGate_State: unsigned char
{
	Closed, 
	Open_Short,
	Open_Long
};
enum class EGate_Transformation : unsigned char
{
	Unknown,

	Init,
	Emitting,
	Active,
	Finalize
};
class AGate: public AGraphics_Objects
{
public:
	AGate(int x_pos, int y_pos, int level_x = -1, int level_y = -1); // -1 означает отсутсвие кирпичей

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Open_Gate(bool is_open_short); 
	void Redraw_Gate();
	bool Is_Opened();
	bool Is_Closed();
	void Get_Pos(double &x_pos, double &y_pos);

	static const int Gates_Jumper_Count = 5; // кол-во перегородок (пар)
	int Level_X, Level_Y;
private:
	int X_Pos;
	double Y_Pos, Original_Y_Pos;
	double Gap_Height;
	int Gate_Close_Tick;
	EGate_State Gate_State;
	EGate_Transformation Gate_Transformation;
	RECT Gate_Rect;

	static const double Max_Gap_Short_Height, Gap_Short_Gate_Expanding_Step;
	static const double Max_Gap_Long_Height, Gap_Long_Gate_Expanding_Step;
	static const int Short_Opening_Timeout = AsConfig::FPS; // 1 секунда в открытом состоянии гейта
	static const int Long_Opening_Timeout = AsConfig::FPS * 3; // 3 секунды в открытом состоянии гейта

	void Draw_Cup(HDC hdc, RECT &paint_area, bool is_top_cup);
	void Draw_Charge(HDC hdc);
	void Draw_Edges(HDC hdc, RECT &paint_area);
	void Draw_Short_Edges(HDC hdc);
	void Draw_Long_Edges(HDC hdc);
	void Draw_One_Edge(HDC hdc, int edge_offset, bool is_blue);
	void Draw_Red_Edge(HDC hdc, int edge_offset, bool has_blick, bool is_long);
	bool Act_For_Open(bool &correct_pos);
	void Correct_Gate_Y_Pos();
};