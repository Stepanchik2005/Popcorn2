#pragma once
#include "Config.h"
enum EBrick_Type
{
   EBT_None,
   EBT_Red,
   EBT_Blue
};
class AActive_Brick
{
public: 
   AActive_Brick(EBrick_Type Brick_Type);
   void Draw( HDC hdc,RECT &paint_area);
   void Act();
   static void Setup_Colors();

private:
   static unsigned char Get_Fading_Channel(unsigned char color, unsigned char bg_color, int step);
   static void Get_Fading_Color(const AColor &color,int step, HPEN &pen, HBRUSH &brush);

   int Fade_Step;

   static const int Max_Fade_Step = 40;

   HPEN pen;
   HBRUSH brush;
   EBrick_Type Brick_Type;
   RECT brick_rect{};



    static HPEN   Fading_Blue_Bricks_Pen[Max_Fade_Step];
    static HBRUSH Fading_Blue_Bricks_Brushes[Max_Fade_Step];
    static HPEN   Fading_Red_Bricks_Pen[Max_Fade_Step];
    static HBRUSH Fading_Red_Bricks_Brushes[Max_Fade_Step];
};