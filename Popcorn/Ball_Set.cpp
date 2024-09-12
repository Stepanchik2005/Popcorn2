#include "Ball_Set.h"
const double AsBall_Set::Min_Angle_For_Triple = M_PI_4 / 2.0;

AsBall_Set::AsBall_Set()
	: Balls(AsConfig::Max_Balls_Count)
{
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Act()
{
	for(auto &ball : Balls)
	{
		if(ball.Get_State() == EBall_State::On_Platform)
			if(ball.Release_Timer_Tick != 0 && AsConfig::Current_Timer_Tick >= ball.Release_Timer_Tick)
				ball.Release();
	}
}
 //------------------------------------------------------------------------------------------------------------
void AsBall_Set::Release_From_Platform(double ball_mid_pos)
{
	for(auto &ball : Balls)
		if(ball.Get_State() == EBall_State::On_Platform)
			ball.Set_State(EBall_State::Normal, ball_mid_pos , ABall::Start_Ball_Y_Pos);
}
//------------------------------------------------------------------------------------------------------------
bool AsBall_Set::Release_Next_Ball()
{
	for(auto &ball : Balls)
	{
		if(ball.Get_State() == EBall_State::On_Platform)
		{  
			ball.Release();
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::On_Platform_Advance(double direction, double speed, double max_speed)
{
	for(auto &ball : Balls)
	{
		if(ball.Get_State() == EBall_State::On_Platform)
			ball.Forced_Advance(direction, speed, max_speed);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Set_On_Platform(double ball_mid_pos)
{
	int i;
	for(i = 0; i < 1; ++i)
	{
		Balls[i].Set_State(EBall_State::Normal);
		Balls[i].Set_State(EBall_State::On_Platform, ball_mid_pos, ABall::Start_Ball_Y_Pos);
	}

   for(; i < Balls.size(); ++i)
		Balls[i].Set_State(EBall_State::Disabled);
}
//------------------------------------------------------------------------------------------------------------
int AsBall_Set::Count_Lost_Ball()
{
	int lost_ball_count = 0;
	for(auto &ball : Balls)
		if (ball.Get_State() == EBall_State::Lost || ball.Get_State() == EBall_State::Disabled)
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
	// 1. Найдем самый дальний мячик
	ABall *further_ball = 0;
	ABall *left_ball = 0, *right_ball = 0;
	double curr_ball_y,curr_ball_x;
	double further_ball_y,further_ball_x;
	
	for(auto &ball : Balls)
	{
		//curr_ball = &Balls[i];

		ball.Get_Center(curr_ball_x,curr_ball_y);
		
		if(further_ball == 0)
			further_ball = &ball;

		further_ball->Get_Center(further_ball_x, further_ball_y);

		if(curr_ball_y <= further_ball_y)
			further_ball = &ball;
	}
	
	// 2. Если нормальный мячик есть, размножаем его
	if(further_ball == 0)
		return;

	// 3. Найдем кандидатов
	for(auto &ball : Balls)
	{
		//curr_ball = &Balls[i];

		if(ball.Get_State() == EBall_State::Disabled || ball.Get_State() == EBall_State::Lost)
		{
			if(left_ball == 0)
			{
				left_ball = &ball;
			   continue;
			}
			if(right_ball == 0)
			{
				right_ball = &ball;
				continue;	
			}
			if(left_ball != 0 && right_ball != 0)
				break;
			
		}
	}

	// 4 Установка кандидатов
	if(left_ball != 0)
	{
		*left_ball = *further_ball;
		Correct_Tripled_Balls(left_ball, false);
	}

	if(right_ball != 0)
	{
		*right_ball = *further_ball;
		Correct_Tripled_Balls(right_ball, true);
	}
}
void AsBall_Set::Correct_Tripled_Balls(ABall *ball, bool is_right_ball)
{
	double prev_direction, direction_delta;
	double angle = Min_Angle_For_Triple;
	double new_direction;

	if(is_right_ball)
	{
		angle = -angle;
		new_direction = Min_Angle_For_Triple / 2.0;
	}
	else
		new_direction = -Min_Angle_For_Triple / 2.0;

	prev_direction = ball->Get_Direction();

	ball->Set_Direction(ball->Get_Direction() + angle);

	direction_delta = ball->Get_Direction() - prev_direction;

	if(direction_delta < Min_Angle_For_Triple / 2.0)
		ball->Set_Direction(prev_direction + new_direction);
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Inverse_Balls()
{//Меняем направление мячика на противоположное
	for(auto &ball : Balls)
	{
		if(ball.Get_State() == EBall_State::Normal)
			ball.Set_Direction(ball.Get_Direction() + M_PI);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Accelerate()
{ // Ускоряем все мячики на константу
	for(auto &ball : Balls)
	{
		if(ball.Get_State() == EBall_State::Normal)
			ball.Set_Speed(ball.Get_Speed() * AsConfig::Accelerate_Ball_Speed);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsBall_Set::Reset_Speed()
{
	for(auto &ball : Balls)
	{
		if(ball.Get_State() == EBall_State::Normal)
			ball.Set_Speed(AsConfig::Initial_Ball_Speed);
	}
}

bool AsBall_Set::Get_Next_Game_Object(int &index, AGame_Object **obj)
{
   if(index >= 0 && index < Balls.size())
   {
      *obj = &Balls[index++]; // увеличиваем индекс после присваивания 
      return true;
   }
   else
      return false;
}