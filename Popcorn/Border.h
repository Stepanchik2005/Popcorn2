#pragma once
#include <Windows.h>
#include "Config.h"


class AsBorder
{
public:
   void Init();
   void Draw(HDC hdc, RECT &paint_area, HPEN bg_pen, HBRUSH bg_brush);



private:
   void Draw_Element(HDC hdc, int x, int y, bool top_border, HPEN bg_pen, HBRUSH bg_brush);

   HPEN   Border_Blue_Pen, Border_White_Pen;
   HBRUSH  Border_Blue_Brush, Border_White_Brush;
};