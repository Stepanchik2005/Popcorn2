#pragma once
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
class AColor
{
public:
   AColor(unsigned char r, unsigned char g, unsigned char b);

   unsigned char R, G, B;
};

class AsConfig
{
public:
   static void Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen, HBRUSH &brush);
   static void Create_Pen_Brush(const AColor &color, HPEN &pen,HBRUSH &brush);
   static void Setup_Colors();
   static int Rand(int range);

  

   static const AColor Blue_Brick_Color;
   static const AColor Red_Brick_Color;
   static const AColor BG_Color;

   static HPEN BG_Pen;
   static HBRUSH BG_Brush;
   static HWND Hwnd;
   static const int Global_Scale = 3;

   static const int Border_X_Offset = 6;
   static const int Border_Y_Offset = 4;

   static const int Level_Height = 14;
   static const int Level_Width = 12;

   static const int Cell_Wight = 16;
   static const int Cell_Height = 8;
  
   static const int Level_Y_Offset = 6;
   static const int Level_X_Offset = 8;

   static const int Y_Pos = 185; // Platform Y_Pos
   static const int Ball_Size = 4;

   static const int Max_X_Pos = Level_X_Offset + Cell_Wight * Level_Width;
   static const int Max_Y_Pos = 199;
  
   static const int FPS = 50;

   static const int Brick_Height = 7;
   static const int Brick_Width = 15;
  
   
   static int Current_Time_Tick;
   static bool Level_Has_Floor;
};