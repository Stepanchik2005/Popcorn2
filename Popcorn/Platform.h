#pragma once
#include "Config.h"
enum EPlatform_State
{
   EPS_Missing,
   EPS_Ready,
   EPS_Normal,
   EPS_Meltdown,
   EPS_Roll_In,
   EPS_Expand_Roll_In
};
class AsPlatform{
public:
    AsPlatform();
    void Init();
    void Draw(HDC hdc, RECT& paint_area);
    void Setup_State(EPlatform_State new_state);

    void Redraw_Platform();
    void Act();

    int X_Pos;
    int Width;
    int Rolling_Step;
    int X_Step;

    EPlatform_State Get_State();

private:
   void Clear_BG(HDC hdc);
   void Draw_Circle_Highlight(HDC hdc,int x, int y);
   void Draw_Normal_State(HDC hdc, RECT& paint_area);
   void Draw_Meltdown_State(HDC hdc, RECT& paint_area);
   void Draw_Roll_In_State(HDC hdc, RECT& paint_area);
   void Draw_Expanding_Roll_In_State(HDC hdc, RECT &paint_area);

   EPlatform_State Platform_State;
   int Inner_width;
   
   static const int Normal_Width = 28;

   int Melt_Platform_Y_Pos[Normal_Width * AsConfig::Global_Scale];

   HPEN  Platform_Circle_Pen, Platform_Inner_Pen, Highlight_Pen;
   HBRUSH Platform_Circle_Brush,  Platform_Inner_Brush;

   RECT Platform_Rect, Prev_Platform_Rect;

   static const int Height = 7;
   static const int Circle_Size = 7;
   static const int Melt_Down_Speed = 4;
   static const int Normal_Platform_Inner_Width = 21;
   static const int Max_Rolling_Step = 16;
   static const int Roll_In_Platform_End_X_Pos = 99;
   static const int Rolling_Platform_Speed = 3; 
};

