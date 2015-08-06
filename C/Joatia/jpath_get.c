
int retrievePath(int sx, int gx, int sy, int gy)
{
	#define RIGHT_MOVEMENT 0
	#define  LEFT_MOVEMENT 1
	#define    UP_MOVEMENT 2
	#define  DOWN_MOVEMENT 3
	#define  UP_RIGHT_MOVE 4
	#define   UP_LEFT_MOVE 5
	#define DWN_RIGHT_MOVE 6
	#define  DWN_LEFT_MOVE 7

	if(sx < gx)
	{
		if (sy > gy)
		{
			return(UP_RIGHT_MOVE);
		}
		else if (sy < gy)
		{
			return(DWN_RIGHT_MOVE);
		}
		else
		{
			return(RIGHT_MOVEMENT);
		}
	}
	else if(sx > gx)
	{

		if(sy > gy)
		{
			return(UP_LEFT_MOVE);
		}
		else if (sy > gy)
		{
			return(UP_RIGHT_MOVE);
		}
		else
		{
			return(LEFT_MOVEMENT);
		}
	}
	else if(sy > gy)
	{
		return(UP_MOVEMENT);
	}
	else if(sy < gy)
	{
		return(DOWN_MOVEMENT);
	}
	else
	{
		return(-1);
	}
}
