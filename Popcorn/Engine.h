#pragma once
#include <Windows.h>
#include <cstdint>
#define _USE_MATH_DEFINES
#include <math.h>

enum EKey_Type
{
   EKT_Left,
   EKT_Right,
   EKT_Space
};
enum ELetter_Type
{
   ELT_None,
   ELT_O
};
enum EBrick_Type
{
   EBT_None,
   EBT_Red,
   EBT_Blue
};
//----------------------//----------------------//----------------------
const int Timer_ID = WM_USER + 1;
//----------------------------------------------------------------------------
class ABall
{
public:
   void Move_Ball();
   void Draw_Ball(HDC hdc, RECT &paint_area);
private:
   int Ball_X_Pos;
   int Ball_Y_Pos;
   double Ball_Speed, Ball_Direction;
};
//----------------------------------------------------------------------------
class AsEngine{
public:
   AsEngine();
   void Init_Engine(HWND hwnd);
   void Draw_Frame(HDC hdc,RECT &paint_area);
   int On_Key_Down(EKey_Type key_type);
   int On_Timer();

   static const int Level_Width = 12;
   static const int Level_Height = 14;
private:
   void Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush);
   void Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type);
   void Draw_Platform(HDC hdc, int x, int y);
   void Set_Brick_Letter_Colors(bool is_switch_color, HPEN &back_pen, HPEN &front_pen, HBRUSH &back_brush, HBRUSH &front_brush);
   void Draw_Brick_Letter(HDC hdc, int x, int y, EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step);
   void Draw_Level(HDC hdc);
   void Draw_Border(HDC hdc, int x, int y, bool top_border);
   void Draw_Bounds(HDC hdc, RECT &paint_area);
   void Redraw_Platform();
   void Check_Level_Brick_Hit(int &next_y_pos);

   HWND Hwnd;
   HPEN Letter_Pen, Brick_Red_Pen, Brick_Blue_Pen, Platform_Circle_Pen, BG_Pen, Platform_Inner_Pen, Highlight_Pen, Ball_Pen, Border_Blue_Pen, Border_White_Pen;
   HBRUSH Brick_Red_Brush, Brick_Blue_Brush, Platform_Circle_Brush, Platform_Inner_Brush, BG_Brush, Ball_Brush, Border_Blue_Brush, Border_White_Brush;

   int Inner_width;
   int Platform_X_Step;
   int Platform_Width;
   int Platform_X_Pos;

   ABall Ball;

   RECT Platform_Rect, Prev_Platform_Rect;
   RECT Level_Rect;
   RECT Ball_Rect, Prev_Ball_Rect;

   static const int Global_Scale = 3;
   static const int Platform_Height = 7;
   static const int Brick_Height = 7;
   static const int Brick_Width = 15;
   static const int Cell_Wight = 16;
   static const int Cell_Height = 8;
   static const int Level_X_Offset = 8;
   static const int Level_Y_Offset = 6;
   static const int Circle_size = 7;
   static const int Platform_Y_Pos = 185;
   static const int Ball_Size = 4;
   static const int Max_X_Pos = Level_X_Offset + Cell_Wight * Level_Width;
   static const int Max_Y_Pos = 199 - Ball_Size;
   static const int Border_X_Offset = 6;
   static const int Border_Y_Offset = 4;
};
//----------------------//----------------------//----------------------