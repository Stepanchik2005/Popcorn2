#pragma once
#include "Tools.h"

class AHit_Checker_List
{
public:
	bool Add_Hit_Checker(AHit_Checker *hit_checker);
	bool Check_Hit(double x_pos, double y_pos);
	bool Check_Hit(double x_pos, double y_pos, ABall_Object* ball);
	bool Check_Hit(RECT &rect);
private:

	AHit_Checker *Hit_Checkers[4]; 
	int Hit_Checkers_Count;
};