
#include "Engine.h"



int8_t Level_01[ALevel::Level_Height][ALevel::Level_Width]{
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
//ALevel--------------------------------------------------------------------------
//----------------------------------------------------------------------------

void ALevel::Init()
{
    Letter_Pen = CreatePen(PS_SOLID, AsEngine::Global_Scale, RGB(255, 255, 255)); // белый
    AsEngine::Create_Pen_Brush(255, 85, 85, Brick_Red_Pen, Brick_Red_Brush);//
    AsEngine::Create_Pen_Brush(85, 225, 255,Brick_Blue_Pen, Brick_Blue_Brush);//
     
   Level_Rect.left = Level_X_Offset * AsEngine::Global_Scale;
   Level_Rect.top = Level_Y_Offset * AsEngine::Global_Scale;
   Level_Rect.right = Level_Rect.left + Cell_Wight *  Level_Width * AsEngine::Global_Scale;
   Level_Rect.bottom = Level_Rect.top + Cell_Height * Level_Height * AsEngine::Global_Scale;
}

void ALevel::Draw_Brick(HDC hdc, int x, int y, EBrick_Type brick_type)
{// отрисовка кирпича

   HPEN pen;
   HBRUSH brush;

   switch (brick_type)
   {
   case EBT_None:
      return;
   case EBT_Red:
      pen = Brick_Red_Pen;
      brush = Brick_Red_Brush;
      break;

   case EBT_Blue:
      pen = Brick_Blue_Pen;
      brush = Brick_Blue_Brush;
      break;

   default:
      return;
   }
   SelectObject(hdc, pen);
   SelectObject(hdc, brush);

   RoundRect(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + Brick_Width) * AsEngine::Global_Scale, (y + Brick_Height) * AsEngine::Global_Scale, 2 * AsEngine::Global_Scale, 2 * AsEngine::Global_Scale);

}

void ALevel::Set_Brick_Letter_Colors(bool is_switch_color,HPEN &back_pen, HPEN &front_pen, HBRUSH &back_brush, HBRUSH &front_brush)
{
   if (is_switch_color)// выбираем цвет
   {
      front_brush = Brick_Red_Brush;
      front_pen = Brick_Red_Pen;

      back_brush = Brick_Blue_Brush;
      back_pen = Brick_Blue_Pen;
   }
   else
   {
      front_brush = Brick_Blue_Brush;
      front_pen = Brick_Blue_Pen;

      back_brush = Brick_Red_Brush;
      back_pen = Brick_Red_Pen;

   }
}

void ALevel::Draw_Brick_Letter(HDC hdc, int x, int y, EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step)
{//����� ��������� �������
   double offset;
   bool switch_color;
   double rotation_angle;//угол поворота для матрицы поворота
   int brick_half_height = Brick_Height * AsEngine::Global_Scale / 2;
   int back_part_offset;
   HPEN back_pen, front_pen;
   HBRUSH back_brush, front_brush;
   XFORM xform, old_xform;

   if (brick_type != EBT_Blue && brick_type != EBT_Red)
      return;// 

   if (rotation_step > 8)
   {
      rotation_angle = 2.0 * M_PI / 16.0 * fabs((double)(8 - rotation_step));
   }
   else
      rotation_angle = 2.0 * M_PI / 16.0 * (double)rotation_step;

   if (rotation_step > 4 and rotation_step <= 12)
   {
      if (brick_type == EBT_Blue)
         switch_color = true;
      else
         switch_color = false;
   }
   else
   {
      if (brick_type == EBT_Red)
         switch_color = true;
      else
         switch_color = false;
   }
   Set_Brick_Letter_Colors(switch_color, back_pen, front_pen, back_brush, front_brush);

   if (rotation_step == 4 or rotation_step == 12)
   {
      
      // ������� ������ ����
      SelectObject(hdc, back_pen);
      SelectObject(hdc, back_brush);

      Rectangle(hdc, x, y + brick_half_height - AsEngine::Global_Scale, x + Brick_Width * AsEngine::Global_Scale, y + brick_half_height);
      // ������� �������� ����
      SelectObject(hdc, front_pen);
      SelectObject(hdc, front_brush);

      Rectangle(hdc, x, y + brick_half_height, x + Brick_Width * AsEngine::Global_Scale, y + brick_half_height + AsEngine::Global_Scale - 1);
   
   }
   else {
      //����������� ������� ���������� �����
      SetGraphicsMode(hdc, GM_ADVANCED);

      xform.eM11 = 1.0f;
      xform.eM12 = 0.0f;
      xform.eM21 = 0.0f;
      xform.eM22 = (float)cos(rotation_angle);
      xform.eDx = (float)x;
      xform.eDy = (float)y + (float)brick_half_height;
      GetWorldTransform(hdc, &old_xform);
      SetWorldTransform(hdc, &xform);
      //�������  ������ ����
      SelectObject(hdc, back_pen);
      SelectObject(hdc, back_brush);

      offset = 3.0 * (1.0 - fabs(xform.eM22)) * (double)AsEngine::Global_Scale;
      back_part_offset = (int)round(offset);

      Rectangle(hdc, 0, -brick_half_height - back_part_offset, Brick_Width * AsEngine::Global_Scale, brick_half_height - back_part_offset);
      //������� �������� ����
      SelectObject(hdc, front_pen);
      SelectObject(hdc, front_brush);

      Rectangle(hdc, 0, -brick_half_height, Brick_Width * AsEngine::Global_Scale, brick_half_height);

      if (rotation_step > 4 and rotation_step <= 12)
      {
         SelectObject(hdc, Letter_Pen);
         Ellipse(hdc, 0 + 5 * AsEngine::Global_Scale, -brick_half_height, 0 + 10 * AsEngine::Global_Scale, brick_half_height);
      }

      SetWorldTransform(hdc, &old_xform);
   }
}


void ALevel::Draw(HDC hdc,RECT &paint_area)
{//рисуем весь уровень кирпичей
   RECT intersertion_rect;
   int i, j;
      if (! IntersectRect(&intersertion_rect, &paint_area, &Level_Rect))
         return;

   for (i = 0; i < Level_Height; ++i) {
      for (j = 0; j < Level_Width; ++j) {
         Draw_Brick(hdc, Level_X_Offset + j * Cell_Wight, Level_Y_Offset + i * Cell_Height, (EBrick_Type)Level_01[i][j]);
      }
   }
}

void ALevel::Check_Level_Brick_Hit(int &next_y_pos, double &ball_direction)
{   //отражение шарика от кирпича

   int i(0), j(0);
   int brick_y_pos = Level_Y_Offset + Level_Height * Cell_Height;
   for (i = Level_Height - 1; i >= 0; i--) 
   {
      for (j = 0; j < Level_Width; j++) 
      {
         if (Level_01[i][j] == 0)
            continue;

         if (next_y_pos < brick_y_pos) 
         {
            next_y_pos = brick_y_pos - (next_y_pos - brick_y_pos);
            ball_direction = -ball_direction;
         }
      }
      brick_y_pos -= Cell_Height;
   }
}
//----------------------------------------------------------------------------
// ABall
 ABall::ABall()
    : Ball_X_Pos(20), Ball_Y_Pos(170), Ball_Speed(3.0), Ball_Direction(M_PI - M_PI_4)

 {
 }
 void ABall:: Init()
 {
    AsEngine::Create_Pen_Brush(255, 255, 255, Ball_Pen, Ball_Brush);

 }
void ABall::Draw(HDC hdc, RECT& paint_area, AsEngine *engine)
{
    RECT intersertion_rect;
      if (!IntersectRect(&intersertion_rect, &paint_area, &Ball_Rect))
         return;

   // 1. перерисовываем старое место шарика
   SelectObject(hdc, engine->BG_Pen);
   SelectObject(hdc, engine->BG_Brush);

   Ellipse(hdc, Prev_Ball_Rect.left , Prev_Ball_Rect.top, Prev_Ball_Rect.right - 1, Prev_Ball_Rect.bottom - 1);
   /*SelectObject(hdc, Brick_Blue_Brush);
   SelectObject(hdc, Brick_Blue_Pen);*/
   //Rectangle(hdc, Prev_Ball_Rect.left, Prev_Ball_Rect.top, Prev_Ball_Rect.left + Ball_Size * Global_Scale + 1, Prev_Ball_Rect.top + Ball_Size * Global_Scale + 1);

  /* SelectObject(hdc, Platform_Circle_Pen);
   SelectObject(hdc, Platform_Circle_Brush);

   Ellipse(hdc, Prev_Ball_Rect.left - 2 , Prev_Ball_Rect.top - 2, Prev_Ball_Rect.right + 1, Prev_Ball_Rect.bottom + 1);*/

   //
   SelectObject(hdc, Ball_Pen);
   SelectObject(hdc, Ball_Brush);

   // рисуем новый шарик 
   Ellipse(hdc, Ball_Rect.left, Ball_Rect.top , Ball_Rect.right - 1, Ball_Rect.bottom - 1);
   /*SelectObject(hdc, engine->BG_Pen);
   SelectObject(hdc, engine->BG_Brush);
  */

  // Rectangle(hdc, Prev_Ball_Rect.left, Prev_Ball_Rect.top, Prev_Ball_Rect.right + Ball_Size * Global_Scale, Prev_Ball_Rect.bottom + Ball_Size * Global_Scale);

 

}

void ABall::Move(AsEngine *engine, ALevel *level, AsPlatform *platform)
{
   int next_x_pos, next_y_pos;

   int max_x_pos = AsEngine::Max_X_Pos - Ball_Size;
   int platform_y_pos = AsPlatform::Y_Pos - Ball_Size;

  /* Prev_Ball_Rect.left = Ball_Rect.left;
   Prev_Ball_Rect.top = Ball_Rect.top;
   Prev_Ball_Rect.right = Ball_Rect.right;
   Prev_Ball_Rect.bottom = Ball_Rect.bottom;*/
   Prev_Ball_Rect = Ball_Rect;
   //задавание след позиции шарика
   next_x_pos = Ball_X_Pos + (int)(Ball_Speed * cos(Ball_Direction));
   next_y_pos = Ball_Y_Pos - (int)(Ball_Speed * sin(Ball_Direction));
   //отражение шарика от границ
   if (next_x_pos < AsBorder::Border_X_Offset) 
   {
      next_x_pos = ALevel::Level_X_Offset - (next_x_pos - ALevel::Level_X_Offset);
      Ball_Direction = M_PI - Ball_Direction;
   }
   
   if (next_y_pos < AsBorder::Border_Y_Offset)
   {
      next_y_pos = AsBorder::Border_Y_Offset - (next_y_pos - AsBorder::Border_Y_Offset);
      Ball_Direction = -Ball_Direction;   
   }

   if (next_x_pos > max_x_pos) 
   {
      next_x_pos = max_x_pos - (next_x_pos - max_x_pos);
      Ball_Direction = M_PI - Ball_Direction;
   }
   if (next_y_pos > AsEngine::Max_Y_Pos) 
   {
      next_y_pos = AsEngine::Max_Y_Pos - (next_y_pos - AsEngine::Max_Y_Pos);
      Ball_Direction = M_PI + (M_PI - Ball_Direction);
   }
   //������������ ��������� ������ �� ��������
   if (next_y_pos > platform_y_pos) 
   {
      if (next_x_pos >= platform->X_Pos and next_x_pos <= platform->X_Pos + platform->Width)
      {
         next_y_pos = platform_y_pos - (next_y_pos - platform_y_pos);
         Ball_Direction = M_PI + (M_PI - Ball_Direction);
      }
   }

   //������������ ��������� ������ �� ��������
   level->Check_Level_Brick_Hit(next_y_pos, Ball_Direction);
    

   Ball_X_Pos = next_x_pos;
   Ball_Y_Pos = next_y_pos;
   //������� �����
   Ball_Rect.left =  Ball_X_Pos * AsEngine::Global_Scale;
   Ball_Rect.top =   Ball_Y_Pos * AsEngine::Global_Scale;
   Ball_Rect.right = Ball_Rect.left +  Ball_Size * AsEngine::Global_Scale;
   Ball_Rect.bottom = Ball_Rect.top +  Ball_Size  * AsEngine::Global_Scale;

   InvalidateRect(engine->Hwnd, &Prev_Ball_Rect, FALSE);
   InvalidateRect(engine->Hwnd, &Ball_Rect, FALSE);
}
//----------------------------------------------------------------------------
// APlatrorm
AsPlatform::AsPlatform()
 : Inner_width(21), X_Step(AsEngine::Global_Scale), Width(28), X_Pos(AsBorder::Border_X_Offset)
{
}

void AsPlatform:: Init()
 {
     AsEngine::Create_Pen_Brush(151, 0, 0, Platform_Circle_Pen, Platform_Circle_Brush);//
     AsEngine::Create_Pen_Brush(0, 128, 192, Platform_Inner_Pen, Platform_Inner_Brush);//
     Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255)); // белый

 }

void AsPlatform::Draw(HDC hdc, AsEngine *engine, RECT& paint_area)
{
   int x = X_Pos;
   int y = Y_Pos;
         
   RECT intersertion_rect;
    if (! IntersectRect(&intersertion_rect, &paint_area, &Platform_Rect))
         return;

   //перерисовка старой платформы цветом бэкграунда
   SelectObject(hdc, engine->BG_Pen);
   SelectObject(hdc, engine->BG_Brush);
  // Rectangle(hdc, x * Global_Scale, y * Global_Scale, (x + Width) * Global_Scale, (y + Height) * Global_Scale);
   Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);
   //1. рисуем шарики
   SelectObject(hdc, Platform_Circle_Pen);
   SelectObject(hdc, Platform_Circle_Brush);

   Ellipse(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + Circle_size) * AsEngine::Global_Scale, (y + Circle_size) * AsEngine::Global_Scale);
   Ellipse(hdc, (x + Inner_width) * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + Circle_size + Inner_width) * AsEngine::Global_Scale, (y + Circle_size) * AsEngine::Global_Scale);

   //2.рисуем блик
   SelectObject(hdc, Highlight_Pen);
   Arc(hdc, (x + 1) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + Circle_size - 1) * AsEngine::Global_Scale, (y + Circle_size - 1) * AsEngine::Global_Scale,
      (x + 1 + 1) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 1) * AsEngine::Global_Scale, (y + 1 + 2) * AsEngine::Global_Scale);
   
   //3. рисуем внутренню часть платформы
   SelectObject(hdc, Platform_Inner_Pen);
   SelectObject(hdc, Platform_Inner_Brush);

   RoundRect(hdc, (x + 4) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 4 + Inner_width - 1) * AsEngine::Global_Scale, (y + 1 + 5) * AsEngine::Global_Scale, 3 * AsEngine::Global_Scale, 3 * AsEngine::Global_Scale);
}


void AsPlatform::Redraw_Platform(AsEngine *engine)
{// перерисовка платформы
   Prev_Platform_Rect = Platform_Rect;
   
   Platform_Rect.left = X_Pos * AsEngine::Global_Scale;
   Platform_Rect.top = Y_Pos * AsEngine::Global_Scale;
   Platform_Rect.right = Platform_Rect.left + Width * AsEngine::Global_Scale;
   Platform_Rect.bottom = Platform_Rect.top + Height * AsEngine::Global_Scale;

   InvalidateRect(engine->Hwnd, &Prev_Platform_Rect, FALSE);

   InvalidateRect(engine->Hwnd,&Platform_Rect, FALSE);

}


//AsBorder----------------------------------------------------------------------------

void AsBorder::Init()
{
   AsEngine::Create_Pen_Brush(85, 225, 255, Border_Blue_Pen, Border_Blue_Brush);
   AsEngine::Create_Pen_Brush(255, 255, 255, Border_White_Pen, Border_White_Brush);// белый
}
  
void AsBorder::Draw_Element(HDC hdc, int x, int y, bool top_border,AsEngine * engine )
{//рисуем элемент рамки
   //голубая часть рамки
   SelectObject(hdc, Border_Blue_Pen);
   SelectObject(hdc, Border_Blue_Brush);

   if (top_border) 
      Rectangle(hdc, x * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 4) * AsEngine::Global_Scale, (y + 4) * AsEngine::Global_Scale);
   else
      Rectangle(hdc, (x + 1) * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + 4) * AsEngine::Global_Scale, (y + 4) * AsEngine::Global_Scale);

   //белая часть рамки
   SelectObject(hdc, Border_White_Pen);
   SelectObject(hdc, Border_White_Brush);

   if (top_border)
      Rectangle(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + 4) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale);
   else
      Rectangle(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + 1) * AsEngine::Global_Scale, (y + 4) * AsEngine::Global_Scale);
   // фоновая часть рамки
   SelectObject(hdc, engine->BG_Pen);
   SelectObject(hdc, engine->BG_Brush);


   if (top_border)
      Rectangle(hdc, (x + 2) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 3) * AsEngine::Global_Scale, (y + 2) * AsEngine::Global_Scale);
   else
      Rectangle(hdc, (x + 2) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 3) * AsEngine::Global_Scale, (y + 2) * AsEngine::Global_Scale);
}

void AsBorder::Draw(HDC hdc, RECT& paint_area, AsEngine *engine)
{// рисуем все рамки
   int i;
   //рисуем левую рамку
   for (i = 0; i < 50; i++)
      Draw_Element(hdc, 2, 1 + i * 4, false, engine);
   //рисуем правую рамку
   for (i = 0; i < 50; i++)
      Draw_Element(hdc, 201, 1 + i * 4, false, engine);
   //рисуем верхнюю рамку
   for (i = 0; i < 50; i++)
     Draw_Element(hdc, 3 + i * 4, 0, true, engine);
}



//AsEngine-------------------------------------------------------------------------
AsEngine::AsEngine()
{
}
    
      

void AsEngine::Init_Engine(HWND hwnd)
{
   Hwnd = hwnd;

   Create_Pen_Brush(15, 63, 31, BG_Pen, BG_Brush);//

   Level.Init();

   Ball.Init();
   Platform.Init();
   Border.Init();
   Platform.Redraw_Platform(this);

   SetTimer(Hwnd, Timer_ID, 50, 0);
}

void AsEngine::Draw_Frame(HDC hdc, RECT& paint_area)
{// рисуем все окно
   /*int i;

   for (i = 0; i < 16; ++i) {
      Draw_Brick_Letter(hdc, 20 + i * Cell_Wight * Global_Scale, 100, EBT_Blue,ELT_O, i);
      Draw_Brick_Letter(hdc, 20 + i * Cell_Wight * Global_Scale, 130, EBT_Red,ELT_O, i);
   }*/

      Level.Draw(hdc, paint_area);
      Platform.Draw(hdc, this, paint_area);

      Ball.Draw(hdc, paint_area, this);

   Border.Draw(hdc, paint_area, this);

}

int AsEngine::On_Key_Down(EKey_Type key_type)
{
   switch (key_type)
   {
   case EKT_Left:
      Platform.X_Pos -= Platform.X_Step;

      if (Platform.X_Pos <= AsBorder::Border_X_Offset)
         Platform.X_Pos = AsBorder::Border_X_Offset;
      Platform.Redraw_Platform(this);
      break;

   case EKT_Right:
      Platform.X_Pos += Platform.X_Step;

      if (Platform.X_Pos >= Max_X_Pos - Platform.Width + 1)
         Platform.X_Pos = Max_X_Pos - Platform.Width + 1;
      Platform.Redraw_Platform(this);

      break;

   case EKT_Space:
       break;
   }

   return 0;
}

int AsEngine::On_Timer()
{
   Ball.Move(this, &Level, &Platform);
   return 0;
}

void AsEngine::Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN &pen,HBRUSH &brush)
{
   pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
   brush = CreateSolidBrush(RGB(r, g, b));
}















