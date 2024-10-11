#include "Monster.h"

//AMonster---------------------------------------------------------
AMonster::AMonster()
	: X_Pos(0.0), Y_Pos(0.0), Monster_Rect{},Prev_Monster_Rect{}, Next_Direction_Switch_Tick(AsCommon::Rand(AsConfig::FPS)), 
	  Monster_State(EMonster_State::Idle), Speed(0.0), Prev_Speed(0.0), Need_To_Freeze(false) ,Direction(0.0), Start_Animation(0), Explodive_Balls(10)
{

}
void AMonster::Advance(double max_speed)
{
	int i;
	double step;
	double next_x_pos, next_y_pos;
	double origin_direction;
	bool is_direction_chagned = false;
	RECT monster_rect = {};

	if(Monster_State == EMonster_State::Idle || Monster_State == EMonster_State::Destroying)
		return;

	step = Speed / max_speed * AsConfig::Moving_Size_Step;

	origin_direction = Direction;

	// сменить направление
	for(i = 0; i < 64; ++i)
	{
		next_x_pos = X_Pos + step * cos(Direction);
		next_y_pos = Y_Pos - step * sin(Direction);

		Get_Monster_Rect(next_x_pos, next_y_pos ,monster_rect);

		if(ALevel::Has_Brick_At(monster_rect))
			Direction += M_PI / 32.0;
		else
		{
			is_direction_chagned = true;
			break;
		}
	}

	if(! is_direction_chagned)
	{
		Direction = origin_direction - M_PI;	
		return;
	}


	if(Monster_State == EMonster_State::Alive)
	{
		if(next_x_pos < (double)(AsConfig::Border_X_Offset) )
			next_x_pos = (double)(AsConfig::Border_X_Offset);

		if(next_x_pos + (double)Width > (double)(AsConfig::Max_X_Pos ) )
			next_x_pos = (double)AsConfig::Max_X_Pos - (double)Width;

		if(next_y_pos < (double)AsConfig::Border_Y_Offset)
			next_y_pos = (double)AsConfig::Border_Y_Offset;

		if(next_y_pos + (double)Height > (double)AsConfig::Floor_Y_Pos)
			next_y_pos = (double)AsConfig::Floor_Y_Pos - (double)Height;
	}

	X_Pos = next_x_pos;
	Y_Pos = next_y_pos;

}
double AMonster::Get_Speed()
{
	return Speed;
}
void AMonster::Start_Movement()
{
	Prev_Monster_Rect = Monster_Rect;
}
void AMonster::End_Movement()
{
	if(Monster_State == EMonster_State::Idle || Monster_State == EMonster_State::Destroying)
		return;

	Redraw_Monster();
}
void AMonster::Act()
{
	
	switch(Monster_State)
	{
	case EMonster_State::Idle:
		return;

	case EMonster_State::Emitting:
		Act_For_Alive_State();

		if(Time_Of_Emitting < AsConfig::Current_Timer_Tick)
			Monster_State = EMonster_State::Alive;
		break;

	case EMonster_State::Alive:
		if(Need_To_Freeze)
		{
			Prev_Speed = Speed;
			Speed = 0.0;
			Need_To_Freeze = false;
		}
		Act_For_Alive_State();
		break;

	case EMonster_State::Destroying:
		Act_For_Destroying_State();
		break;

	default:
		AsConfig::Throw();
	}

	On_Change_Direction();

	Redraw_Monster();
}
bool AMonster::Is_Finished()
{
	if(Monster_State == EMonster_State::Idle)
		return true;
	else
		return false;
}
void AMonster::Draw(HDC hdc, RECT &paint_area)
{
	switch(Monster_State)
	{
	case EMonster_State::Idle:
		break;

	case EMonster_State::Emitting:
		Draw_For_Alive_State(hdc, paint_area);
		break;

	case EMonster_State::Alive:
		Draw_For_Alive_State(hdc, paint_area);
		break;

	case EMonster_State::Destroying:
		Draw_For_Destroying_State(hdc, paint_area);
		break;

	default:
		AsConfig::Throw();
	}
}
void AMonster::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Prev_Monster_Rect) )
		return;

	AsCommon::Ellipse(hdc, Prev_Monster_Rect, AsConfig::BG_Color);
	AsCommon::Ellipse(hdc, Monster_Rect, AsConfig::BG_Color);
}
bool AMonster::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{
	if(Monster_State == EMonster_State::Idle || Monster_State == EMonster_State::Destroying)
		return false;

	if(! AsCommon::Reflect_On_Circle(next_x_pos, next_y_pos, X_Pos, Y_Pos, Width, ball) )
		return false;

	Destroy();

	return true;

}
bool AMonster::Check_Hit(double next_x_pos, double next_y_pos)
{
	if(next_x_pos >= X_Pos && next_x_pos <= X_Pos + (double)Width
		&& next_y_pos >= Y_Pos && next_y_pos <= Y_Pos + (double)Height)
	{
		Destroy();
		return true;
	}
  return false;
}
bool AMonster::Check_Hit(RECT& rect)
{
	RECT intersection_rect;

	if(! (Monster_State == EMonster_State::Emitting || Monster_State == EMonster_State::Alive))
		return false;

	if(IntersectRect(&intersection_rect, &rect, &Monster_Rect))
	{
		Destroy();
		return true;
	}

	return false;
}

void AMonster::Destroy()
{
	int rest_size;
	int x_offset, y_offset;
	int max_size, half_size;
	int time_offset;
	bool is_red;
   
	if(Monster_State == EMonster_State::Idle || Monster_State == EMonster_State::Destroying)
		return;

	int half_width = Width * AsConfig::Global_Scale / 2;
	int half_height = Height * AsConfig::Global_Scale / 2;
	int x_pos = (int)X_Pos * AsConfig::Global_Scale + half_width; // координаты центра глаза (эллипса)
	int y_pos = (int)Y_Pos * AsConfig::Global_Scale + half_height;
	half_size = half_width; // radius

	if(half_height < half_size)
		half_size = half_height;

	for(auto &ball : Explodive_Balls)
	{
		//max_size = AsCommon::Rand(20) + 10;
		time_offset = AsCommon::Rand(AsConfig::FPS);

		while(true) // проверка попадает ли точка в круг
		{
			x_offset = AsCommon::Rand(half_width) - half_width / 2;
			y_offset = AsCommon::Rand(half_height ) - half_height / 2;

			if(sqrt(x_offset * x_offset + y_offset * y_offset) < half_size)
				break;
		}

		rest_size = half_size - (int)( sqrt( pow(x_offset, 2) + pow(y_offset, 2)));

		

		max_size = AsCommon::Rand(rest_size / 2) + rest_size / 2;

		if(max_size < AsConfig::Global_Scale)
			max_size = AsConfig::Global_Scale;

		is_red = AsCommon::Rand(2);

		ball.Explode(x_pos + x_offset, y_pos + y_offset, time_offset, max_size * 2, 10, is_red);

	}

	AsInfo_Panel::Update_Score(EHit_Type::Hit_Monster);

	Monster_State = EMonster_State::Destroying;
}

void AMonster::Set_Freeze_State(bool is_freeze)
{
	if(! is_freeze)
		Speed = Prev_Speed; 

	Need_To_Freeze = is_freeze;
}
void AMonster::Activate(double x_pos, double y_pos, bool is_left_gate)
{
	Monster_State = EMonster_State::Emitting;

	Start_Animation = AsConfig::Current_Timer_Tick;
	Speed = (AsCommon::Rand(5) + 2) / 10.0;
	Time_Of_Emitting = AsConfig::Current_Timer_Tick + Emitting_Timeout;

	if(is_left_gate)
	{
		Direction = 0;
		X_Pos = x_pos;
	}
	else
	{
		X_Pos = x_pos - Width + AsConfig::Gate_Width;
		Direction = M_PI;
	}

	Y_Pos = y_pos;
	Redraw_Monster();
}

void AMonster::Act_For_Destroying_State()
{
	int count = 0; 
	for(auto &ball : Explodive_Balls)
	{
		ball.Act();

		if(ball.Explodive_Ball_State == EExplodive_Ball_State::Idle)
			count++;
	}
	
	if(count == Explodive_Balls_Count)
		Monster_State = EMonster_State::Idle;
}
void AMonster::Draw_For_Destroying_State(HDC hdc, RECT &paint_area)
{
	for(auto &ball : Explodive_Balls)
		ball.Draw(hdc, paint_area);
}
void AMonster::Get_Monster_Rect(double x_pos, double y_pos, RECT& monster_rect)
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;

	monster_rect.left = (int)(x_pos * d_scale);
	monster_rect.top = (int)(y_pos * d_scale);
	monster_rect.right = monster_rect.left + Width * scale;
	monster_rect.bottom = monster_rect.top + Height * scale;
}
void AMonster::Redraw_Monster()
{
	Get_Monster_Rect(X_Pos, Y_Pos, Monster_Rect);

	AsCommon::Invalidate_Rect(Prev_Monster_Rect);
	AsCommon::Invalidate_Rect(Monster_Rect);
}
void AMonster::On_Change_Direction()
{
	// рандомная корректировка направления
	if(AsConfig::Current_Timer_Tick > Next_Direction_Switch_Tick && Monster_State == EMonster_State::Alive)
	{
		Next_Direction_Switch_Tick += AsConfig::FPS;

		Direction += (double)(AsCommon::Rand(90) - 45) * M_PI / 180.0;
	}
}



//------------------------------------------------------------------------------------------------------------------
const EEye_State AMonster_Eye::Blick_State[Blick_Count] = {
	EEye_State::Closed, EEye_State::Opening, EEye_State::Staring, EEye_State::Closing,
	EEye_State::Opening, EEye_State::Staring, EEye_State::Closing
};
const double AMonster_Eye::Blick_Tick[Blick_Count] = {0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 0.5};

AMonster_Eye::AMonster_Eye()
	: Cornea_Height(0),  Eye_State(EEye_State::Closed)
{
}
void AMonster_Eye::Act_For_Alive_State()
{
	int number_tick = INT_MIN; // индекс в массиве  Blick_Tick[]
	double curr_tick = 0.0; 
	double ratio;
	int prev_blick_tick = 0, frame_in_curr_tick = 0;

	int anim_time_before_curr_tick = AsConfig::Current_Timer_Tick - Start_Animation;
	// 1. Найдем текущий тик
	Find_Current_Tick(number_tick, curr_tick, prev_blick_tick);

	if(number_tick < 0 || number_tick > Blick_Count)
		AsConfig::Throw();
	else
		Eye_State = Blick_State[number_tick];

	if(prev_blick_tick == 0) // нахождение текущего кадра в текущем тике
		frame_in_curr_tick = anim_time_before_curr_tick;
	else
	   frame_in_curr_tick = anim_time_before_curr_tick % prev_blick_tick;

	// Коэффициент на который умножается Cornea_Height
   ratio = (curr_tick - (double)frame_in_curr_tick) / curr_tick;

	switch(Eye_State)
	{
	case EEye_State::Closed:
		Cornea_Height = 0.0;
		break;
		
	case EEye_State::Opening:
		Cornea_Height = (1.0 - ratio) * (double)Max_Cornea_Height;
		break;

	case EEye_State::Staring:
		Cornea_Height = (double)Max_Cornea_Height;
		break;

	case EEye_State::Closing:
		Cornea_Height = ratio * (double)Max_Cornea_Height;
		break;
	}

  
}
void AMonster_Eye::Draw_For_Alive_State(HDC hdc, RECT &paint_area)
{
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	int cornea_rect_top_offset;
	HRGN region = {};

	RECT intersection_rect = {}, rect = {}, cornea_rect = {};

	if (! IntersectRect(&intersection_rect, &paint_area, &Monster_Rect) )
		return;

	rect = Monster_Rect;

	++rect.right;
	++rect.bottom;

	region = CreateEllipticRgnIndirect(&rect);
	SelectClipRgn(hdc, region);

	// 1.1 Фон
	AsCommon::Ellipse(hdc, rect, AsConfig::Monster_Dark_Red_Color);

	Correct_Drawing_Rect(rect, -2, -3, -2, -3);

	AsCommon::Ellipse(hdc, rect, AsConfig::Red_Color);

	SelectClipRgn(hdc, 0);
   DeleteObject(region);

	//2.2 Роговица
	cornea_rect = Monster_Rect;
	cornea_rect_top_offset = 2 * scale;

	cornea_rect.left += 1 * scale;
	cornea_rect.right -= 1 * scale;

	cornea_rect.top += cornea_rect_top_offset + (int)( ( (double)Max_Cornea_Height / 2.0 - Cornea_Height / 2.0) * d_scale);
	cornea_rect.bottom = cornea_rect.top + (int) (Cornea_Height * d_scale);

	region = CreateEllipticRgnIndirect(&cornea_rect);
	SelectClipRgn(hdc, region);

	AsCommon::Ellipse(hdc, cornea_rect, AsConfig::Cornea_Color);
	
	//2.2 Радужка iris
	Correct_Drawing_Rect(rect, 4, 4, -4, -5);

	AsCommon::Ellipse(hdc, rect, AsConfig::Iris_Color);

	//2.3 Зрачок

	Correct_Drawing_Rect(rect, 7, 6, -7, -7);

	AsCommon::Rect(hdc, rect, AsConfig::BG_Color);

	SelectClipRgn(hdc, 0);
   DeleteObject(region);

	AsConfig::Cornea_Arc_Color.Select_Pen(hdc);

	Arc(hdc, cornea_rect.left,cornea_rect.top, cornea_rect.right - 1, cornea_rect.bottom - 1,0,0,0,0);
}

void AMonster_Eye::Find_Current_Tick(int &number_tick, double &curr_tick, int &prev_blick_tick)
{
	int i;
	double curr_blick_tick = 0;
	int animation_step = (AsConfig::Current_Timer_Tick - Start_Animation);

	// 1. Нахождения текущего диапазона (тика)
	for(i = 0; i < Blick_Count; ++i)
	{
		curr_blick_tick += Blick_Tick[i] * AsConfig::FPS; // получаем текущий тик в кадрах

		if(animation_step < curr_blick_tick)// нашли в какой диапозон (шаг анимации) попал текущий кадр
		{
			prev_blick_tick = (int) (curr_blick_tick - Blick_Tick[i] * (double)AsConfig::FPS) ;
			number_tick = i;
			break;
		}
	}
	
	if(number_tick >= 0 && number_tick < Blick_Count)
	  curr_tick = Blick_Tick[number_tick] * (double)AsConfig::FPS;

	// 2. Проверка стоит ли нам начинать анимацию заново или нет
	if(number_tick == INT_MIN) // проверка (если текущий тик таймера больше чем сумма всей анимации) если да, то начинаем ее заново
	{
		Start_Animation = AsConfig::Current_Timer_Tick;
		number_tick = 0;
		curr_tick = 0.0;
	}
}
void AMonster_Eye::Correct_Drawing_Rect(RECT &rect, int left_offset, int top_offset, int right_offset, int bottom_offset)
{
	const int scale = AsConfig::Global_Scale;

	rect = Monster_Rect;

	rect.left += left_offset * scale;
	rect.top += top_offset * scale;
	rect.right += right_offset * scale;
	rect.bottom += bottom_offset * scale;
}




//AMonster_Comet---------------------------------------------------------------------------------
AMonster_Comet::AMonster_Comet()
	: Ticks_Per_Rotation(0), Current_Angle(0.0)
{
	int rotation_ticks_delta = Max_Ticks_Per_Rotation - Min_Ticks_Per_Rotation;

	Ticks_Per_Rotation = AsCommon::Rand(rotation_ticks_delta) + Min_Ticks_Per_Rotation;
}
void AMonster_Comet::Act_For_Alive_State()
{
	int time_offset;
	double ratio;

	if (Monster_State == EMonster_State::Idle)
		return;

	time_offset = (AsConfig::Current_Timer_Tick - Start_Animation) % Ticks_Per_Rotation;
	ratio = (double)time_offset / (double)Ticks_Per_Rotation; // для коэффа от 0 до 1 делим число на не превыщающий его максимум

	Current_Angle = ratio * 2.0 * M_PI;

}
void AMonster_Comet::Draw_For_Alive_State(HDC hdc, RECT &paint_area)
{
	int i;
	XFORM xform, old_xform;
	RECT rect, ball_rect;
	double alpha, monster_radius;
	double x_pos, y_pos;
	const int scale = AsConfig::Global_Scale;
	const double d_scale = AsConfig::D_Global_Scale;
	const int ball_size = 11;

	if(Monster_State == EMonster_State::Idle)
		return;

   alpha = Current_Angle; /* -2.0 * M_PI / (double)Max_Rolling_Step * (double)Rolling_Step;*/

	monster_radius = (double)Width * d_scale / 2.0 - 1;
	x_pos = (double)X_Pos * d_scale + monster_radius; // координаты центра мячика
	y_pos = (double)Y_Pos * d_scale + monster_radius;

	
	 GetWorldTransform(hdc, &old_xform);

	for(i = 0; i < 2; ++i)
	{
		 xform.eM11 = (float)cos(alpha);
		 xform.eM12 = (float)sin(alpha);
		 xform.eM21 = (float)-sin(alpha);
		 xform.eM22 = (float)cos(alpha);
		 xform.eDx = (float)(x_pos);
		 xform.eDy = (float)(y_pos);

		 SetWorldTransform(hdc, &xform);

		 ball_rect.left = (int)-monster_radius;
		 ball_rect.top = -ball_size / 2;
		 ball_rect.right = ball_rect.left + ball_size;
		 ball_rect.bottom = ball_rect.top + ball_size;
		 
	

		 AsCommon::Ellipse(hdc, ball_rect, AsConfig::White_Color);
		 
		 rect.left = (int)(-monster_radius + (double)ball_size / 2.0);
		 rect.top = (int)-monster_radius + 2 * scale;
		 rect.right = (int)(monster_radius - (double)ball_size / 2.0);
		 rect.bottom = (int)monster_radius  - 2 * scale;
		 
		 AsConfig::Comet_Color.Select_Pen(hdc);
		 Arc(hdc, rect.left, rect.top, rect.right, rect.bottom, 0, rect.top, rect.left, rect.top + 3 * scale);
		 
		 rect.left -= scale - 1;
		 
		Arc(hdc, rect.left, rect.top, rect.right, rect.bottom, 0, rect.top, rect.left, rect.top + 3 * scale);

		alpha += M_PI;
	}
	
	SetWorldTransform(hdc, &old_xform);
  
}
void AMonster_Comet::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if (! IntersectRect(&intersection_rect, &paint_area, &Prev_Monster_Rect) )
		return;

	AsCommon::Rect(hdc, Prev_Monster_Rect, AsConfig::BG_Color);
	
}
