#pragma once

#include "Ball_Set.h"
#include "Falling_Letter.h"
//------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------
enum class EFigure_Type : unsigned char
{
	Ellipse,
	Rectangle,
	Round_Rect
};
enum class EPlatform_State: unsigned char
{
	Unknown,

	Regular,
	Meltdown,
	Rolling,
	Glue,
	Expanding,
	Laser
};

enum class EPlatform_Substate_Regular : unsigned char
{
	Unknown,

	Missing,
	Ready,
	Normal,
};
enum class EPlatform_Substate_Meltdown: unsigned char
{
	Unknown,

	Init,
	Active
};
enum class EPlatform_Substate_Rolling : unsigned char
{
	Unknown,

	Roll_In,
	Expand_Roll_In,
};
enum class EPlatform_Transformation : unsigned char
{
	Unknown,

	Init,
	Active,
	Finalize
};
enum class EPlatform_Moving_State: unsigned char
{
	Stopping,
	Stop,
	Left,
	Right
};

class AsPlatform_State
{
public:
	AsPlatform_State();

	void Set_Next_State(EPlatform_State next_state);
	EPlatform_State Get_Next_State();
	EPlatform_State Set_State(EPlatform_Substate_Regular new_regular_state);
	EPlatform_State Set_New_Or_Regular_State(EPlatform_Substate_Regular new_state);

	operator EPlatform_State() const;
	void operator = (EPlatform_State new_state);

	EPlatform_Substate_Regular Regular;
	EPlatform_Substate_Meltdown Meltdown;
	EPlatform_Substate_Rolling Rolling;
   EPlatform_Transformation Glue;
	EPlatform_Transformation Expanding;
	EPlatform_Transformation Laser;
	EPlatform_Moving_State Moving;
	

private:
	EPlatform_State Current_State;
	EPlatform_State Next_State;
};
//------------------------------------------------------------------------------------------------------------

class AsPlatform_Glue
{
public:
	AsPlatform_Glue(AsPlatform_State &platform_state);


	bool Act(EPlatform_Transformation &glue_state, AsBall_Set *ball_set, EPlatform_State &next_state);
	void Draw_Glue_State(HDC hdc, double x_pos);
	void Draw_Spot_State(HDC hdc, int x_offset, int width, int height, double x_pos);
	void Reset();

private:
	double Platform_Glue_Ratio;
	AsPlatform_State *Platform_State;

	static const double Max_Glue_Ratio, Min_Glue_Ratio, Glue_Spot_Height_Ratio_Step;
};



class AsPlatform_Expanding
{
public:
	~AsPlatform_Expanding();

	AsPlatform_Expanding(AsPlatform_State &platform_state);

	void Init(AColor &inner_color, AColor &circle_color, AColor &highlight_color);
	bool Act_For_Expanding_State(double &x_pos, bool &is_correct, EPlatform_State &next_state);
	void Draw_Expanding_State(HDC hdc, double x);
	void Draw_Circle_Highlight(HDC hdc, int x, int y);
	void Reset();

	double Expanding_Platform_Width;
private:
	void Draw_Expanding_Ball(HDC hdc, double x, bool is_left);
	void Draw_Expanding_Truss(HDC hdc, RECT &inner_rect, bool is_left);

	
	AsPlatform_State *Platform_State;
	
	AColor *Inner_Color, *Circle_Color, *Highlight_Color; // UNO Use, Not Own
	AColor *Truss_Color;
	static const double Max_Expanding_Size, Min_Expanding_Size, Expanding_Platform_Step;
	static const int Expanding_Platform_Inner_Width = 12; // игровые пиксели
};



enum class Elaser_Beam_State
{
	Disabled,
	Active,
	Stopping,
	Cleanup
};


class ALaser_Beam: public AMover, public AGraphics_Objects
{
public:
	ALaser_Beam();

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Set_At(double x_pos, double y_pos);
	bool Is_Active();

	static void Add_Hit_Checker(AHit_Checker *hit_checker);

	static AHit_Checker *Hit_Checkers[3]; 

	Elaser_Beam_State Laser_Beam_State;

	

private:
	double X_Pos, Y_Pos; 
	double Speed;
	RECT Beam_Rect, Prev_Beam_Rect;
	static const int Width = 1;
	static const int Height = 3;
	static int Hit_Checkers_Count;

	void Redraw_Beam();
};




class AsLaser_Beam_Set: public AMover, public AGraphics_Objects
{  
public:
	virtual void Advance(double max_speed);
	virtual double Get_Speed();
   virtual void Start_Movement();
   virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Init();
	void Fire(double left_gun_x_pos, double right_gun_x_pos);
private:
	static const int Max_Count_Beams = 10;

	ALaser_Beam Beams[AsLaser_Beam_Set::Max_Count_Beams];

};




class AsPlatform_Laser
{
public:
	~AsPlatform_Laser();

	AsPlatform_Laser(AsPlatform_State &platform_state, AsLaser_Beam_Set *laser_beam_set);

	void Init(AsLaser_Beam_Set *laser_beam_set, AColor &inner_color, AColor &circle_color,  AColor &highlight_color);
	bool Act_For_Laser_State(EPlatform_State &next_state, double x_pos);
	void Draw_Laser_State(HDC hdc, double x ,RECT &platform_rect); 
	void Reset();
	void Fire(bool is_enable_firing, double x);

private:
	void Draw_Inner_Part(HDC hdc, double x);
	void Draw_Laser_Wing(HDC hdc, double x_pos, bool is_left);
	void Draw_Laser_Leg(HDC hdc, double x_pos, bool is_left);
	void Draw_Laser_Cabin(HDC hdc, double x_pos);
	void Draw_Expanding_Figure(HDC hdc, EFigure_Type figure_type, double start_x, double start_y, double start_width, double start_height, double ratio, double end_x, double end_y, double end_width, double end_height);
	double Get_Expanding_Value(HDC hdc, double start_pos, double end_pos, double ratio);
	double Get_Gun_Pos(bool is_left, double platform_x_pos);
	AsPlatform_State *Platform_State;

	AColor *Inner_Color, *Circle_Color; // UNO
	AColor *Gun_Color;
	AsLaser_Beam_Set *Laser_Beam_Set; // UNO
	int Laser_Transformation_Step;
	int Last_Laser_Timer_Shot;	
	bool Enable_Laser_Firing;

	static const int Max_Laser_Transformation_Step = 20;
	static const int Laser_Shot_Timeout = AsConfig::FPS / 2;
};





//------------------------------------------------------------------------------------------------------------
class AsPlatform: public AHit_Checker, public AMover, public AGraphics_Objects
{
public:
	~AsPlatform();

	AsPlatform();

	virtual bool Check_Hit(double next_x_pos, double next_y_pos, ABall *ball);

	virtual void Advance(double max_speed);
	virtual double Get_Speed();
	virtual void Start_Movement();
	virtual void End_Movement();

	virtual void Act();
	virtual bool Is_Finished();
	virtual void Draw(HDC hdc, RECT &paint_area);
	virtual void Clear(HDC hdc, RECT &paint_area);

	void Init(AsBall_Set *ball_set, AsLaser_Beam_Set *laser_beam_set);
	void On_Space_Key(bool key_down);
	EPlatform_State Get_State();
	void Set_State(EPlatform_State new_state);
	void Set_State(EPlatform_Substate_Regular new_regular_state);
	bool Has_Substate_Regular(EPlatform_Substate_Regular new_state);
	void Redraw_Platform();
   void Move(bool is_left, bool key_down);
	bool HitBy(AFalling_Letter *falling_letter);
	double Get_Middle_Pos();
	

	
	double X_Pos;
	static const int Height = 7;
	static const int Normal_Width = 28;
	static const int Circle_Size = 7; // диаметр
	static const int Normal_Platform_Inner_Width = Normal_Width - Circle_Size;

private:
	bool Set_Transformation_State(EPlatform_State new_state, EPlatform_Transformation &transformation_state);
	void Act_For_Rolling_State();
	void Act_For_Meltdown_State();
	bool Correct_Platform_Pos();
	void Clear_BG(HDC hdc, RECT &paint_area);
	void Draw_Expanding_Figure(HDC hdc, EFigure_Type figure_type, double start_x, double start_y, double start_width, double start_height, double ratio, double end_x, double end_y, double end_width, double end_height);
	double Get_Expanding_Value(HDC hdc, double start_pos, double end_pos, double ratio);


	void Draw_Normal_State(HDC hdc, RECT &paint_area);
	void Get_Normal_Platform_Image(HDC hdc);
	void Draw_Meltdown_State(HDC hdc, RECT &paint_area);
	void Draw_Roll_In_State(HDC hdc, RECT &paint_area);
	bool Reflect_On_Circle(double next_x_pos, double next_y_pos, double platform_ball_x_offset, ABall *ball);
	bool Get_Platform_Image_Stroke_Color(int x, int y, AColor &color, int &stroke_length);
	double Get_Platform_Width();
	
	AsPlatform_State Platform_State;
	AsPlatform_Glue Platform_Glue;
	AsPlatform_Expanding Platform_Expanding;
	AsPlatform_Laser Platform_Laser;

	int Inner_Width;
	int Rolling_Step;
	int Normal_Platform_Image_Width;
	int Normal_Platform_Image_Height;
	int Last_Timer_Tick;
	bool Left_Key_Dowm;
	bool Right_Key_Down;
	double Speed;
	int *Normal_Platform_Image; 

	int Meltdown_Platform_Y_Pos[Normal_Width * AsConfig::Global_Scale];

	RECT Platform_Rect, Prev_Platform_Rect;

   AColor Highlight_Color, Platform_Circle_Color, Platform_Inner_Color;
	AsBall_Set *Ball_Set; // UNO
	AsLaser_Beam_Set *Laser_Beam_Set; // UNO
	
	
	static const int Meltdown_Speed = 3;
	static const int Max_Rolling_Step = 16;
	static const int Roll_In_Platform_End_X_Pos = 99;
	static const int Rolling_Platform_Speed = 3;
	static const int X_Step = 6;
};
//------------------------------------------------------------------------------------------------------------
