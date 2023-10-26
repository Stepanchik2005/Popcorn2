#pragma once
#include <Windows.h>


#include "Border.h"
#include "Ball.h"

enum EKey_Type
{
   EKT_Left,
   EKT_Right,
   EKT_Space
};

//----------------------//----------------------//----------------------
const int Timer_ID = WM_USER + 1;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


class AsEngine;

//----------------------------------------------------------------------------
class AsPlatform{
public:
    AsPlatform();
    void Init();
    void Draw(HDC hdc, AsEngine *engine, RECT& paint_area);
    void Redraw_Platform(AsEngine *engine);

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
   //AsBorder---------------------------------------------------------------------------


//----------------------------------------------------------------------------


//-Config---------------------------------------------------------------------------



//----------------------------------------------------------------------------

class AsEngine
{
public:
   AsEngine();

   void Init_Engine(HWND hwnd);
   void Draw_Frame(HDC hdc,RECT &paint_area);
   int On_Key_Down(EKey_Type key_type);
   int On_Timer();


  
  


   HWND Hwnd;
   HPEN BG_Pen;
   HBRUSH BG_Brush;
  
private:
  
   ABall Ball;
   ALevel Level;
   AsPlatform Platform;
   AsBorder Border;

   
  
};
//----------------------//----------------------//----------------------