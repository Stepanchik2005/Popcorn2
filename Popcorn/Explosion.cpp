#include "Explosion.h"
AsExplosion::AsExplosion()
	: Explodive_Balls(Explodive_Balls_Count)
{
}

void AsExplosion::Activate_Explosion(RECT &area_rect)
{
	int rest_size;
	int x_offset, y_offset;
	int max_size, half_size;
	int time_offset;
	bool is_red;

	int half_width = (area_rect.right - area_rect.left) / 2;
	int half_height = (area_rect.bottom - area_rect.top) / 2;
	int x_pos = area_rect.left + half_width; // координаты центра глаза (эллипса)
	int y_pos = area_rect.top + half_height;
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

		rest_size = half_size - (int)(sqrt( pow(x_offset, 2) + pow(y_offset, 2)));

		max_size = AsCommon::Rand(rest_size / 2) + rest_size / 2;

		if(max_size < AsConfig::Global_Scale)
			max_size = AsConfig::Global_Scale;

		is_red = AsCommon::Rand(2);

		ball.Explode(x_pos + x_offset, y_pos + y_offset, time_offset, max_size * 2, 10, is_red);

	}
}
void AsExplosion::Draw_Explosion(HDC hdc, RECT &paint_area)
{
	for(auto &ball : Explodive_Balls)
		ball.Draw(hdc, paint_area);
}
void AsExplosion::Act_Explosion(int &count)
{
	for(auto &ball : Explodive_Balls)
	{
		ball.Act();

		if(ball.Explodive_Ball_State == EExplodive_Ball_State::Idle)
			count++;
	}
}