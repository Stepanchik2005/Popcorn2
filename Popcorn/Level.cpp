#include "Level.h"

// APoint
APoint::APoint()
{
}
//------------------------------------------------------------------------------------------------------------
APoint::APoint(int x_pos, int y_pos)
	: X(x_pos), Y(y_pos)
{
}



//AsLevel_Table-----------------------------------------------------------------------------------------------------------



AsLevel_Table::AsLevel_Table(int x_pos, int y_pos)
	: X_Pos(x_pos), Y_Pos(y_pos), Level_Table_State(ELevel_Table_State::Missing), Level_Label(X_Pos + 10, Y_Pos + 3, AsConfig::Max_X_Pos / 2 - Width / 2, Height - 5, AsConfig::Name_Font, AsConfig::White_Color),
	  Level_Number_Label(X_Pos + Width - 25, Y_Pos + 3, 20, Height - 5, AsConfig::Name_Font, AsConfig::White_Color),
	  Level_Number(0)
{
	const int scale = AsConfig::Global_Scale;

	Level_Table_Rect.left = X_Pos * scale;
	Level_Table_Rect.top = Y_Pos * scale;
	Level_Table_Rect.right = Level_Table_Rect.left + Width * scale;
	Level_Table_Rect.bottom = Level_Table_Rect.top + Height * scale;
}

void AsLevel_Table::Act()
{
	// заглушка, т.к нету своей анимации
}
bool AsLevel_Table::Is_Finished()
{
	return false; // заглушка, т.к нету своей анимации
}
void AsLevel_Table::Draw(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(Level_Table_State == ELevel_Table_State::Missing)
		return;

	if(IntersectRect(&intersection_rect, &paint_area, &Level_Table_Rect))
	{
		// 1. Рисуем плажку
		AsCommon::Rect(hdc, Level_Table_Rect, AsConfig::Blue_Color);

		Level_Label.Content = L"УРОВЕНЬ";

		Level_Label.Draw(hdc);

		Level_Number_Label.Content.Append(Level_Number, 2);

		Level_Number_Label.Draw(hdc);

	}
}
void AsLevel_Table::Clear(HDC hdc, RECT &paint_area)
{
	RECT intersection_rect;

	if(Level_Table_State != ELevel_Table_State::Hidding)
		return;

	if(IntersectRect(&intersection_rect, &paint_area, &Level_Table_Rect))
	{
		AsCommon::Rect(hdc, Level_Table_Rect, AsConfig::BG_Color);
		Level_Table_State = ELevel_Table_State::Missing;
		Level_Number_Label.Clear();
	}
}

void AsLevel_Table::Show(int level_number)
{
	Level_Table_State = ELevel_Table_State::Showing;

	Level_Number = level_number;

	AsCommon::Invalidate_Rect(Level_Table_Rect);
}
void AsLevel_Table::Hide()
{
	Level_Table_State = ELevel_Table_State::Hidding;

	AsCommon::Invalidate_Rect(Level_Table_Rect);
}




// ALevel
//------------------------------------------------------------------------------------------------------------
ALevel *ALevel::Level = 0;

//------------------------------------------------------------------------------------------------------------
ALevel::~ALevel()
{
	
}
//------------------------------------------------------------------------------------------------------------

ALevel::ALevel()
: Level_Rect{}, Next_Level_Number(0), Current_Level_Number(0), Teleport_Pos(0), Advertisement(0), Need_To_Stop_All_Activity(false), Active_Brick_Count(0), Mop(),
  Level_Table(55, 145)
  
{
	Level = this;
}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Check_Hit(double next_x_pos, double next_y_pos, ABall_Object *ball)
{// Корректируем позицию мячика при отражении от кирпичей

	int i, j;
	double direction;
	double min_ball_x, max_ball_x;
	double min_ball_y, max_ball_y;
	int min_level_x, max_level_x;
	int min_level_y, max_level_y;
	bool got_horizontal_hit, got_vertical_hit;
	double horizontal_reflection_pos, vertical_reflection_pos;

	if(ball->Get_State() == EBall_State::On_Paraschute)
		return false;

	if (next_y_pos + AsConfig::Ball_Radius > AsConfig::Level_Y_Offset + (AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height)
		return false;

	direction = ball->Get_Direction();

	min_ball_x = next_x_pos - AsConfig::Ball_Radius;
	max_ball_x = next_x_pos + AsConfig::Ball_Radius;
	min_ball_y = next_y_pos - AsConfig::Ball_Radius;
	max_ball_y = next_y_pos + AsConfig::Ball_Radius;

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
	Act_Objects(Active_Bricks);
	Act_Objects(Falling_Letters);

	if(Advertisement != 0)
		Advertisement->Act();

	Mop.Act();
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

	
		Draw_Objects(hdc, paint_area, Active_Bricks);
	}

	Draw_Objects(hdc, paint_area, Falling_Letters);

	Mop.Draw(hdc, paint_area);
	Level_Table.Draw(hdc, paint_area);
}
void ALevel::Clear(HDC hdc, RECT &paint_area)
{
	// 1. Очищаем движущиеся объекты
	Clear_Objects(hdc, paint_area, Falling_Letters);

	Mop.Clear(hdc, paint_area);
	Level_Table.Clear(hdc, paint_area);

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
	int i;
	ALevel_Data *level_data;

	Level_Rect.left = AsConfig::Level_X_Offset * AsConfig::Global_Scale;
	Level_Rect.top = AsConfig::Level_Y_Offset * AsConfig::Global_Scale;
	Level_Rect.right = Level_Rect.left + AsConfig::Cell_Width * AsConfig::Level_Width * AsConfig::Global_Scale;
	Level_Rect.bottom = Level_Rect.top + AsConfig::Cell_Height * AsConfig::Level_Height * AsConfig::Global_Scale;

	memset(Current_Level, 0, sizeof(Current_Level) );

	for(i = 0; i < ALevel_Data::Max_Level_Count; ++i)
	{
		level_data = new ALevel_Data(i + 1); 
		Levels_Data.push_back(level_data);

		if(i == 7)
			level_data->Advertisement = new AAdvertisement(4, 7, 2, 3);

		if(i == 9)
			level_data->Advertisement = new AAdvertisement(1, 9, 2, 3);
		
	}
		
	Mop.Erase_Level();
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Set_Current_Level(int level_index)
{
	int i,j;
	EBrick_Type brick_type;
	ALevel_Data *level_data;

	if(level_index < 1 || level_index > Levels_Data.size())
		AsConfig::Throw;

	level_data = Levels_Data[level_index - 1];

	memcpy(Current_Level, level_data->Level, sizeof(Current_Level) );
	
	Advertisement = level_data->Advertisement;

	// 1. Считаем телепорты (только для того, чтобы задать размер массива)
	for(i = 0; i < AsConfig::Level_Height; ++i)
	{
		for(j = 0; j < AsConfig::Level_Width; ++j)
		{
			brick_type = (EBrick_Type)ALevel::Current_Level[i][j];
			if(brick_type == EBrick_Type::Teleport)
				Teleport_Pos.emplace_back(j, i);
			
		}
		if(Teleport_Pos.size() > ALevel::Max_Teleports_In_Level)
			Teleport_Pos.resize(ALevel::Max_Teleports_In_Level);
	}
	if(Teleport_Pos.size() == 1)
		AsConfig::Throw();

	// 2. Записать телепорты в массив
	/*delete[] Teleport_Pos;
	Teleport_Pos = 0;*/

	//Teleport_Pos = new SPoint[Teleport_Counter];

	/*if(Teleport_Pos != 0)
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
	}*/
	//Advertisement = new AAdvertisement(9, 5, 2, 3);
	Current_Level_Number = level_index;
	Active_Brick_Count = level_data->Get_Avaliable_Bricks_Count();
}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Get_Next_Falling_Letter(int& index, AFalling_Letter **falling_letter)
{

	if(Falling_Letters.size() == 0)
		return false;

	if(index >= Falling_Letters.size())
		return false;

	//*falling_letter = (AFalling_Letter *)Falling_Letters[index];

	while(index < Falling_Letters.size())
	{
		*falling_letter = (AFalling_Letter *)Falling_Letters[index++]; 
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Stop()
{
	Need_To_Stop_All_Activity = true;
}
void ALevel::Mop_Level(int next_level)
{
	if(next_level < 0 || next_level >= ALevel_Data::Max_Level_Count)
		AsConfig::Throw();

	Next_Level_Number = next_level;

	Mop.Erase_Level();
}
void ALevel::Mop_Next_Level()
{
	if(! Can_Mop_Next_Level())
		AsConfig::Throw();

	Mop_Level(Current_Level_Number + 1);
}
bool ALevel::Can_Mop_Next_Level()
{
	if(Current_Level_Number + 1 >= (int)Levels_Data.size())
		return false;
	else
		return true;
}
void ALevel::Show_Level_Table()
{
	Level_Table.Show(Current_Level_Number);
}
void ALevel::Hide_Level_Table()
{
	Level_Table.Hide();
}
bool ALevel::Is_Mopping_Done()
{
	if(Mop.Get_State() == EMop_State::Idle)
		return true;
	else
		return false;
}
bool ALevel::Has_Brick_At(int brick_x, int brick_y)
{
	EBrick_Type brick_type;
	
	if(brick_x > AsConfig::Level_Width - 1 || brick_y > AsConfig::Level_Height - 1)
		return false;

	brick_type = (EBrick_Type)Level->Current_Level[brick_y][brick_x];
	
	if(brick_type != EBrick_Type::None)
		return true;
	else
		return false;

}
bool ALevel::Has_Brick_At(RECT &monster_rect)
{
	const int scale = AsConfig::Global_Scale;
	int i, j;
	int min_x_index, min_y_index;
	int max_x_index, max_y_index;
	int cell_width = AsConfig::Cell_Width * scale;
	int cell_height = AsConfig::Cell_Height * scale;
	int x_offset = AsConfig::Level_X_Offset * scale;
	int y_offset = AsConfig::Level_Y_Offset * scale;
	int max_cell_y, max_cell_x;
	int min_cell_y, min_cell_x;

	if(monster_rect.top > x_offset + ( (AsConfig::Level_Height - 1) * AsConfig::Cell_Height + AsConfig::Brick_Height)  * scale)
		return false;

	min_x_index = (monster_rect.left - x_offset) / cell_width;
	max_x_index = (monster_rect.right - x_offset) / cell_width;

	min_y_index = (monster_rect.top - y_offset) / cell_height;
	max_y_index = (monster_rect.bottom - y_offset) / cell_height;

	if(min_x_index >= AsConfig::Level_Width)
		min_x_index = AsConfig::Level_Width - 1;

	if(max_x_index >= AsConfig::Level_Width)
		max_x_index = AsConfig::Level_Width - 1;

	if(min_y_index >= AsConfig::Level_Height)
		min_y_index = AsConfig::Level_Height - 1;

	if(max_y_index >= AsConfig::Level_Height)
		max_y_index = AsConfig::Level_Height - 1;

	// Т.к. ячейка уровня больше кипича (хотя и начинается в одинаковых с кирпичом координатах),
	// то она имеет правую и нижнюю пустую полосу, в которой может находиться монстр.
	// Игнорируем ряд (или столбец) кирпичей, если монстр попал в ячейку, но не попал в кирпич.

	max_cell_y = max_y_index * cell_height + y_offset;
	max_cell_x = max_x_index * cell_width + x_offset;

	min_cell_y = min_y_index * cell_height + y_offset;
	min_cell_x = min_x_index * cell_width + x_offset;

	if(monster_rect.top > min_cell_y + AsConfig::Brick_Height * scale
		&& monster_rect.top < min_cell_y + cell_height)
		++min_y_index;
	if(monster_rect.left > min_cell_x + AsConfig::Brick_Width * scale
		&& monster_rect.left < min_cell_x + cell_width)
		++min_x_index;

	for(i = min_y_index; i <= max_y_index; ++i)
		for(j = min_x_index; j <= max_x_index; ++j)
			if(Has_Brick_At(j, i))
				return true;

	return false;

}
//------------------------------------------------------------------------------------------------------------
bool ALevel::On_Hit(int brick_x, int brick_y, ABall_Object *ball, bool got_vertical_hit)
{
	EBrick_Type brick_type;
	bool can_reflect = true;
	AMessage *message;

	brick_type = (EBrick_Type)Current_Level[brick_y][brick_x];

	
	if(brick_type == EBrick_Type::Paraschute)
	{ 
		if(ball != 0)
		{
			ball->Set_On_Paraschute(brick_x, brick_y);
			Current_Level[brick_y][brick_x] = (char)EBrick_Type::None;
			can_reflect = false;
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
		can_reflect = Create_Active_Brick(brick_x, brick_y, brick_type, ball, got_vertical_hit);
			
	Redraw_Brick(brick_x, brick_y);	


	AsInfo_Panel::Update_Score(EHit_Type::Hit_Brick);
	 
	if(Current_Level[brick_y][brick_x] == (char)EBrick_Type::None)	
		Active_Brick_Count--;
	
	if(Active_Brick_Count <= 0)
	{
		message = new AMessage(EMessage_Type::Level_Done);
		AsMessage_Manager::Add_Message(message);
	}

	return can_reflect;
} 
//------------------------------------------------------------------------------------------------------------
void ALevel::Clear_Objects(HDC hdc, RECT &paint_area, std::vector<AGraphics_Objects *> &object_for_drawing)
{
	for(auto *it : object_for_drawing)
			it->Clear(hdc, paint_area);
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
bool ALevel::Check_Vertical_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos)
{
	double direction = ball->Get_Direction();

	// Проверяем попадание в нижнюю грань
	if(ball->Is_Moving_Up())
	{
		if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Low_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, AsConfig::Ball_Radius, reflection_pos) )
		{
			// Проверяем возможность отскока вниз
			if (level_y == AsConfig::Level_Height - 1 || (level_y < AsConfig::Level_Height - 1 && Current_Level[level_y + 1][level_x] == 0))
				return true;
			else
				return false;
		}
	}
	else
	{// Проверяем попадание в верхнюю грань
	   if (Hit_Circle_On_Line(next_y_pos - Current_Brick_Top_Y, next_x_pos, Current_Brick_Left_X, Current_Brick_Right_X, AsConfig::Ball_Radius, reflection_pos) )
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
bool ALevel::Check_Horizontal_Hit(double next_x_pos, double next_y_pos, int level_x, int level_y, ABall_Object *ball, double &reflection_pos)
{
	double direction = ball->Get_Direction();

	// Проверяем попадание в левую грань
	if (! ball->Is_Moving_Left())
	{
		if (Hit_Circle_On_Line(Current_Brick_Left_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, AsConfig::Ball_Radius, reflection_pos) )
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
		if (Hit_Circle_On_Line(Current_Brick_Right_X - next_x_pos, next_y_pos, Current_Brick_Top_Y, Current_Brick_Low_Y, AsConfig::Ball_Radius, reflection_pos) )
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
	ELetter_Type letter_type;
	AFalling_Letter *falling_letter;
	int letter_x, letter_y;
	if( !(brick_type == EBrick_Type::Blue || brick_type == EBrick_Type::Red) )
		return false;
	
	if(AsCommon::Rand(AsConfig::Max_Random_Number_For_Letter) != 0) // рандом: или буква или нет
		return false;
		
	if(Falling_Letters.size() >= AsConfig::Max_Falling_Letters_Size - 1) // падающих букв слишком много
		return false;
			

	letter_x = (AsConfig::Level_X_Offset + brick_x * AsConfig::Cell_Width) * AsConfig::Global_Scale;
	letter_y = (AsConfig::Level_Y_Offset + brick_y * AsConfig::Cell_Height) * AsConfig::Global_Scale;

	letter_type = AFalling_Letter::Get_Random_Letter_Type();

	switch(AsCommon::Rand(4))
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
	case 3:
		letter_type = ELetter_Type::O;
		break;
	}
	/*if(AsCommon::Rand(2) == 0)
		letter_type = ELetter_Type::O;
	else
		letter_type = ELetter_Type::M;*/

	//letter_type = ELetter_Type::O;

	falling_letter = new AFalling_Letter (letter_type, letter_x, letter_y, brick_type);
	Falling_Letters.push_back(falling_letter);

	return true;

}
//------------------------------------------------------------------------------------------------------------
bool ALevel::Create_Active_Brick(int brick_x, int brick_y, EBrick_Type brick_type, ABall_Object *ball, bool got_vertical_hit)
{ // brick_x, brick_у - номер позиции в двухмерном массиве конкретного кирпича 
	bool is_teleport = false;

	AActive_Brick *active_brick = 0;
	AActive_Brick_Teleport *destination_teleport = 0;

	if(Active_Bricks.size() > AsConfig::Max_Active_Brick_Size - 1)
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
		Active_Bricks.push_back(active_brick);
	
  return is_teleport;
}

//------------------------------------------------------------------------------------------------------------
void ALevel::Add_Active_Brick_Teleport(int brick_x, int brick_y, ABall_Object *ball, bool got_vertical_hit)
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

	Active_Bricks.push_back(source_teleport);
	Active_Bricks.push_back(destination_teleport);
}
//------------------------------------------------------------------------------------------------------------
AActive_Brick_Teleport* ALevel::Select_Destination_Teleport(int brick_x, int brick_y)
{
	AActive_Brick_Teleport *destination_teleport = 0;
	int another_brick_x, another_brick_y;
	int rand_index;
	while(1) // рандомный выбор телепорта
	{
		rand_index = AsCommon::Rand(Teleport_Pos.size());

		if( !(Teleport_Pos[rand_index].X == brick_x && Teleport_Pos[rand_index].Y == brick_y))
	        break;
	}
	another_brick_x = Teleport_Pos[rand_index].X;
	another_brick_y = Teleport_Pos[rand_index].Y;

	destination_teleport = new AActive_Brick_Teleport(another_brick_x, another_brick_y, 0, 0);

	return destination_teleport;
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Act_Objects(std::vector<AGraphics_Objects *> &object_for_drawing)
{
	auto it = object_for_drawing.begin();
	while(it != object_for_drawing.end())
	{
		 (*it)->Act();
		  
		 if((*it)->Is_Finished())
		 {
		  	delete *it;
		  	it = object_for_drawing.erase(it);
		 }
		 else
			 it++;
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
void ALevel::Draw_Objects(HDC hdc, RECT &paint_area, std::vector<AGraphics_Objects *> &object_for_drawing)
{
	for(auto *obj : object_for_drawing)
			obj->Draw(hdc, paint_area);
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
	Delete_Objects(Active_Bricks);
	Delete_Objects(Falling_Letters);

	Need_To_Stop_All_Activity = false;
}
//------------------------------------------------------------------------------------------------------------
void ALevel::Delete_Objects(std::vector<AGraphics_Objects *> &object_for_drawing)
{
	auto it = object_for_drawing.begin();
    while (it != object_for_drawing.end())
    {
        delete *it;  // Удаляем объект
        it = object_for_drawing.erase(it);  // erase возвращает итератор на следующий элемент
    }
		
}

//------------------------------------------------------------------------------------------------------------