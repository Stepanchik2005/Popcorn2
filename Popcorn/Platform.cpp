#include "Platform.h"


// AsPlatform
//------------------------------------------------------------------------------------------------------------
AsPlatform::AsPlatform()
: X_Pos(AsConfig::Border_X_Offset), Platform_State(), Platform_Glue(Platform_State), Platform_Expanding(Platform_State), Platform_Laser(Platform_State, Laser_Beam_Set) ,Inner_Width(AsConfig::Platform_Normal_Inner_Width), 
  Rolling_Step(0), Normal_Platform_Image_Width(0), // !!!!!
  Normal_Platform_Image_Height(0), Last_Timer_Tick(0),Left_Key_Dowm(false), Right_Key_Down(false), Speed(0.0), Normal_Platform_Image(0),Platform_Rect{}, 
  Prev_Platform_Rect{}, Highlight_Color(255, 255, 255), Platform_Circle_Color(151, 0, 0), Platform_Inner_Color(0, 128, 192), Ball_Set(0), Laser_Beam_Set(0)

{
   X_Pos = (double)( (AsConfig::Max_X_Pos - AsConfig::Platform_Normal_Width) / 2);
}

//------------------------------------------------------------------------------------------------------------
AsPlatform::~AsPlatform()	
{
   delete [] Normal_Platform_Image;
}

//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{
   
   double inner_top_y; 
   double inner_low_y;
   double inner_left_x, inner_right_x;
   double reflection_pos;
   double inner_y;
   double ball_x, ball_y;
   int width;
   if(next_y_pos + ball->Radius < AsConfig::Platform_Y_Pos)
      return false;
   
   width = Get_Platform_Width();

   inner_top_y = (double) (AsConfig::Platform_Y_Pos + 1);
   inner_left_x = (double) (X_Pos + (AsConfig::Platform_Circle_Size - 1));
   inner_right_x = (double) (X_Pos + width - (AsConfig::Platform_Circle_Size - 1));
   inner_low_y = (double) (AsConfig::Platform_Y_Pos + (AsConfig::Platform_Height - 1));

   //1. Проверка отражения от мячика
   if(Reflect_On_Circle( next_x_pos, next_y_pos, 0, ball))
       goto _on_hit;// от левого
   
   if(Reflect_On_Circle( next_x_pos, next_y_pos, width - AsConfig::Platform_Circle_Size, ball))
       goto _on_hit; // от правого


   if(ball->Is_Moving_Up() )
      inner_y = inner_low_y;
   else
      inner_y = inner_top_y;
   
   
   if (Hit_Circle_On_Line(next_y_pos - inner_y, next_x_pos, inner_left_x, inner_right_x, ball->Radius, reflection_pos) ) // отрадежение от внутренней части платформы
   {
      ball->Reflect(true);
      goto _on_hit;
   }

   return false;

_on_hit:
   if(ball->Get_State() == EBS_On_Paraschute)
      ball->Set_State(EBS_Off_Paraschute);

   if(Platform_State == EPlatform_State::Glue && Platform_State.Glue == EPlatform_Transformation::Active)
   {
      ball->Get_Center(ball_x, ball_y);
      ball->Set_State(EBS_On_Platform, ball_x, ball_y);
   }
   return true;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act()
{
   EPlatform_State next_state = EPlatform_State::Unknown;
   bool is_correct;
   switch (Platform_State)
   {
      case EPlatform_State::Rolling:
         Act_For_Rolling_State();
         break;
      
      case EPlatform_State::Glue:
         if(Platform_Glue.Act(Platform_State.Glue, Ball_Set, next_state) )
             Redraw_Platform();
      
         if(next_state != EPlatform_State::Unknown)
            Set_State(next_state);

         break;

      case EPlatform_State::Meltdown:
         Act_For_Meltdown_State();
         break;

      case EPlatform_State::Expanding:
         if(Platform_Expanding.Act_For_Expanding_State(X_Pos, is_correct, next_state) )
             Redraw_Platform();

         if(is_correct)
            Correct_Platform_Pos();

         if(next_state != EPlatform_State::Unknown)
            Set_State(next_state);

         break;
      case EPlatform_State::Laser:
         if(Platform_Laser.Act_For_Laser_State(next_state, X_Pos) )
            Redraw_Platform();

         if(next_state != EPlatform_State::Unknown)
            Set_State(next_state); // устанока нового состояния 

         break;
   }
}
bool AsPlatform::Is_Finished()
{
   return false;
}
void AsPlatform::Draw(HDC hdc, RECT &paint_area)
{
   // Рисуем платформу

   RECT intersection_rect;

   if (! IntersectRect(&intersection_rect, &paint_area, &Platform_Rect) )
      return;

   switch (Platform_State)
   {
   case EPlatform_State::Regular:
      if(Has_Substate_Regular( EPlatform_Substate_Regular::Ready) || Has_Substate_Regular(EPlatform_Substate_Regular::Normal) )
         Draw_Normal_State(hdc, paint_area);
      break;

   case EPlatform_State::Meltdown:
      if(Platform_State.Meltdown == EPlatform_Substate_Meltdown::Active)
         Draw_Meltdown_State(hdc, paint_area);
      break;
   
   case EPlatform_State::Rolling:
      switch(Platform_State.Rolling)
      {
      
      case EPlatform_Substate_Rolling::Roll_In:
         Draw_Roll_In_State(hdc, paint_area);
         break;

      case EPlatform_Substate_Rolling::Expand_Roll_In:
         Draw_Normal_State(hdc, paint_area); // отрисовка нормального состояния, но с постоянным смещением по оси Х в методе Act()
         break;
      }
      break;

   case EPlatform_State::Glue:
      Draw_Normal_State(hdc, paint_area);
      Platform_Glue.Draw_Glue_State(hdc, X_Pos);
      break;

   case EPlatform_State::Expanding:
      Platform_Expanding.Draw_Expanding_State(hdc, X_Pos);
      break;

   case EPlatform_State::Laser:
      Platform_Laser.Draw_Laser_State(hdc, X_Pos, Platform_Rect);
      break;
   }
}
void AsPlatform::Clear(HDC hdc, RECT &paint_area)
{
   switch (Platform_State)
   {
   case EPlatform_State::Regular:
      if(! ( Has_Substate_Regular(EPlatform_Substate_Regular::Ready) || Has_Substate_Regular(EPlatform_Substate_Regular::Normal) ))
         break;
      // else - no break
   case EPlatform_State::Rolling:
   case EPlatform_State::Glue:
   case EPlatform_State::Expanding:
   case EPlatform_State::Laser:
      Clear_BG(hdc, paint_area);
      break;
   }
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Init(AsBall_Set *ball_set, AsLaser_Beam_Set *laser_beam_set)
{
   Ball_Set = ball_set;
   Laser_Beam_Set = laser_beam_set;

   Platform_Expanding.Init(Platform_Inner_Color, Platform_Circle_Color, Highlight_Color);
   Platform_Laser.Init(laser_beam_set,Platform_Inner_Color, Platform_Circle_Color, Highlight_Color);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::On_Space_Key(bool key_down)
{
  
   if(Has_Substate_Regular(EPlatform_Substate_Regular::Ready) )
   {
      if(key_down)
      {
         Ball_Set->Release_From_Platform(Get_Middle_Pos());
         Platform_State.Set_State(EPlatform_Substate_Regular::Normal);
      }
   }
   else if(Platform_State == EPlatform_State::Glue)
   {
      if(key_down)
      Ball_Set->Release_Next_Ball();
   }

   else if (Platform_State == EPlatform_State::Laser)
         Platform_Laser.Fire(key_down, X_Pos);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Advance(double max_speed)
{
   double step;

   if(Platform_State.Moving == EPlatform_Moving_State::Stopping || Platform_State.Moving == EPlatform_Moving_State::Stop)
      return;

   step = Speed / max_speed * AsConfig::Moving_Size_Step;
   X_Pos += step;

   if(Correct_Platform_Pos())
   {
      Speed = 0.0;	
      Platform_State.Moving = EPlatform_Moving_State::Stopping;
   }
   

   // cмещаем приклеенные мячики
   if( (Platform_State == EPlatform_State::Glue && Platform_State.Glue == EPlatform_Transformation::Active) || Has_Substate_Regular(EPlatform_Substate_Regular::Ready) )
   {
      if(Platform_State.Moving == EPlatform_Moving_State::Left)
         Ball_Set->On_Platform_Advance(M_PI, fabs(Speed), max_speed);
      else
         if(Platform_State.Moving == EPlatform_Moving_State::Right)
            Ball_Set->On_Platform_Advance(0, fabs(Speed), max_speed);
   }
}

//------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Speed()
{
   return Speed;
}
//------------------------------------------------------------------------------------------------------------

void AsPlatform::Start_Movement()
{
   Prev_Platform_Rect = Platform_Rect;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::End_Movement()
{
   if(Platform_State.Moving == EPlatform_Moving_State::Stop)
      return;

   Redraw_Platform();

   if(Platform_State.Moving == EPlatform_Moving_State::Stopping)
      Platform_State.Moving = EPlatform_Moving_State::Stop;
}
//------------------------------------------------------------------------------------------------------------
EPlatform_State AsPlatform::Get_State()
{
   return Platform_State;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Set_State(EPlatform_State new_state)
{
   int i, len;

   if (Platform_State == new_state)
      return;

   switch (new_state)
   {
   case EPlatform_State::Regular:
      AsConfig::Throw(); // это состояние будем устанавливать в другом Set_State()

   case EPlatform_State::Meltdown:
      if(Platform_State != EPlatform_State::Regular)
      {
         Platform_State.Set_Next_State(new_state);
         return;
      }
      if(Platform_State.Meltdown == EPlatform_Substate_Meltdown::Unknown)
         Platform_State.Meltdown = EPlatform_Substate_Meltdown::Init;

      Speed = 0.0;
      len = sizeof(Meltdown_Platform_Y_Pos) / sizeof(Meltdown_Platform_Y_Pos[0]);

      for (i = 0; i < len; i++)
         Meltdown_Platform_Y_Pos[i] = Platform_Rect.top;

      break; 

   case EPlatform_State::Rolling:
      if(Platform_State.Rolling != EPlatform_Substate_Rolling::Unknown)
         return;

      Platform_State.Rolling = EPlatform_Substate_Rolling::Roll_In;

      X_Pos = AsConfig::Max_X_Pos - 1;
      Rolling_Step = Max_Rolling_Step - 1;
      break;


   case EPlatform_State::Glue:
      if(! Set_Transformation_State(new_state, Platform_State.Glue))
         return;

      Platform_Glue.Reset();

      break;
      
   case EPlatform_State::Expanding:
      if(! Set_Transformation_State(new_state, Platform_State.Expanding))
         return;

     Platform_Expanding.Reset();
      
      break;

   case EPlatform_State::Laser:
      if(! Set_Transformation_State(new_state, Platform_State.Laser))
         return;

     Platform_Laser.Reset();
      
      break;
   }
   
   Platform_State = new_state;
}
void AsPlatform::Set_State(EPlatform_Substate_Regular new_regular_state)
{
   EPlatform_State next_state;   

   next_state = Platform_State.Set_State(new_regular_state);

   if(next_state != EPlatform_State::Unknown)
      Set_State(next_state);
}

bool AsPlatform::Set_Transformation_State(EPlatform_State new_state, EPlatform_Transformation &transformation_state)
{
   if(Platform_State != EPlatform_State::Regular)
   {
      Platform_State.Set_Next_State(new_state);
      return false;
   }

   if(transformation_state == EPlatform_Transformation::Finalize)
      return false;

   else
   {
      transformation_state = EPlatform_Transformation::Init;
      return true;
   }
}


//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Has_Substate_Regular(EPlatform_Substate_Regular new_state)
{
   if(Platform_State.Regular == new_state)
      return true;
   else 
      return false;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Redraw_Platform()
{
   double platform_width;

   if(Last_Timer_Tick != AsConfig::Current_Timer_Tick)
   {
      Prev_Platform_Rect = Platform_Rect;
      Last_Timer_Tick = AsConfig::Current_Timer_Tick;
   }

      

   platform_width = Get_Platform_Width();

   Platform_Rect.left = (int) ( X_Pos * AsConfig::D_Global_Scale);
   Platform_Rect.top = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
   Platform_Rect.right = Platform_Rect.left +(int) ( platform_width * AsConfig::D_Global_Scale);
   Platform_Rect.bottom = Platform_Rect.top + AsConfig::Platform_Height* AsConfig::Global_Scale;

   if (Platform_State == EPlatform_State::Meltdown)
      Prev_Platform_Rect.bottom = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;
   

   AsConfig::Invalidate_Rect(Prev_Platform_Rect);
   AsConfig::Invalidate_Rect(Platform_Rect);
}
//------------------------------------------------------------------------------------------------------------

 void AsPlatform::Move(bool is_left, bool key_down)
 {
    if(! (Has_Substate_Regular(EPlatform_Substate_Regular::Normal) || Platform_State == EPlatform_State::Glue || Platform_State == EPlatform_State::Laser))
       return;
      
    if(is_left)
       Left_Key_Dowm = key_down;
    else
       Right_Key_Down = key_down;

    if(Left_Key_Dowm && Right_Key_Down)
       return;
    
    if(! Left_Key_Dowm && ! Right_Key_Down)
    {
       Speed = 0.0;
       Platform_State.Moving = EPlatform_Moving_State::Stopping;
       return;
    }

    if(Left_Key_Dowm)
    {
       Platform_State.Moving = EPlatform_Moving_State::Left;
       Speed = -X_Step;
    }

    if(Right_Key_Down)
    {
      Platform_State.Moving = EPlatform_Moving_State::Right;
      Speed = X_Step;
    }
 }
 
 //------------------------------------------------------------------------------------------------------------
 bool AsPlatform::HitBy(AFalling_Letter *falling_letter)
 {
   RECT intersection_rect, falling_letter_rect;

   falling_letter->Get_Letter_Cell(falling_letter_rect);

   if (IntersectRect(&intersection_rect, &falling_letter_rect, &Platform_Rect))
      return true;
   else
      return false;
 }
 //------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Middle_Pos()
{
   return (double) (X_Pos + (double)AsConfig::Platform_Normal_Width/ 2.0);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act_For_Rolling_State()
{
   switch(Platform_State.Rolling)
   {
      case EPlatform_Substate_Rolling::Roll_In:
         ++Rolling_Step;

         if (Rolling_Step >= Max_Rolling_Step)
            Rolling_Step -= Max_Rolling_Step;

         X_Pos -= Rolling_Platform_Speed;

         if (X_Pos <= Roll_In_Platform_End_X_Pos)
         {
            X_Pos = Roll_In_Platform_End_X_Pos;
            Platform_State.Rolling = EPlatform_Substate_Rolling::Expand_Roll_In;
            Inner_Width = 1;
         }
         break;

      case EPlatform_Substate_Rolling::Expand_Roll_In:
         --X_Pos;
         Inner_Width += 2;

         if (Inner_Width >= AsConfig::Platform_Normal_Inner_Width)
         {
            Inner_Width = AsConfig::Platform_Normal_Inner_Width;
            Set_State(EPlatform_Substate_Regular::Ready);
            Platform_State.Rolling = EPlatform_Substate_Rolling::Unknown;
      
         }
         break;
   }

   Redraw_Platform();

}

void AsPlatform::Act_For_Meltdown_State()
{
   switch(Platform_State.Meltdown)
   {
      case EPlatform_Substate_Meltdown::Init:
         Platform_State.Meltdown = EPlatform_Substate_Meltdown::Active;
         break;

      case EPlatform_Substate_Meltdown::Active:
         Redraw_Platform();
         break;
   }
}


bool AsPlatform::Correct_Platform_Pos()
{// проверка на выходы за пределы рамки и корректировка, если таковая потребуется
   double max_platform_x, min_platform_x;

   min_platform_x = AsConfig::Border_X_Offset + 1;

   if(X_Pos <= min_platform_x)
   {
      X_Pos = min_platform_x;
      return true;
   }

   max_platform_x = AsConfig::Max_X_Pos - Get_Platform_Width();

   if(X_Pos >= max_platform_x)
   {
      X_Pos = max_platform_x;
      return true;
   }

   return false;
}
void AsPlatform::Clear_BG(HDC hdc, RECT &paint_area)
{// Очищаем фоном прежнее место
   RECT intersection_rect;

   if ( !(IntersectRect(&intersection_rect, &paint_area, &Prev_Platform_Rect)) )
      return;

   AsConfig::BG_Color.Select(hdc);
   Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);
}
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Normal_State(HDC hdc, RECT &paint_area)
{// Рисуем платформу в нормальном состоянии
   
   double x = X_Pos;
   int y = AsConfig::Platform_Y_Pos;
   const double d_scale = AsConfig::D_Global_Scale;
   const int scale = AsConfig::Global_Scale;
   RECT inner_rect = {};
   //Clear_BG(hdc);

   // 1. Рисуем боковые шарики
   AsPlatform::Platform_Circle_Color.Select(hdc);

   Ellipse(hdc, (int)(x * d_scale), y * scale, (int)((x + (double)AsConfig::Platform_Circle_Size) * d_scale - 1), (y + AsConfig::Platform_Circle_Size) * scale - 1);
   Ellipse(hdc,(int)( (x + (double)Inner_Width) * d_scale), y * scale, (int)((x + (double)AsConfig::Platform_Circle_Size + (double)Inner_Width) * d_scale - 1), (y + AsConfig::Platform_Circle_Size) * scale - 1);

   // 2. Рисуем блик
   Platform_Expanding.Draw_Circle_Highlight(hdc, x * d_scale, y * scale);

   // 3. Рисуем среднюю часть
   AsPlatform::Platform_Inner_Color.Select(hdc);

   inner_rect.left = (x + 4) * d_scale;
   inner_rect.top = (y + 1) * scale;
   inner_rect.right = (x + 4 + Inner_Width - 1) * d_scale;
   inner_rect.bottom = (y + 1 + 5) * scale;

   AsConfig::Round_Rect(hdc, inner_rect, 3);

   
   if(Normal_Platform_Image == 0 && Has_Substate_Regular(EPlatform_Substate_Regular::Ready) )
      Get_Normal_Platform_Image(hdc);
   
}


//------------------------------------------------------------------------------------------------------------
void AsPlatform::Get_Normal_Platform_Image(HDC hdc)
{
   int i,j;
   int x, y;
   int offset = 0;

   x = (int) (X_Pos * AsConfig::D_Global_Scale);
   y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;

   Normal_Platform_Image_Width = AsConfig::Platform_Normal_Width * AsConfig::Global_Scale;
   Normal_Platform_Image_Height = AsConfig::Platform_Height * AsConfig::Global_Scale;

   Normal_Platform_Image = new int[Normal_Platform_Image_Width * Normal_Platform_Image_Height];

   for(i = 0; i < Normal_Platform_Image_Height; ++i)
      for(j = 0; j < Normal_Platform_Image_Width; ++j)
         Normal_Platform_Image[offset++] = GetPixel(hdc, x + j, y + i );
   
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Meltdown_State(HDC hdc, RECT &paint_area)
{// Рисуем платформу в расплавленном состоянии

   int i, j;
   int x, y;
   int y_offset;
   int stroke_length = 0;
   int moved_columns_count = 0;
   int max_platform_y;
   AColor color;

   max_platform_y = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;

   for (i = 0; i < Normal_Platform_Image_Width; i++)
   {
      if (Meltdown_Platform_Y_Pos[i] > max_platform_y)
         continue;

      ++moved_columns_count;

      y_offset = AsConfig::Rand(Meltdown_Speed) + 1;
      x = Platform_Rect.left + i;

      j = 0;
      y = Meltdown_Platform_Y_Pos[i];

      MoveToEx(hdc, x, y, 0);

      while(Get_Platform_Image_Stroke_Color(i, j, color, stroke_length))
      {
         color.Select_Pen(hdc);

         LineTo(hdc, x, y + stroke_length);

         y += stroke_length;
         j += stroke_length;
      }

      y = Meltdown_Platform_Y_Pos[i];
      MoveToEx(hdc, x, y, 0);
      AsConfig::BG_Color.Select_Pen(hdc);

      LineTo(hdc, x, y + y_offset);

      Meltdown_Platform_Y_Pos[i] += y_offset;
   }




   if (moved_columns_count == 0)
   {
      Platform_State.Set_State(EPlatform_Substate_Regular::Missing);  // Вся платформа сдвинулась за пределы окна
      Platform_State.Meltdown = EPlatform_Substate_Meltdown::Unknown;
   }
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Roll_In_State(HDC hdc, RECT &paint_area)
{// Рисуем выкатывающуюся платформу

   int x = (int) (X_Pos * AsConfig::D_Global_Scale);
   int y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
   int roller_size = AsConfig::Platform_Circle_Size * AsConfig::Global_Scale;
   double alpha;
   XFORM xform = {}, old_xform = {};

   //Clear_BG(hdc);

   // 1. Шарик
   AsPlatform::Platform_Circle_Color.Select(hdc);
   Ellipse(hdc, x, y, x + roller_size - 1, y + roller_size - 1);


   // 2. Разделительная линия

   alpha = -2.0 * M_PI / (double)Max_Rolling_Step * (double)Rolling_Step;

   xform.eM11 = (float)cos(alpha);
   xform.eM12 = (float)sin(alpha);
   xform.eM21 = (float)-sin(alpha);
   xform.eM22 = (float)cos(alpha);
   xform.eDx = (float)(x + roller_size / 2);
   xform.eDy = (float)(y + roller_size / 2);
   GetWorldTransform(hdc, &old_xform);
   SetWorldTransform(hdc, &xform);

   AsConfig::BG_Color.Select(hdc);

   Rectangle(hdc, -AsConfig::Global_Scale / 2, -roller_size / 2, AsConfig::Global_Scale / 2, roller_size / 2);

   SetWorldTransform(hdc, &old_xform);


   // 3. Блик
   Platform_Expanding.Draw_Circle_Highlight(hdc, x, y);

   
}
//------------------------------------------------------------------------------------------------------------
bool AsPlatform::Reflect_On_Circle(double next_x_pos, double next_y_pos, double platform_ball_x_offset, ABall *ball)
{
   double dx, dy;
   double distance;
   double two_radiuses;
   double platform_ball_radius,platform_ball_x,platform_ball_y;
   double beta, alpha, gamma;
   double related_ball_direction;
   double const pi_2 = 2.0 * M_PI;

   platform_ball_radius = (double)AsConfig::Platform_Circle_Size / 2.0;
   platform_ball_x =  X_Pos + platform_ball_radius + platform_ball_x_offset;
   platform_ball_y = (double)(AsConfig::Platform_Y_Pos + platform_ball_radius);
   
   dx = next_x_pos - platform_ball_x;
   dy = next_y_pos - platform_ball_y;

   distance = sqrt(dx * dx + dy * dy);
   two_radiuses = ball->Radius + platform_ball_radius;

   if(fabs(distance - two_radiuses < AsConfig::Moving_Size_Step))
   {// Мячик коснулся шарика
      beta = atan2(-dy, dx);

      related_ball_direction = ball->Get_Direction();
      related_ball_direction -= beta;

      if(related_ball_direction > pi_2)
         related_ball_direction -= pi_2;

      if(related_ball_direction < 0)
         related_ball_direction += pi_2;

      
      if(related_ball_direction > M_PI_2 && related_ball_direction < M_PI + M_PI_2)
      {
         alpha = M_PI + beta - ball->Get_Direction();
         gamma = beta + alpha;
         
         ball->Set_Direction(gamma);

         return true;
      }
   }
   return false;
}
//------------------------------------------------------------------------------------------------------------

bool AsPlatform::Get_Platform_Image_Stroke_Color(int x, int y, AColor &color, int &stroke_length)
{
   int i;
   int offset = y * Normal_Platform_Image_Width + x;
   int image_color = 0;

   stroke_length = 0;
   
   if(y >= Normal_Platform_Image_Height)
      return false;

   for(i = y; i < Normal_Platform_Image_Height; ++i)
   {
      if(i == y)
      {
         image_color = Normal_Platform_Image[offset];
         ++stroke_length;
      }
      else
      {
         if(Normal_Platform_Image[offset] == image_color)
         {
            ++stroke_length;
         }
         else
            break;
      }

      offset += Normal_Platform_Image_Width;
   }

   if(image_color == Highlight_Color.Get_RGB())
      color.Set_Pen(AsPlatform::Highlight_Color.Get_Pen()); 

   else if(image_color == Platform_Circle_Color.Get_RGB())
      color.Set_Pen(AsPlatform::Platform_Circle_Color.Get_Pen()); 
   
   else if(image_color == Platform_Inner_Color.Get_RGB())
      color.Set_Pen(AsPlatform::Platform_Inner_Color.Get_Pen()); 

   else if(image_color == AsConfig::BG_Color.Get_RGB())
       color.Set_Pen(AsConfig::BG_Color.Get_Pen()); 
      
   return true;
}

double AsPlatform::Get_Platform_Width()
{
   if (Platform_State == EPlatform_State::Rolling && Platform_State.Rolling == EPlatform_Substate_Rolling::Roll_In)
      return (double)AsConfig::Platform_Circle_Size;

   else if (Platform_State == EPlatform_State::Expanding)
      return Platform_Expanding.Expanding_Platform_Width;

   else
      return (double)AsConfig::Platform_Normal_Width;
}
//------------------------------------------------------------------------------------------------------------
