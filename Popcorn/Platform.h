#pragma once
#include "Config.h"

class AsPlatform{
public:
    AsPlatform();
    void Init();
    void Draw(HDC hdc, RECT& paint_area);
    void Redraw_Platform(HWND hwnd);

    int X_Pos;
    int Width;
    int X_Step;


private:
   int Inner_width;
   
   HPEN  Platform_Circle_Pen, Platform_Inner_Pen, Highlight_Pen;
   HBRUSH Platform_Circle_Brush,  Platform_Inner_Brush;

   RECT Platform_Rect, Prev_Platform_Rect;

   static const int Height = 7;
   static const int Circle_size = 7;
};

