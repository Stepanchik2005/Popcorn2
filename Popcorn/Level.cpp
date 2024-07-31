#include "Level.h"



//-ALevel--------------------------------------------------------------------------------------------------------
char ALevel::Level_01[AsConfig::Level_Height][AsConfig::Level_Width] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 7,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

char ALevel::Test_Level[AsConfig::Level_Height][AsConfig::Level_Width] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// ALevel
//------------------------------------------------------------------------------------------------------------
ALevel::~ALevel()
{
	delete[] Teleport_Pos;
}
//------------------------------------------------------------------------------------------------------------

ALevel::ALevel()
: Level_Rect{}, Active_Brick_Counter(0), Falling_Letters_Counter(0), Teleport_Counter(0), Teleport_Pos(0), Advertisement(0), Need_To_Stop_All_Activity(false)
{
}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Check_Hit(double next_x_pos, double next_y_pos, ABall *ball)
{// Корректируем позицию мячика при отражении от кирпичей

	int i, j;
	double direction;
	double min_ball_x, max_ball_x;
	double min_ball_y, max_ball_y;
	int min_level_x, max_level_x;
	int min_level_y, max_level_y;
	bool got_horizontal_hit, got_vertical_hit;
	double horizontal_reflection_pos, vertical_reflection_pos;

	if (next_y_pos + ball->Radius > AsConfig::Level_Y_Offset + (AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height)
		return false;

	direction = ball->Get_Direction();

	min_ball_x = next_x_pos - ball->Radius;
	max_ball_x = next_x_pos + ball->Radius;
	min_ball_y = next_y_pos - ball->Radius;
	max_ball_y = next_y_pos + ball->Radius;

	min_level_x = (int)( (min_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);

	if(min_level_x < 0) // валидация уровня
		min_level_x = 0;

	max_level_x = (int)( (max_ball_x - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);

	if(max_level_x > AsConfig::Level_Width - 1)
		max_level_x = AsConfig::Level_Width - 1;

	min_level_y = (int)( (min_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);

	if(min_level_y < 0)
		min_level_y = 0;

	max_level_y = (int)( (max_ball_y - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);

	if(max_level_y > AsConfig::Level_Height - 1)
		max_level_y = AsConfig::Level_Height - 1;

	for (i = max_level_y; i >= min_level_y; i--)
	{
		Current_Brick_Top_Y = AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height;
		Current_Brick_Low_Y = Current_Brick_Top_Y + AsConfig::Brick_Height;

		for (j = min_level_x; j <= max_level_x; j++)
		{
			if (Current_Level[i][j] == 0)
				continue;

			Current_Brick_Left_X = AsConfig::Level_X_Offset + j * AsConfig::Cell_Width;
			Current_Brick_Right_X = Current_Brick_Left_X + AsConfig::Brick_Width;

			got_horizontal_hit = Check_Horizontal_Hit(next_x_pos, next_y_pos, j, i, ball, horizontal_reflection_pos);

			got_vertical_hit = Check_Vertical_Hit(next_x_pos, next_y_pos, j, i, ball, vertical_reflection_pos);

			if (got_horizontal_hit && got_vertical_hit)
			{
				 if(On_Hit(j, i, ball, true))
				 {
				    if (vertical_reflection_pos < horizontal_reflection_pos)
				    	ball->Reflect(true);
				    else
				    	ball->Reflect(false);
				 }

          
				
				return true;
			}
			else
				if (got_horizontal_hit) // получили удар слева/справа от мячика
				{
					if(On_Hit(j, i, ball, false))
						ball->Reflect(false);

					return true;
					
				}
				else
					if (got_vertical_hit) // // получили удар сверху/снизу от мячика
					{
						if(On_Hit(j, i, ball, true))
							ball->Reflect(true);

						return true;
						
					}
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Check_Hit(double next_x_pos, double next_y_pos)
{	
	int level_x, level_y;
	double current_brick_top_y, current_brick_low_y, current_brick_left_x, current_brick_right_x;
	// сделать проверку на активность кирпича

	if (next_y_pos > AsConfig::Level_Y_Offset + (AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height)
		return false;


	level_x = (int)( (next_x_pos - AsConfig::Level_X_Offset) / (double)AsConfig::Cell_Width);

	if(level_x > AsConfig::Level_Width - 1)
		level_x = AsConfig::Level_Width - 1;

	level_y = (int)( (next_y_pos - AsConfig::Level_Y_Offset) / (double)AsConfig::Cell_Height);

	if(level_y > AsConfig::Level_Height - 1)
		level_y = AsConfig::Level_Height - 1;

	
	if (Current_Level[level_y][level_x] == 0)
		return false;
	
	current_brick_top_y = AsConfig::Level_Y_Offset + level_y * AsConfig::Cell_Height;
	current_brick_low_y = current_brick_top_y + AsConfig::Brick_Height;

	current_brick_left_x = AsConfig::Level_X_Offset + level_x * AsConfig::Cell_Width;
	current_brick_right_x = current_brick_left_x + AsConfig::Brick_Width;

	if(next_x_pos >= current_brick_left_x && next_x_pos <= current_brick_right_x)
		if(next_y_pos <= current_brick_low_y)
		{
			On_Hit(level_x, level_y, 0, true);
			return true;
		}
	
	return false;
}


//------------------------------------------------------------------------------------------------------------
void ALevel::Act()
{
	Act_Objects((AGraphics_Objects**)&Active_Bricks, Active_Brick_Counter, AsConfig::Max_Active_Brick_Size);
	Act_Objects((AGraphics_Objects**)&Falling_Letters, Falling_Letters_Counter, AsConfig::Max_Falling_Letters_Size);

	if(Advertisement != 0)
		Advertisement->Act();
}
bool ALevel::Is_Finished()
{
	return false;
}
void ALevel::Draw(HDC hdc, RECT &paint_area)
{
	int i, j;
	RECT intersection_rect, brick_rect = {};

	
	if(Advertisement != 0)
		Advertisement->Draw(hdc, paint_area);

	if(Need_To_Stop_All_Activity)
		Delete();

	if (IntersectRect(&intersection_rect, &paint_area, &Level_Rect) )
	{
		for (i = 0; i < AsConfig::Level_Height; i++)
	    	for (j = 0; j < AsConfig::Level_Width; j++)
			{
				brick_rect.left = (AsConfig::Level_X_Offset + j * AsConfig::Cell_Width) * AsConfig::Global_Scale;
				brick_rect.top = (AsConfig::Level_Y_Offset + i * AsConfig::Cell_Height) * AsConfig::Global_Scale;
				brick_rect.right = brick_rect.left + AsConfig::Brick_Width * AsConfig::Global_Scale;
				brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height * AsConfig::Global_Scale;

				if(IntersectRect(&intersection_rect, &paint_area, &brick_rect))
	    			Draw_Brick(hdc, brick_rect, j, i);
			}

	
		Draw_Objects(hdc, paint_area, (AGraphics_Objects**)&Active_Bricks, AsConfig::Max_Active_Brick_Size);
	}

	Draw_Objects(hdc, paint_area, (AGraphics_Objects**)&Falling_Letters, AsConfig::Max_Falling_Letters_Size);

	
}
void ALevel::Clear(HDC hdc, RECT &paint_area)
{
	// 1. Очищаем движущиеся объекты
	Clear_Objects(hdc, paint_area, (AGraphics_Objects**)&Falling_Letters, AsConfig::Max_Falling_Letters_Size);
	
	if(Advertisement != 0)
		Advertisement->Clear(hdc, paint_area);
}
	
// нету реализации для методов AMover т.к. уровень не двигается
void ALevel::Advance(double max_speed)
{
}
double ALevel::Get_Speed()
{
	return 0.0;
}
void ALevel::Start_Movement()
{
}
void ALevel::End_Movement()
{
}

void ALevel::Init()
{

	Level_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	Level_Rect.top = AsConfig::Level_Y_Offset * AsConfig::Global_Scale;
	Level_Rect.right = Level_Rect.left + AsConfig::Cell_Width * AsConfig::Level_Width * AsConfig::Global_Scale;
	Level_Rect.bottom = Level_Rect.top + AsConfig::Cell_Height * AsConfig::Level_Height * AsConfig::Global_Scale;

	memset(Current_Level, 0, sizeof(Current_Level) );
	memset(Active_Bricks, 0, sizeof(Active_Bricks) );
   memset(Falling_Letters, 0, sizeof(Falling_Letters) );

}
//------------------------------------------------------------------------------------------------------------
void ALevel::Set_Current_Level(char level[AsConfig::Level_Height][AsConfig::Level_Width])
{
	int i,j;
	int index;
	EBrick_Type brick_type;

	memcpy(Current_Level, level, sizeof(Current_Level) );

	// 1. Считаем телепорты (только для того, чтобы задать размер массива)
	for(i = 0; i < AsConfig::Level_Height; ++i)
	{
		for(j = 0; j < AsConfig::Level_Width; ++j)
		{
			brick_type = (EBrick_Type)ALevel::Current_Level[i][j];
			if(brick_type == EBrick_Type::Teleport)
				Teleport_Counter++;
		}
		if(Teleport_Counter > ALevel::Max_Teleports_In_Level)
			Teleport_Counter = ALevel::Max_Teleports_In_Level;
	}
	if(Teleport_Counter < 2 && Teleport_Counter != 0)
		AsConfig::Throw();

	// 2. Записать телепорты в массив
	delete[] Teleport_Pos;
	Teleport_Pos = 0;

	Teleport_Pos = new SPoint[Teleport_Counter];

	if(Teleport_Pos != 0)
	{
		index = 0;
		for(i = 0; i < AsConfig::Level_Height; ++i)
	   {
			for(j = 0; j < AsConfig::Level_Width; ++j)
			{
				brick_type = (EBrick_Type)ALevel::Current_Level[i][j];
				if(brick_type == EBrick_Type::Teleport)
				{
					Teleport_Pos[index].X = j;
					Teleport_Pos[index].Y = i;
					index++;
				}
			}
		}
	}

	//Advertisement = new AAdvertisement(9, 5, 2, 3);
	

}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Get_Next_Falling_Letter(int& index, AFalling_Letter **falling_letter)
{
	AFalling_Letter *current_letter;
	if(Falling_Letters_Counter == 0)
		return false;

	if(index < 0 || index >= AsConfig::Max_Falling_Letters_Size)
		return false;
  
	*falling_letter = Falling_Letters[index];

	while(index < AsConfig::Max_Falling_Letters_Size)
	{
		current_letter = Falling_Letters[index++];

		if(current_letter != 0)
		{
			*falling_letter = current_letter;
			return true;
		}

	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Stop()
{
	Need_To_Stop_All_Activity = true;
}
//------------------------------------------------------------------------------------------------------------
bool ALevel::On_Hit(int brick_x, int brick_y, ABall *ball, bool got_vertical_hit)
{
	EBrick_Type brick_type;
	bool can_reflect = true;

	brick_type = (EBrick_Type)Current_Level[brick_y][brick_x];

	
	if(brick_type == EBrick_Type::Paraschute)
	{ 
		if(ball != 0)
		{
			ball->Set_On_Paraschute(brick_x, brick_y);
			Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		}
		else
		{
			brick_type = EBrick_Type::Red;
			Current_Level[brick_y][brick_x] = (char)EBrick_Type::Red; // превращаем кирпич с парашютом в красный при попадании луча
		}
	}


	else if(Add_Falling_Letter(brick_type, brick_x,  brick_y))
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
	
	else
		if(Create_Active_Brick(brick_x, brick_y, brick_type, ball, got_vertical_hit))
			can_reflect = false;
		
	
	Redraw_Brick(brick_x, brick_y);	

	return can_reflect;
} 
//------------------------------------------------------------------------------------------------------------
void ALevel::Clear_Objects(HDC hdc, RECT &paint_area, AGraphics_Objects** object_for_drawing,  int max_size)
{
	int i;
	for(i = 0; i < max_size; ++i)
	{
		if(object_for_drawing[i] != 0)
			object_for_drawing[i]->Clear(hdc, paint_area);
	}
	
}

//------------------------------------------------------------------------------------------------------------
void ALevel::Redraw_Brick(int brick_x, int brick_y)
{
	const int scale = AsConfig::Global_Scale;
	RECT brick_rect = {};

	brick_rect.left = (AsConfig::Level_X_Offset + brick_x * AsConfig::Cell_Width) * scale;
	brick_rect.top = (AsConfig::Level_Y_Offset + brick_y * AsConfig::Cell_Height) * scale;
	brick_rect.right = brick_rect.left + AsConfig::Brick_Width * scale;
	brick_rect.bottom = brick_rect.top + AsConfig::Brick_Height * scale;

	AsCommon::Invalidate_Rect(brick_rect);
}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall *ball, double &reflection_pos)
{
	double direction = ball->Get_Direction();

	// Проверяем попадание в нижнюю грань
	if(ball->Is_Moving_Up())
	{
		if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Low_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, ball->Radius, reflection_pos) )
		{
			// Проверяем возможность отскока вниз
			if (level_y < AsConfig::Level_Height - 1 && Current_Level[level_y + 1][level_x] == 0)
				return true;
			else
				return false;
		}
	}
	else
	{// Проверяем попадание в верхнюю грань
	   if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Top_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, ball->Radius, reflection_pos) )
		{
			// Проверяем возможность отскока вверх
			if (level_y > 0 && Current_Level[level_y - 1][level_x] == 0)
				return true;
			else
				return false;
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall *ball, double &reflection_pos)
{
	double direction = ball->Get_Direction();

	// Проверяем попадание в левую грань
	if (! ball->Is_Moving_Left())
	{
		if (Hit_Circle_On_Line(Current_Brick_Left_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, ball->Radius, reflection_pos) )
		{
			// Проверяем возможность отскока влево
			if (level_x > 0 && Current_Level[level_y][level_x - 1] == 0)
				return true;
			else
				return false;
		}
	}
	else
	{// Проверяем попадание в правую грань
		if (Hit_Circle_On_Line(Current_Brick_Right_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, ball->Radius, reflection_pos) )
		{
			// Проверяем возможность отскока вправо
			if (level_x < AsConfig::Level_Width - 1 && Current_Level[level_y][level_x + 1] == 0)
				return true;
			else
				return false;
		}
	}
	return false;
}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Add_Falling_Letter(EBrick_Type brick_type,int brick_x, int brick_y)
{
	int i;
	ELetter_Type letter_type;
	AFalling_Letter *falling_letter;
	int letter_x, letter_y;
	if( !(brick_type == EBrick_Type::Blue || brick_type == EBrick_Type::Red) )
		return false;
	
	if(AsCommon::Rand(AsConfig::Max_Random_Number_For_Letter) != 0) // рандом: или буква или нет
		return false;
		
	if(Falling_Letters_Counter >= AsConfig::Max_Falling_Letters_Size - 1) // падающих букв слишком много
		return false;
			
	for(i = 0; i < AsConfig::Max_Falling_Letters_Size; ++i)
		{
			if(Falling_Letters[i] == 0)
			{
				letter_x = (AsConfig::Level_X_Offset + brick_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
				letter_y = (AsConfig::Level_Y_Offset + brick_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;

				letter_type = AFalling_Letter::Get_Random_Letter_Type();

				switch(AsCommon::Rand(3))
				{
				case 0:
					letter_type = ELetter_Type::K;
					break;
				case 1:
					letter_type = ELetter_Type::Sh;
					break;
				case 2:
					letter_type = ELetter_Type::L;
					break;
				}
				/*if(AsCommon::Rand(2) == 0)
					letter_type = ELetter_Type::Sh;
				else
					letter_type = ELetter_Type::L;*/

				//letter_type = ELetter_Type::L;

				falling_letter = new AFalling_Letter (letter_type, letter_x, letter_y, brick_type);
				Falling_Letters[i] = falling_letter;
				Falling_Letters_Counter++;
				return true;
			}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Create_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type, ABall *ball, bool got_vertical_hit)
{ // brick_x, brick_у - номер позиции в двухмерном массиве конкретного кирпича 
	bool is_teleport = false;
	//double ball_x, ball_y;
	AActive_Brick *active_brick = 0;
	AActive_Brick_Teleport *destination_teleport = 0;

	if(Active_Brick_Counter > AsConfig::Max_Active_Brick_Size - 1)
		return is_teleport;//Активных кирпичей слишком много

	switch(brick_type)
	{
	case EBrick_Type::None:
		break;

	case EBrick_Type::Blue:
	case EBrick_Type::Red:
		active_brick = new AActive_Brick_Red_Blue(brick_x,brick_y,brick_type);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		break;

	case EBrick_Type::Unbreakable:
		active_brick = new AActive_Brick_Unbreakable(brick_x,brick_y);
		break;

	case EBrick_Type::Multihit_1:
		active_brick = new AActive_Brick_Multihit(brick_x,brick_y);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
		break;

	case EBrick_Type::Multihit_2:
	case EBrick_Type::Multihit_3:
	case EBrick_Type::Multihit_4:
		Current_Level[brick_y][brick_x] = (int)brick_type - 1;
	   break;

	case EBrick_Type::Teleport:
		Add_Active_Brick_Teleport(brick_x, brick_y, ball, got_vertical_hit);
		is_teleport = true;
		break;

	case EBrick_Type::Ad:
		active_brick = new AActive_Brick_Ad(brick_x,brick_y, Advertisement);
		Current_Level[brick_y][brick_x] = (char)EBrick_Type::Invisible;
		break;

	case EBrick_Type::Invisible:
		break;

	default:
		AsConfig::Throw();
	}

	if(active_brick != 0)
		Add_Active_Brick(active_brick); 
	
  return is_teleport;
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Add_Active_Brick(AActive_Brick *active_brick)
{
	int i;

	for(i = 0; i < AsConfig::Max_Active_Brick_Size; ++i) // добавление активного кирпича в массив активных кирпичей
	{
		if(Active_Bricks[i] == 0)
		{
			Active_Bricks[i] = active_brick;
			++Active_Brick_Counter;
			break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Add_Active_Brick_Teleport(int brick_x, int brick_y, ABall *ball, bool got_vertical_hit)
{
	double pre_x_pos, pre_y_pos;
	double curr_x_pos, curr_y_pos;
	int destination_teleport_x_pos = 0, destination_teleport_y_pos = 0; // координаты телепорта 
	bool got_direction = false;
	AActive_Brick *source_teleport = 0;
	AActive_Brick_Teleport *destination_teleport = 0;
	EDirection_State direction;

	ball->Get_Center(pre_x_pos, pre_y_pos);

	destination_teleport = Select_Destination_Teleport(brick_x, brick_y);
	source_teleport = new AActive_Brick_Teleport(brick_x,brick_y, ball, destination_teleport);
	
	destination_teleport->Get_X_Y(destination_teleport_x_pos, destination_teleport_y_pos);
	
	ball->Get_Center(curr_x_pos, curr_y_pos);
	// определение куда летел мячик
	if(got_vertical_hit)
	{
		if(pre_y_pos > curr_y_pos)
			direction = EDirection_State::Up;
		else
			direction = EDirection_State::Down;
	}
	else
	{
		if(pre_x_pos < curr_x_pos)
			direction = EDirection_State::Right;
		else
			direction = EDirection_State::Left;
	}

	// 2. Перебираем все направления в поисках свободного
	int i;
	for(i = 0; i < 4; ++i)
	{
	   switch(direction)
	   {
	   	case EDirection_State::Left: 
			   if(destination_teleport_x_pos > 0 && Current_Level[destination_teleport_y_pos][destination_teleport_x_pos - 1] == (char)EBrick_Type::None)
					got_direction = true;
				break;

			case EDirection_State::Up:
				if(destination_teleport_y_pos > 0 && Current_Level[destination_teleport_y_pos - 1][destination_teleport_x_pos] == (char)EBrick_Type::None)
					got_direction = true;
				break;

			case EDirection_State::Right:
				if(destination_teleport_x_pos < AsConfig::Level_Width - 1 && Current_Level[destination_teleport_y_pos][destination_teleport_x_pos + 1] == (char)EBrick_Type::None)
					got_direction = true;
				break;

			case EDirection_State::Down:
				if(destination_teleport_y_pos < AsConfig::Level_Height - 1 && Current_Level[destination_teleport_y_pos + 1][destination_teleport_x_pos] == (char)EBrick_Type::None)
					got_direction = true;
				break;

			default:
				AsConfig::Throw();
	   }
		if(got_direction == true)
			break;

		direction = (EDirection_State)((int)direction - 1);

		ball->Set_Direction(ball->Get_Direction() + M_PI_2);

		if((int)direction < 0)
			direction = EDirection_State::Down; // максимальное значение
	}

	destination_teleport->Release_Direction_State = direction;

	Add_Active_Brick(source_teleport);
	Add_Active_Brick(destination_teleport);
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport* ALevel::Select_Destination_Teleport(int brick_x, int brick_y)
{
	AActive_Brick_Teleport *destination_teleport = 0;
	int another_brick_x, another_brick_y;
	int rand_index;
	while(1) // рандомный выбор телепорта
	{
		rand_index = AsCommon::Rand(ALevel::Teleport_Counter);

		if( !(Teleport_Pos[rand_index].X == brick_x && Teleport_Pos[rand_index].Y == brick_y))
	        break;
	}
	another_brick_x = Teleport_Pos[rand_index].X;
	another_brick_y = Teleport_Pos[rand_index].Y;

	destination_teleport = new AActive_Brick_Teleport(another_brick_x, another_brick_y, 0, 0);

	return destination_teleport;
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Act_Objects(AGraphics_Objects** object_for_drawing, int &object_count, int max_size)
{
	int i;
	for(i = 0; i < max_size; ++i)
	{
		if(object_for_drawing[i] != 0)
		{
		  object_for_drawing[i]->Act();
		  
		  if(object_for_drawing[i]->Is_Finished())
		  {
		  	delete object_for_drawing[i];
		  	object_for_drawing[i] = 0;
			--object_count;
		  }
		}
	} 
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Draw_Brick(HDC hdc, RECT brick_rect, int level_x, int level_y)
{// Вывод "кирпича"

	const AColor *color = 0;
	EBrick_Type brick_type = (EBrick_Type)ALevel::Current_Level[level_y][level_x];

	switch (brick_type)
	{
	case EBrick_Type::None:
		if(Advertisement != 0 && Advertisement->Has_Brick_At(level_x, level_y))
			break;
		// else - No break;
	case EBrick_Type::Red:
	case EBrick_Type::Blue:
		AActive_Brick_Red_Blue::Draw_In_Level(hdc, brick_rect, brick_type);
		break;

	case EBrick_Type::Unbreakable:
		AActive_Brick_Unbreakable::Draw_In_Level(hdc, brick_rect);
		break;
	
	case EBrick_Type::Multihit_1:
	case EBrick_Type::Multihit_2:
	case EBrick_Type::Multihit_3:
	case EBrick_Type::Multihit_4:
	AActive_Brick_Multihit::Draw_In_Level(hdc, brick_rect, brick_type);
		break;

	case EBrick_Type::Paraschute:
		Draw_Paraschute_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Teleport:
		AActive_Brick_Teleport::Draw_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Ad:
		AActive_Brick_Ad::Draw_In_Level(hdc, brick_rect);
		break;

	case EBrick_Type::Invisible:
		break;
	default:
		AsConfig::Throw();
	}
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Draw_Objects(HDC hdc, RECT &paint_area, AGraphics_Objects** object_for_drawing, int max_size)
{
	int i;
	for(i = 0; i < max_size; ++i)
	{
		if(object_for_drawing[i] != 0)
	   object_for_drawing[i]->Draw(hdc, paint_area);
	}
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Draw_Paraschute_In_Level(HDC hdc, RECT &brick_rect)
{
	Draw_Part_Paraschute(hdc, brick_rect, 0, 4);
	Draw_Part_Paraschute(hdc, brick_rect, 4, 6);
	Draw_Part_Paraschute(hdc, brick_rect, 10, 4);
}
void ALevel::Draw_Part_Paraschute(HDC hdc, RECT &brick_rect, double offset, int width)
{
	const int scale = AsConfig::Global_Scale;
	RECT parashcute_rect = {};

	parashcute_rect.left = (int)( (double)brick_rect.left + offset * AsConfig::D_Global_Scale + 1.0);
	parashcute_rect.top = brick_rect.top + 1;
	parashcute_rect.right = parashcute_rect.left + width * scale + 1;
	parashcute_rect.bottom = parashcute_rect.top + 3 * scale;

	AsConfig::Paraschute_Color.Select(hdc);
	AsCommon::Round_Rect(hdc, parashcute_rect);

	parashcute_rect.top += 3 * scale + 1 ;
	parashcute_rect.bottom += 3 * scale + 1;

	AsConfig::Paraschute_Color.Select(hdc);
	AsCommon::Round_Rect(hdc, parashcute_rect);
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Delete()
{
	Delete_Objects((AGraphics_Objects**)&Active_Bricks, Active_Brick_Counter, AsConfig::Max_Active_Brick_Size);
	Delete_Objects((AGraphics_Objects**)&Falling_Letters, Falling_Letters_Counter, AsConfig::Max_Falling_Letters_Size);

	Need_To_Stop_All_Activity = false;
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Delete_Objects(AGraphics_Objects** object_for_drawing, int &object_count, int max_size)
{
	int i;
	for(i = 0; i < max_size; ++i)
	{
		if(object_for_drawing[i] != 0)
		{
		  	delete object_for_drawing[i];
		  	object_for_drawing[i] = 0;
			--object_count;
		  
		}
	} 
}

//------------------------------------------------------------------------------------------------------------