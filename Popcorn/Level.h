#pragma once

#include <cstdint>
#include "Config.h"
#include "Active_Brick.h"
enum ELetter_Type
{
   ELT_None,
   ELT_O
};



class ALevel
{
public:
   ALevel();

   void Init();
   void Draw(HDC hdc,RECT &paint_area);
   void Check_Level_Brick_Hit(double &next_y_pos, double &ball_direction);

   
   AActive_Brick Active_Brick;

private:
   void Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type);
   void Set_Brick_Letter_Colors(bool is_switch_color, HPEN &back_pen, HPEN &front_pen, HBRUSH &back_brush, HBRUSH &front_brush);
   void Draw_Brick_Letter(HDC hdc, int x, int y, EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step);

   HPEN Brick_Red_Pen,Brick_Blue_Pen,Letter_Pen;
   HBRUSH Brick_Red_Brush, Brick_Blue_Brush;
   RECT Level_Rect;


   static char Level_01[AsConfig::Level_Height][AsConfig::Level_Width];
};