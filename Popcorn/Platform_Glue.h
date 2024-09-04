#pragma once
#include "Platform_State.h"
#include "Ball_Set.h"

//------------------------------------------------------------------------------------------------------------

class AsPlatform_Glue
{
public:
	AsPlatform_Glue(AsPlatform_State &platform_state);


	bool Act(EPlatform_Transformation &glue_state, AsBall_Set *ball_set, EPlatform_State &next_state);
	void Draw_Glue_State(HDC hdc, double x_pos);
	void Draw_Spot_State(HDC hdc, int x_offset, int width, int height, double x_pos);
	void Reset();

private:
	double Platform_Glue_Ratio;
	AsPlatform_State *Platform_State;

	static const double Max_Glue_Ratio, Min_Glue_Ratio, Glue_Spot_Height_Ratio_Step;
};
