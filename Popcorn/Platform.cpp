#include "Platform.h"

// AsPlatform
const double AsPlatform::Max_Glay_Ratio = 1.0;
const double AsPlatform::Min_Glay_Ratio = 0.0;
//------------------------------------------------------------------------------------------------------------
AsPlatform::AsPlatform()
: Width(Normal_Width),X_Pos(AsConfig::Border_X_Offset), Platform_State(EPS_Missing), Platform_Moving_State(EPMS_Stop),Inner_Width(Normal_Platform_Inner_Width),
  Rolling_Step(0), Normal_Platform_Image_Width(0), Normal_Platform_Image_Height(0),Left_Key_Dowm(false), Right_Key_Down(false), Speed(0.0), Platform_Glay_Ratio(0.0) ,Normal_Platform_Image(0),Platform_Rect{}, 
  Prev_Platform_Rect{}, Highlight_Pen_Color(255, 255, 255), Platform_Circle_Color(151, 0, 0), Platform_Inner_Color(0, 128, 192)
{
	X_Pos = (AsConfig::Max_X_Pos - Width) / 2;
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
	if(next_y_pos + ball->Radius < AsConfig::Platform_Y_Pos)
		return false;

	inner_top_y = (double) (AsConfig::Platform_Y_Pos + 1);
	inner_left_x = (double) (X_Pos + (Circle_Size - 1));
	inner_right_x = (double) (X_Pos + Width - (Circle_Size - 1));
	inner_low_y = (double) (AsConfig::Platform_Y_Pos + (Height - 1));

	//1. Проверка отражения от мячика
	if(Reflect_On_Circle( next_x_pos, next_y_pos, 0, ball))
	    goto _on_hit;// от левого
	
	if(Reflect_On_Circle( next_x_pos, next_y_pos, Width - Circle_Size, ball))
	    goto _on_hit; // от правого

//ball->Ball_Speed = 0.3;

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

	if(Platform_State == EPS_Glay)
	{
		ball->Get_Center(ball_x, ball_y);
		ball->Set_State(EBS_On_Platform, ball_x, ball_y);
	}
	return true;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Act()
{
	switch (Platform_State)
	{
		case EPS_Meltdown:
		case EPS_Roll_In:
		case EPS_Expand_Roll_In:
			Redraw_Platform();
			break;

		case EPS_Glay_Init:
			if(Platform_Glay_Ratio < AsPlatform::Max_Glay_Ratio)
				Platform_Glay_Ratio += 0.05;
			else
				Platform_State = EPS_Glay;

			Redraw_Platform(false);

			break;
	
		case EPS_Glay_Finalize:
			if(Platform_Glay_Ratio > AsPlatform::Min_Glay_Ratio)
				Platform_Glay_Ratio -= 0.05;
			else
				Platform_State = EPS_Normal;

			Redraw_Platform(false);

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
	case EPS_Ready:
	case EPS_Normal:
		Draw_Normal_State(hdc, paint_area);
		break;

	case EPS_Pre_Meltdown:
		Draw_Normal_State(hdc, paint_area);
		Set_State(EPS_Meltdown);
		break;

	case EPS_Meltdown:
		Draw_Meltdown_State(hdc, paint_area);
		break;

	case EPS_Roll_In:
		Draw_Roll_In_State(hdc, paint_area);
		break;

	case EPS_Expand_Roll_In:
		Draw_Expanding_Roll_In_State(hdc, paint_area);
		break;

	case EPS_Glay_Init:
		Draw_Glay_State(hdc, paint_area);
		break;

	case EPS_Glay:
		Draw_Glay_State(hdc, paint_area);
		break;
	case EPS_Glay_Finalize:
		Draw_Glay_State(hdc, paint_area);
		break;
	}
}
void AsPlatform::Clear(HDC hdc, RECT &paint_area)
{
	switch (Platform_State)
	{
	case EPS_Ready:
	case EPS_Normal:
	case EPS_Pre_Meltdown:
	case EPS_Roll_In:
	case EPS_Expand_Roll_In:
	case EPS_Glay_Init:
	case EPS_Glay:
	case EPS_Glay_Finalize:
		Clear_BG(hdc, paint_area);
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Init(AsBall_Set *ball_set)
{
	Ball_Set = ball_set;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::On_Space_Key(bool key_down)
{
	if(! key_down)
		return;
	
	switch(Platform_State)
	{
		case  EPS_Ready:
			Ball_Set->Release_From_Platform(Get_Middle_Pos());
			Set_State(EPS_Normal);
			break;

		case EPS_Glay:
			Ball_Set->Release_Next_Ball();
			break;
	
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Advance(double max_speed)
{
	double step;
	
	if(Platform_Moving_State == EPMS_Stopping || Platform_Moving_State == EPMS_Stop)
		return;

	step = Speed / max_speed * AsConfig::Moving_Size_Step;
	X_Pos += step;

	if(X_Pos <= AsConfig::Border_X_Offset)
	{
	   X_Pos = AsConfig::Border_X_Offset;
	   Speed = 0.0;	
		Platform_Moving_State = EPMS_Stopping;
	}
	if(X_Pos >= AsConfig::Max_X_Pos - Width + 1)
	{ 
		X_Pos = AsConfig::Max_X_Pos - Width + 1;
		Speed = 0.0;
		Platform_Moving_State = EPMS_Stopping;
	
	}

	// cмещаем приклеенные мячики
	if(Platform_State == EPS_Glay || Platform_State == EPS_Ready)
	{
		if(Platform_Moving_State == EPMS_Left)
			Ball_Set->On_Platform_Advance(M_PI, fabs(Speed), max_speed);
		else
			if(Platform_Moving_State == EPMS_Right)
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
	if(Platform_Moving_State == EPMS_Stop)
		return;

	Redraw_Platform();

	if(Platform_Moving_State == EPMS_Stopping)
		Platform_Moving_State = EPMS_Stop;
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
	case EPS_Pre_Meltdown:
		Speed = 0.0;
		break;

	case EPS_Meltdown:
		len = sizeof(Meltdown_Platform_Y_Pos) / sizeof(Meltdown_Platform_Y_Pos[0]);

		for (i = 0; i < len; i++)
			Meltdown_Platform_Y_Pos[i] = Platform_Rect.top;

		break; 

	case EPS_Roll_In:
		X_Pos = AsConfig::Max_X_Pos - 1;
		Rolling_Step = Max_Rolling_Step - 1;
		break;

	case EPS_Glay_Init:
		if(Platform_State == EPS_Glay || Platform_State == EPS_Glay_Finalize)
			return;
		else
			Platform_Glay_Ratio = 0.2;
		break;
	case EPS_Glay:
		AsConfig::Throw(); // такое состояние устанавливается в другом методе
		break;
	case EPS_Glay_Finalize:
		if( ! (Platform_State == EPS_Glay || Platform_State == EPS_Glay_Init) )
			return;

		while (Ball_Set->Release_Next_Ball() )
		{
		}
		break;
	}

	Platform_State = new_state;
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Redraw_Platform(bool update_rect)
{
	int platform_width;

	if(update_rect)
	{
		Prev_Platform_Rect = Platform_Rect;

		if (Platform_State == EPS_Roll_In)
			platform_width = Circle_Size;
		else
			platform_width = Width;

		Platform_Rect.left = (int) ( X_Pos * AsConfig::D_Global_Scale);
		Platform_Rect.top = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
		Platform_Rect.right = Platform_Rect.left + platform_width * AsConfig::Global_Scale;
		Platform_Rect.bottom = Platform_Rect.top + Height * AsConfig::Global_Scale;

		if (Platform_State == EPS_Meltdown)
			Prev_Platform_Rect.bottom = (AsConfig::Max_Y_Pos + 1) * AsConfig::Global_Scale;
	}

	InvalidateRect(AsConfig::Hwnd, &Prev_Platform_Rect, FALSE);
	InvalidateRect(AsConfig::Hwnd, &Platform_Rect, FALSE);
}
//------------------------------------------------------------------------------------------------------------

 void AsPlatform::Move(bool is_left, bool key_down)
 {
	 if(! (Platform_State == EPS_Normal || Platform_State == EPS_Glay_Init || Platform_State == EPS_Glay || Platform_State == EPS_Glay_Finalize))
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
		 Platform_Moving_State = EPMS_Stopping;
		 return;
	 }

	 if(Left_Key_Dowm)
	 {
		 Platform_Moving_State = EPMS_Left;
		 Speed = -X_Step;
	 }

	 if(Right_Key_Down)
	 {
	   Platform_Moving_State = EPMS_Right;
	   Speed = X_Step;
	 }
 }
 //------------------------------------------------------------------------------------------------------------
double AsPlatform::Get_Middle_Pos()
{
	return (double) (X_Pos + (double)Width / 2.0);
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

void AsPlatform::Clear_BG(HDC hdc, RECT &paint_area)
{// Очищаем фоном прежнее место
	RECT intersection_rect;

	if ( !(IntersectRect(&intersection_rect, &paint_area, &Prev_Platform_Rect)) )
		return;

	AsConfig::BG_Color.Select(hdc);
	Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Circle_Highlight(HDC hdc, int x, int y)
{// Рисуем блик на шарике

	AsPlatform::Highlight_Pen_Color.Select_Pen(hdc);

	Arc(hdc, x + AsConfig::Global_Scale, y + AsConfig::Global_Scale, x + (Circle_Size - 1) * AsConfig::Global_Scale - 1, y + (Circle_Size - 1) * AsConfig::Global_Scale - 1,
		x + 2 * AsConfig::Global_Scale, y + AsConfig::Global_Scale, x + AsConfig::Global_Scale, y + 3 * AsConfig::Global_Scale);
}
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

	Ellipse(hdc, (int)(x * d_scale), y * scale, (int)((x + (double)Circle_Size) * d_scale - 1), (y + Circle_Size) * scale - 1);
	Ellipse(hdc,(int)( (x + (double)Inner_Width) * d_scale), y * scale, (int)((x + (double)Circle_Size + (double)Inner_Width) * d_scale - 1), (y + Circle_Size) * scale - 1);

	// 2. Рисуем блик
	Draw_Circle_Highlight(hdc, x * d_scale, y * scale);

	// 3. Рисуем среднюю часть
	AsPlatform::Platform_Inner_Color.Select(hdc);

	inner_rect.left = (x + 4) * d_scale;
	inner_rect.top = (y + 1) * scale;
	inner_rect.right = (x + 4 + Inner_Width - 1) * d_scale;
	inner_rect.bottom = (y + 1 + 5) * scale;

	AsConfig::Round_Rect(hdc, inner_rect, 3);

	
	if(Normal_Platform_Image == 0 && Platform_State == EPS_Ready)
		Get_Normal_Platform_Image(hdc);
	
}
void AsPlatform::Draw_Glay_State(HDC hdc, RECT &paint_area)
{
	Draw_Normal_State(hdc, paint_area);

	HRGN region;
	RECT glue_rect;

	glue_rect.left = (X_Pos + 5) * AsConfig::Global_Scale;
	glue_rect.top = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
	glue_rect.right = glue_rect.left + Normal_Platform_Inner_Width * AsConfig::Global_Scale;
	glue_rect.bottom = glue_rect.top + (Height - 2) * AsConfig::Global_Scale;

	region = CreateRectRgnIndirect(&glue_rect);

	SelectClipRgn(hdc, region);

	AsConfig::BG_Color.Select(hdc);

	Draw_Spot_State(hdc, 5, 8, 5);
	Draw_Spot_State(hdc, 12, 5, 4);
	Draw_Spot_State(hdc, 15, 8, 5);

	AsConfig::White_Color.Select(hdc);

	Draw_Spot_State(hdc, 6, 6, 4);
	Draw_Spot_State(hdc, 12, 5, 3);
	Draw_Spot_State(hdc, 15, 7, 4);

	SelectClipRgn(hdc, 0);
	DeleteObject(region);
}
void AsPlatform::Draw_Spot_State(HDC hdc, int x_offset, int width, int height)
{
	RECT rect;
	int platform_y = (AsConfig::Platform_Y_Pos + 1) * AsConfig::Global_Scale;
	double spot_height = (double)height * Platform_Glay_Ratio;

	rect.left = (X_Pos + x_offset) * AsConfig::Global_Scale;
	rect.top = platform_y - spot_height * AsConfig::Global_Scale;
	rect.right = rect.left + width * AsConfig::Global_Scale;
	rect.bottom = (AsConfig::Platform_Y_Pos + spot_height + 1) * AsConfig::Global_Scale;
		
	Chord(hdc, rect.left, rect.top, rect.right - 1, rect.bottom - 2, rect.left, platform_y, rect.right, platform_y);
}

//------------------------------------------------------------------------------------------------------------
void AsPlatform::Get_Normal_Platform_Image(HDC hdc)
{
	int i,j;
	int x, y;
	int offset = 0;

	x = (int) (X_Pos * AsConfig::D_Global_Scale);
	y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;

	Normal_Platform_Image_Width = Width * AsConfig::Global_Scale;
	Normal_Platform_Image_Height = Height * AsConfig::Global_Scale;

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
		Platform_State = EPS_Missing;  // Вся платформа сдвинулась за пределы окна
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Roll_In_State(HDC hdc, RECT &paint_area)
{// Рисуем выкатывающуюся платформу

	int x = (int) (X_Pos * AsConfig::D_Global_Scale);
	int y = AsConfig::Platform_Y_Pos * AsConfig::Global_Scale;
	int roller_size = Circle_Size * AsConfig::Global_Scale;
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
	Draw_Circle_Highlight(hdc, x, y);

	++Rolling_Step;

	if (Rolling_Step >= Max_Rolling_Step)
		Rolling_Step -= Max_Rolling_Step;

	X_Pos -= Rolling_Platform_Speed;

	if (X_Pos <= Roll_In_Platform_End_X_Pos)
	{
		X_Pos += Rolling_Platform_Speed;
		Platform_State = EPS_Expand_Roll_In;
		Inner_Width = 1;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsPlatform::Draw_Expanding_Roll_In_State(HDC hdc, RECT &paint_area)
{// Рисуем расширяющуюся после выкатывания платформу

	Draw_Normal_State(hdc, paint_area);

	--X_Pos;
	Inner_Width += 2;

	if (Inner_Width >= Normal_Platform_Inner_Width)
	{
		Inner_Width = Normal_Platform_Inner_Width;
		Platform_State = EPS_Ready;
		Redraw_Platform();
	}
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

	platform_ball_radius = (double)Circle_Size / 2.0;
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

	if(image_color == Highlight_Pen_Color.Get_RGB())
		color.Set_Pen(AsPlatform::Highlight_Pen_Color.Get_Pen()); 

	else if(image_color == Platform_Circle_Color.Get_RGB())
		color.Set_Pen(AsPlatform::Platform_Circle_Color.Get_Pen()); 
	
	else if(image_color == Platform_Inner_Color.Get_RGB())
	   color.Set_Pen(AsPlatform::Platform_Inner_Color.Get_Pen()); 

	else if(image_color == AsConfig::BG_Color.Get_RGB())
		 color.Set_Pen(AsConfig::BG_Color.Get_Pen()); 
		

	return true;
}