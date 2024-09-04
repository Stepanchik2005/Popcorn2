#include "Hit_Checker_List.h"

bool AHit_Checker_List::Add_Hit_Checker(AHit_Checker *hit_checker)
{
   if (Hit_Checkers_Count >= sizeof(Hit_Checkers) / sizeof(Hit_Checkers[0]) )
		return false;

	Hit_Checkers[Hit_Checkers_Count++] = hit_checker;

	return true;
}

bool AHit_Checker_List::Check_Hit(double x_pos, double y_pos)
{
	bool got_hit = false;
	int i;

	 for (i = 0; i < Hit_Checkers_Count; i++)
			got_hit |= Hit_Checkers[i]->Check_Hit(x_pos, y_pos);

	 return got_hit;
}
bool AHit_Checker_List::Check_Hit(RECT &rect)
{
	bool got_hit = false;
	int i;

	 for (i = 0; i < Hit_Checkers_Count; i++)
			got_hit |= Hit_Checkers[i]->Check_Hit(rect);

	 return got_hit;
}


bool AHit_Checker_List::Check_Hit(double x_pos, double y_pos, ABall_Object* ball)
{
	bool got_hit = false;
	int i;

	for (i = 0; i < Hit_Checkers_Count; i++)
			got_hit |= Hit_Checkers[i]->Check_Hit(x_pos, y_pos, ball); 

	 return got_hit;
}