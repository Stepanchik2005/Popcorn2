#include "Ball_Set.h"
void AsBall_Set::Advance(double max_speed)
{
	int i;
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
		Balls[i].Advance(max_speed);
}
double AsBall_Set::Get_Speed()
{
	int i;
	double max_speed = 0.0;

	for(i = 0; i < AsConfig::Max_Balls_Count;++i)
	{
		if(Balls[i].Get_Speed() > max_speed)
			max_speed = Balls[i].Get_Speed();
	}

	return max_speed;
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Start_Movement()
{
	int i;
	for(i = 0; i < AsConfig::Max_Balls_Count;++i)
		Balls[i].Start_Movement();
	
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::End_Movement()
{
	int i;
	for(i = 0; i < AsConfig::Max_Balls_Count;++i)
		Balls[i].End_Movement();
}
 //------------------------------------------------------------------------------------------------------------
void AsBall_Set::Act()
{
	int i;
	ABall *curr_ball;
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
	{
		curr_ball = &Balls[i];

		if(curr_ball->Get_State() == EBS_On_Platform)
			if(curr_ball->Release_Timer_Tick != 0 && AsConfig::Current_Timer_Tick >= curr_ball->Release_Timer_Tick)
				curr_ball->Release();
	}
}
bool AsBall_Set::Is_Finished()
{
	return false;
}
void AsBall_Set::Draw(HDC hdc, RECT &paint_area)
{
	int i;
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
		Balls[i].Draw(hdc, paint_area);
}
void AsBall_Set::Clear(HDC hdc, RECT &paint_area)
{
	int i;
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
		Balls[i].Clear(hdc, paint_area);
}
 //------------------------------------------------------------------------------------------------------------
void AsBall_Set::Release_From_Platform(double ball_mid_pos)
{
	int i;
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
		if(Balls[i].Get_State() == EBS_On_Platform)
			Balls[i].Set_State(EBS_Normal, ball_mid_pos , ABall::Start_Ball_Y_Pos);
}
//------------------------------------------------------------------------------------------------------------
bool AsBall_Set::Release_Next_Ball()
{
	int i;
	double ball_x, ball_y;
	ABall *curr_ball;
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
	{
		curr_ball = &Balls[i];

		if(curr_ball->Get_State() == EBS_On_Platform)
		{  
			curr_ball->Release();
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::On_Platform_Advance(double direction, double speed, double max_speed)
{
	int i;
	ABall *curr_ball;
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
	{
		curr_ball = &Balls[i];
		
		if(curr_ball->Get_State() == EBS_On_Platform)
			curr_ball->Forced_Advance(direction, speed, max_speed);
		
	}
		
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Set_On_Platform(double ball_mid_pos)
{
	int i;
	for(i = 0; i < 4; ++i)
	{
		Balls[i].Set_State(EBS_Normal);
		Balls[i].Set_State(EBS_On_Platform, ball_mid_pos, ABall::Start_Ball_Y_Pos);
	}

   for(; i < AsConfig::Max_Balls_Count; ++i)
		Balls[i].Set_State(EBS_Disabled);
}
//------------------------------------------------------------------------------------------------------------
int AsBall_Set::Count_Lost_Ball()
{
	int i, lost_ball_count = 0;
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
		if (Balls[i].Get_State() == EBS_Lost || Balls[i].Get_State() == EBS_Disabled)
			++lost_ball_count;

	return lost_ball_count;
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Set_For_Test()
{
	Balls[0].Set_For_Test();
}
//------------------------------------------------------------------------------------------------------------
bool AsBall_Set::Is_Test_Finished()
{
	if(Balls[0].Is_Test_Finished())
		return true;
	else
		return false;
}
void AsBall_Set::Triple_Balls()
{
	int i;
	// 1. Найдем самый дальний мячик

	ABall *curr_ball = 0;
	ABall *further_ball = 0;
	ABall *left_ball = 0, *right_ball = 0;
	double curr_ball_y,curr_ball_x;
	double further_ball_y,further_ball_x;
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
	{
		curr_ball = &Balls[i];

		curr_ball->Get_Center(curr_ball_x,curr_ball_y);
		
		if(further_ball == 0)
			further_ball = &Balls[i];

		further_ball->Get_Center(further_ball_x, further_ball_y);

		if(curr_ball_y <= further_ball_y)
			further_ball = curr_ball;
	}
	
	// 2. Если нормальный мячик есть, размножаем его
	if(further_ball == 0)
		return;

	// 3. Найдем кандидатов
	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
	{
		curr_ball = &Balls[i];

		if(curr_ball->Get_State() == EBS_Disabled ||curr_ball->Get_State() == EBS_Lost)
		{
			if(left_ball == 0)
			{
				left_ball = &Balls[i];
			   continue;
			}
			if(right_ball == 0)
			{
				right_ball = &Balls[i];
				continue;	
			}
			if(left_ball != 0 && right_ball != 0)
				break;
			
		}
	}
	// 4 Установление кандидатов
	if(left_ball != 0)
	{
		*left_ball = *further_ball;
		left_ball->Set_Direction(further_ball->Get_Direction() + M_PI_4 / 2.0);
	}

	if(right_ball != 0)
	{
		*right_ball = *further_ball;
		right_ball->Set_Direction(further_ball->Get_Direction() - M_PI_4 / 2.0);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Inverse_Balls()
{//Меняем направление мячика на противоположное
	int i;
	ABall *curr_ball = 0;

	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
	{
		curr_ball = &Balls[i];
		
		if(curr_ball->Get_State() == EBS_Normal)
			curr_ball->Set_Direction(curr_ball->Get_Direction() + M_PI);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Accelerate()
{ // Ускоряем все мячики на константу
	int i;
	ABall *curr_ball = 0;

	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
	{
		curr_ball = &Balls[i];
		
		if(curr_ball->Get_State() == EBS_Normal)
			curr_ball->Set_Speed(curr_ball->Get_Speed() * AsConfig::Accelerate_Ball_Speed);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Reset_Speed()
{
	int i;
	ABall *curr_ball = 0;

	for(i = 0; i < AsConfig::Max_Balls_Count; ++i)
	{
		curr_ball = &Balls[i];
		
		if(curr_ball->Get_State() == EBS_Normal)
			curr_ball->Set_Speed(AsConfig::Initial_Ball_Speed);
	}
}