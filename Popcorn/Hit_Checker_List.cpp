#include "Hit_Checker_List.h"

void AHit_Checker_List::Add_Hit_Checker(AHit_Checker *hit_checker)
{
	Hit_Checkers.push_back(hit_checker);
}

bool AHit_Checker_List::Check_Hit(double x_pos, double y_pos)
{
	bool got_hit = false;

	for (auto *it : Hit_Checkers)
		got_hit |= it->Check_Hit(x_pos, y_pos);

	return got_hit;
}
bool AHit_Checker_List::Check_Hit(RECT &rect)
{
	bool got_hit = false;

	for (auto *it : Hit_Checkers)
		got_hit |= it->Check_Hit(rect);

	return got_hit;
}


bool AHit_Checker_List::Check_Hit(double x_pos, double y_pos, ABall_Object* ball)
{
	bool got_hit = false;

	for (auto *it : Hit_Checkers)
		got_hit |= it->Check_Hit(x_pos, y_pos, ball); 

   return got_hit;
}