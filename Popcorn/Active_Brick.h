#pragma once
#include "Config.h"

class AActive_Brick
{
public: 
   AActive_Brick();
   void Draw(HWND hwnd, HDC hdc,RECT &paint_area);
   void Act(HWND hwnd);

   int Fade_Step;

  
private:
   

    HPEN pen;
    HBRUSH brush;

    RECT brick_rect{};

    static const int Max_Fade_Step = 40;
    
};