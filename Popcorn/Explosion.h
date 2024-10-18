#pragma once
#include "Config.h"
#include "Explodive_Balls.h"
class AsExplosion
{
public:
	AsExplosion();

	void Activate_Explosion(RECT &area_rect);
	void Draw_Explosion(HDC hdc, RECT &paint_area);
	void Act_Explosion(int &count);

	static const int Explodive_Balls_Count = 10;
private:

	std::vector<AExplodive_Ball> Explodive_Balls;

	
};