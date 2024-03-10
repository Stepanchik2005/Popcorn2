#include "Ball.h"
//AGraphics_Objects
AGraphics_Objects::~AGraphics_Objects()
{
}




// AMover
AMover::~AMover()
{
}



// ABall
const double ABall::Start_Ball_Y_Pos = 181.0;
const double ABall::Radius = 2.0 - 0.5 / AsConfig::Global_Scale;
int ABall::Hit_Checkers_Count = 0;
AHit_Checker *ABall::Hit_Checkers[] = {};
//------------------------------------------------------------------------------------------------------------
ABall::ABall()
: Ball_State(EBS_Disabled),Prev_Ball_State(EBS_Disabled), Center_X_Pos(0.0), Center_Y_Pos(Start_Ball_Y_Pos), Prev_Ball_Speed(0.0), 
  Ball_Speed(0.0), Prev_Ball_Direction(0),  Ball_Direction(0), Testing_Is_Active(false), Test_Iteration(0), Ball_Rect{}, Prev_Ball_Rect{}, Release_Timer_Tick(0)
{
	//Set_State(EBS_Normal, 0);
}
//------------------------------------------------------------------------------------------------------------
bool AHit_Checker::Hit_Circle_On_Line(double y, double next_x_pos, double left_x, double right_x, double radius, double &x)
{// Проверяет пересечение горизонтального отрезка (проходящего от left_x до right_x через y) с окружностью радиусом radius

	double min_x, max_x;

	// x * x + y * y = R * R
	// x = sqrt(R * R - y * y)
	// y = sqrt(R * R - x * x)

	if (y > radius)
		return false;

	x = sqrt(radius * radius - y * y);

	max_x = next_x_pos + x;
	min_x = next_x_pos - x;

	if (max_x >= left_x && max_x <= right_x  ||  min_x >= left_x && min_x <= right_x)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Advance(double max_speed)
{
	int i;
	int prev_hits_count = 0;
	const int max_hits_count = 8;
	bool got_hit;
	double next_x_pos, next_y_pos;
	double next_step = Ball_Speed / max_speed * AsConfig::Moving_Size_Step;
	if(Ball_State == EBS_Disabled)
		return;

	if (Ball_State == EBS_Lost || Ball_State == EBS_On_Platform || Ball_State == EBS_Teleporting)
		return;

		got_hit = false;

		next_x_pos = Center_X_Pos + next_step * cos(Ball_Direction);
		next_y_pos = Center_Y_Pos - next_step * sin(Ball_Direction);

		// Корректируем позицию при отражении:
		for (i = 0; i < Hit_Checkers_Count; i++)
			got_hit |= Hit_Checkers[i]->Check_Hit(next_x_pos, next_y_pos, this); 

		if (got_hit)
		{
			++prev_hits_count;

			if(prev_hits_count >= max_hits_count)
			{
				Ball_Direction += M_PI / 8.0;
				prev_hits_count = 0;
			}

		}
		else

		{
			// Мячик продолжит движение, если не взаимодействовал с другими объектами
			//Rest_Distance -= AsConfig::Moving_Size_Step;

			Center_X_Pos = next_x_pos;
			Center_Y_Pos = next_y_pos;

			if (Testing_Is_Active)
				Rest_Test_Distance -= AsConfig::Moving_Size_Step;
		}
}
//------------------------------------------------------------------------------------------------------------
double ABall::Get_Speed()
{
	return Ball_Speed;
}
//------------------------------------------------------------------------------------------------------------

void ABall::Start_Movement()
{
	Prev_Ball_Rect = Ball_Rect;
}
//------------------------------------------------------------------------------------------------------------

void ABall::End_Movement()
{
	if(Ball_State == EBS_Disabled || Ball_State == EBS_Lost)
		return;

	Redraw_Ball();

	if(Ball_State == EBS_On_Paraschute)
	{
		Prev_Paraschute_Rect = Paraschute_Rect;

		Paraschute_Rect.bottom = Ball_Rect.bottom;
		Paraschute_Rect.top = Paraschute_Rect.bottom - Paraschute_Size * AsConfig::Global_Scale;
		
	   Redraw_Paraschute();
	}
}
//------------------------------------------------------------------------------------------------------------
void ABall::Act()
{

}
bool ABall::Is_Finished()
{
	return false;
}
void ABall::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(Ball_State == EBS_Disabled)
		return;

	if( (Ball_State == EBS_Teleporting || Ball_State == EBS_Lost) && Prev_Ball_State == Ball_State)
		return;

	switch (Ball_State)
	{
	case EBS_Disabled:
		return;
	case EBS_Normal: // сразу переходим к рисованию, т.к. доп. условий для этих состояний не требуется
	case EBS_On_Platform:
		break;

	  case EBS_On_Paraschute:
		 Draw_Paraschute(hdc, paint_area);
		 break;

	  case EBS_Lost:
		  if(Prev_Ball_State == EBS_On_Paraschute)
			   Clean_Up_Paraschute(hdc);
		  return;

	  case EBS_Off_Paraschute:
		  Clean_Up_Paraschute(hdc);
		  Set_State(EBS_Normal, Center_X_Pos, Center_Y_Pos);
		  break;
	  
     case EBS_Teleporting: // анимация телепорта отрисовывается отдельным методом
		  return;

	  default:
		  AsConfig::Throw();
	  
	}

	// 2. Рисуем шарик
	if (IntersectRect(&intersection_rect, &paint_area, &Ball_Rect) )
	{
		AsConfig::White_Color.Select(hdc);

		Ellipse(hdc, Ball_Rect.left, Ball_Rect.top, Ball_Rect.right - 1, Ball_Rect.bottom - 1);
	}
}
void ABall::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(Ball_State == EBS_Disabled)
		return;

	if( (Ball_State == EBS_Teleporting || Ball_State == EBS_Lost) && Prev_Ball_State == Ball_State)
		return;

	// 1. Очищаем фон
	if (IntersectRect(&intersection_rect, &paint_area, &Prev_Ball_Rect) )
	{
      AsConfig::BG_Color.Select(hdc);

		Ellipse(hdc, Prev_Ball_Rect.left, Prev_Ball_Rect.top, Prev_Ball_Rect.right - 1, Prev_Ball_Rect.bottom - 1);
	}
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_Speed(double ball_speed)
{
	Ball_Speed = ball_speed;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_For_Test()
{
	Testing_Is_Active = true;
	Rest_Test_Distance = 30.0;

	Set_State(EBS_Normal, 80, 189 - Test_Iteration);
	Ball_Direction = M_PI_4 / 4.0;

	++Test_Iteration;
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Test_Finished()
{
	if (Testing_Is_Active)
	{
		if (Rest_Test_Distance <= 0.0)
		{
			Testing_Is_Active = false;
			Set_State(EBS_Lost);
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
EBall_State ABall::Get_State()
{
	return Ball_State;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_State(EBall_State new_state, double x_pos, double y_pos)
{
	switch (new_state)
	{
	case EBS_Disabled:
		Ball_Speed = 0.0;
		break;

	case EBS_Normal:
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = AsConfig::Initial_Ball_Speed;
		Ball_Direction = M_PI_4;
		Redraw_Ball();
		break;


	case EBS_Lost:
		if(! (Ball_State == EBS_Normal || Ball_State == EBS_On_Paraschute))
			AsConfig::Throw();// только из EBS_Normal или EBS_On_Paraschute можно установить EBS_Lost

		Redraw_Ball();

		if(Ball_State == EBS_On_Paraschute)
			Redraw_Paraschute();

		Ball_Speed = 0.0;
		break;

	case EBS_On_Platform:
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Prev_Ball_Speed = Ball_Speed;
		Ball_Speed = 0.0;
		Prev_Ball_Direction = Ball_Direction;
		//Ball_Direction = M_PI_4;
		Release_Timer_Tick = AsConfig::Current_Timer_Tick + On_Platform_Timeout;
		Redraw_Ball();
		break;

	case EBS_On_Paraschute:
		AsConfig::Throw(); // для установки этого состояния юзается метод Set_On_Paraschute()
		break;

	case EBS_Off_Paraschute:
		if(Ball_State != EBS_On_Paraschute)
		   AsConfig::Throw(); // только из EBS_Off_Paraschute  можно установить EBS_On_Paraschute
			
		Ball_Speed = 0.0;
		Redraw_Ball();
		Redraw_Paraschute();
		break;
   
	case EBS_Teleporting:
		if( !(Ball_State == EBS_Normal || Ball_State == EBS_On_Paraschute || Ball_State == EBS_Teleporting))
			AsConfig::Throw();
		Center_X_Pos = x_pos;
		Center_Y_Pos = y_pos;
		Ball_Speed = 0.0;
		Redraw_Ball();

		if(Ball_State == EBS_On_Paraschute)
			Redraw_Paraschute();

		break;

	default:
		  AsConfig::Throw();
	}
	Prev_Ball_State = Ball_State;
	Ball_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
double ABall::Get_Direction()
{
	return Ball_Direction;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Get_Center(double &x_pos, double &y_pos)
{
	x_pos = Center_X_Pos;
	y_pos = Center_Y_Pos;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_Direction(double new_direction)
{
	const double pi_2 = 2.0 * M_PI;

	while (new_direction > pi_2)
		new_direction -= pi_2;

	while (new_direction < 0.0)
		new_direction += pi_2;

	Ball_Direction = new_direction;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Forced_Advance(double direction, double speed, double max_speed)
{
	EBall_State prev_ball_state;
	double prev_direction, prev_speed;
	
	prev_ball_state = Ball_State;
	prev_direction = Ball_Direction;
	prev_speed = Ball_Speed;
	
	Ball_State = EBS_Normal;
	Ball_Speed = speed;
	Ball_Direction = direction;

	Advance(max_speed);

	Ball_Speed = prev_speed;
	Ball_Direction = prev_direction;
	Ball_State = prev_ball_state;
}
void ABall::Release()
{// продолжить прерванное движение мячика
	Set_State(EBS_Normal, Center_X_Pos, Center_Y_Pos);
	Ball_Speed = Prev_Ball_Speed;

	if(Ball_Speed < AsConfig::Initial_Ball_Speed)
		Ball_Speed = AsConfig::Initial_Ball_Speed;

	Ball_Direction = Prev_Ball_Direction;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Reflect(bool from_horizontal)
{
	if (from_horizontal)
		Set_Direction(-Ball_Direction);
	else
		Set_Direction(M_PI - Ball_Direction);
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Moving_Up()
{
	if (Ball_Direction >= 0.0 && Ball_Direction < M_PI)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
bool ABall::Is_Moving_Left()
{
	if(Ball_Direction > M_PI_2 && Ball_Direction < M_PI + M_PI_2)
		return true;
	else
		return false;
}
//------------------------------------------------------------------------------------------------------------
void ABall::Set_On_Paraschute(int brick_x, int brick_y)
{
	const int scale = AsConfig::Global_Scale;
	int cell_x = AsConfig::Level_X_Offset + brick_x * AsConfig::Cell_Width;
	int cell_y = AsConfig::Level_Y_Offset + brick_y * AsConfig::Cell_Height;
	Ball_State = EBS_On_Paraschute;
	Ball_Direction = M_PI + M_PI_2;
	Ball_Speed = 1.0;

	Paraschute_Rect.left = cell_x * scale;
	Paraschute_Rect.top = cell_y * scale;
	Paraschute_Rect.right = Paraschute_Rect.left + Paraschute_Size * scale;
	Paraschute_Rect.bottom = Paraschute_Rect.top + Paraschute_Size * scale;

	Prev_Paraschute_Rect = Paraschute_Rect;

	Center_X_Pos =  (double)cell_x + ((double)AsConfig::Cell_Width / 2);
   Center_Y_Pos = (double) (cell_y + Paraschute_Size) - Radius * 2.0;

   Redraw_Paraschute();
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw_Teleporting(HDC hdc, int step)
{
	const int scale = AsConfig::Global_Scale;

	int top_y = (int)(Ball_Rect.top + step * 1.7);
	int low_y = (int)(Ball_Rect.bottom - step * 1.7);

	if(top_y > low_y)
		return;

	AsConfig::White_Color.Select(hdc);
	Ellipse(hdc, Ball_Rect.left, top_y, Ball_Rect.right, low_y);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Add_Hit_Checker(AHit_Checker *hit_checker)
{
	if (Hit_Checkers_Count >= sizeof(Hit_Checkers) / sizeof(Hit_Checkers[0]) )
		return;

	Hit_Checkers[Hit_Checkers_Count++] = hit_checker;
}
//------------------------------------------------------------------------------------------------------------

void ABall::Redraw_Ball()
{
	Ball_Rect.left = (int)((Center_X_Pos - Radius) * AsConfig::Global_Scale);
	Ball_Rect.top = (int)((Center_Y_Pos - Radius) * AsConfig::Global_Scale);
	Ball_Rect.right = (int)((Center_X_Pos + Radius) * AsConfig::Global_Scale);
	Ball_Rect.bottom = (int)((Center_Y_Pos + Radius) * AsConfig::Global_Scale);

	InvalidateRect(AsConfig::Hwnd, &Prev_Ball_Rect, FALSE);
	InvalidateRect(AsConfig::Hwnd, &Ball_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------
void ABall::Draw_Paraschute(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	RECT intersection_rect, sub_arc = {}, other_arc = {};
	int sub_arc_height = 2 * AsConfig::Global_Scale;
	int dome_height = (Paraschute_Rect.bottom - Paraschute_Rect.top) / 2;
	int arc_x = Paraschute_Rect.left + 1;
	int line_y =  Paraschute_Rect.top + dome_height;
	int ball_center;
	
	if (! IntersectRect(&intersection_rect, &paint_area, &Paraschute_Rect) )
	 return;

	// 1. Очистить фон
	Clean_Up_Paraschute(hdc);

	// 2. Купол
	AsConfig::Blue_Color.Select(hdc);
	Chord(hdc, Paraschute_Rect.left, Paraschute_Rect.top, Paraschute_Rect.right - 1, Paraschute_Rect.bottom - 1, Paraschute_Rect.right, Paraschute_Rect.top + dome_height - 1, Paraschute_Rect.left, Paraschute_Rect.top + dome_height - 1);

	// 3. Арки
	AsConfig::BG_Color.Select(hdc);

	sub_arc.left = arc_x;
	sub_arc.top = Paraschute_Rect.top +  (dome_height - sub_arc_height);
   sub_arc.right = sub_arc.left + 3 * scale ;
	sub_arc.bottom = sub_arc.top + 4 * scale;

	// 3.1 Левая
	Ellipse(hdc, sub_arc.left, sub_arc.top, sub_arc.right - 1, sub_arc.bottom - 1);

	other_arc = sub_arc;

	other_arc.left =   arc_x + 3 * scale + 1;
   other_arc.right =  arc_x + 11 * scale - 1;

	// 3.2 Центральная
	Ellipse(hdc, other_arc.left, other_arc.top, other_arc.right - 1, other_arc.bottom - 1);

	other_arc = sub_arc;

	other_arc.left =  arc_x + 11 * scale;
   other_arc.right = arc_x + 14 * scale + 1;

	// 3.3 Правая
	Ellipse(hdc, other_arc.left, other_arc.top, other_arc.right - 1, other_arc.bottom - 1);

	// 3. Стропы
   ball_center =  (Paraschute_Rect.left + Paraschute_Rect.right) / 2;

	AsConfig::White_Color.Select_Pen(hdc);

	MoveToEx(hdc, Paraschute_Rect.left, line_y, 0);
	LineTo(hdc, ball_center, Paraschute_Rect.bottom - 2 * scale);

	MoveToEx(hdc, Paraschute_Rect.left + 3 * scale + 1, line_y, 0);
	LineTo(hdc, ball_center, Paraschute_Rect.bottom - 2 * scale);

	MoveToEx(hdc, Paraschute_Rect.right - 4 * scale, line_y, 0);
   LineTo(hdc, ball_center, Paraschute_Rect.bottom - 2 * scale);

	MoveToEx(hdc, Paraschute_Rect.right, line_y - 1, 0);
   LineTo(hdc, ball_center, Paraschute_Rect.bottom - 2 * scale);
} 
//------------------------------------------------------------------------------------------------------------
void ABall::Redraw_Paraschute()
{
	InvalidateRect(AsConfig::Hwnd, &Prev_Paraschute_Rect, FALSE);
	InvalidateRect(AsConfig::Hwnd, &Paraschute_Rect, FALSE);
}
void ABall::Clean_Up_Paraschute(HDC hdc)
{
	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, Prev_Paraschute_Rect.left, Prev_Paraschute_Rect.top, Prev_Paraschute_Rect.right, Prev_Paraschute_Rect.bottom);
}