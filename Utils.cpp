#include "Utils.h"
#include "ardu_main.h"

#ifdef _DEBUG
char * DirToText(uint8_t dir)
{
	switch(dir)
	{
	case LEFT_BUTTON: return "left";
	case RIGHT_BUTTON: return "right";
	case UP_BUTTON: return "up";
	case DOWN_BUTTON: return "down";
	}
	return "error";
}
#endif

void MovePosition(float *pXInOut, float *pYInOut, uint8_t dir, float amount)
{
	if (dir & RIGHT_BUTTON)
	{
		(*pXInOut)+=amount;
		return;
	}
	
	if (dir & LEFT_BUTTON)
	{
		(*pXInOut)-=amount;
		return;
	}

	if (dir & UP_BUTTON)
	{
		(*pYInOut)-=amount;
		return;
	}
	
	if (dir & DOWN_BUTTON)
	{
		(*pYInOut)+=amount;
		return;
	}
}

uint8_t getPixelSafe(int16_t x, int16_t y)
{
	if (y > HEIGHT-1 || y < 0) return false;
	return arduboy.getPixel(x,y);
}

bool HitWall(int16_t x, int16_t y)
{
	//are we inside a wall right now?
	//right
	if (getPixelSafe(x+PLAY_W, y-1)
		||getPixelSafe(x+PLAY_W, y+PLAY_H))
	{
		return true;
	}

	//left
	if (getPixelSafe(x-1, y-1) 
		||getPixelSafe(x-1, y+PLAY_H) )
	{
		return true;
	}

	//up
	if (getPixelSafe(x-1, y-1)
		|| getPixelSafe(x+PLAY_W, y-1))
	{
		return true;
	}

	//down
	if (getPixelSafe(x-1, y+PLAY_H)
		|| getPixelSafe(x+PLAY_W, y+PLAY_H))
	{
		return true;
	}

	return false;
}

bool CanGoRight(int16_t x, int16_t y)
{
	if (!getPixelSafe(x+PLAY_W+1, y-1) 
		&& !getPixelSafe(x+PLAY_W+1, y+PLAY_H)
		)
	{
		return true;
	}

	return false;
}

bool CanGoUp(int16_t x, int16_t y)
{
	if (!getPixelSafe(x-1, y-2)
		&& !getPixelSafe(x+PLAY_W, y-2)
		)
	{
		return true;
	}
	return false;
}

bool CanGoLeft(int16_t x, int16_t y)
{
	if (!getPixelSafe(x-2, y-1) 
		&& !getPixelSafe(x-2, y+PLAY_H)
		)
	{
		return true;
	}

	return false;
}

bool CanGoDown(int16_t x, int16_t y)
{
	if (!getPixelSafe(x-1, y+PLAY_H+1)
		&& !getPixelSafe(x+PLAY_W, y+PLAY_H+1)
		)
	{
		return true;
	}
	return false;
}

bool CanTurn(int16_t x, int16_t y, uint8_t dir)
{
	if (dir == LEFT_BUTTON || dir == RIGHT_BUTTON)
	{
		return CanGoUp(x,y)	||CanGoDown(x,y);
	}

	return CanGoLeft(x,y)	||CanGoRight(x,y);
}

bool IsInTunnel(int16_t x, int16_t y)
{
	if (x == TUNNEL_X)
	{
		if (y < 12 || y > 48) return true;
	}

	return false;
}
float EntitiesAreTouching(float ax, float ay, float bx, float by, float width )
{
	ax -= bx;
	ay -= by;
	if (fabs(ax) <= width && fabs(ay) <= width) return true;
	return false;
}

void WorldWrapY(float &yInOut)
{
	if (yInOut > 63) yInOut = -2;
	if (yInOut < -3) yInOut = 62;
}

bool GetFoodWeAreOn(uint8_t *pFoodOutX,uint8_t *pFoodOutY, int16_t posX, int16_t posY)
{

	if (posY > 1 && posY < 64)
	{

		for (int x=0; x < 3; x++)
		{
			for (int y=0; y < 3; y++)
			{
				if (getPixelSafe(posX+x, posY+y))
				{
					*pFoodOutX = posX+x;
					*pFoodOutY = posY+y;
					return true;
				}
			}
		}
	}

	return false;
}

//if vertical flag is sent, it isn't centered
size_t printNumberFancy(uint8_t x, uint8_t y, long n, bool bCentered, bool bVertical)
{
	char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
	char *str = &buf[sizeof(buf) - 1];
	*str = '\0';
	uint8_t count = 0;
	do {
		count++;
		char c = n % 10;
		n /= 10;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	} while(n);

	
	if (bVertical)
	{
	    while (str[0])
		{
			text.setCursor(x, y);
			y += 8;
			text.print(str[0]);
			str++;
		}
		return count;
	} else
	{
		if (bCentered)
		{
			text.setCursor(x-((count*6)/2), y);
		} else
		{
			text.setCursor(x, y);
		}
		return text.print(str);
	}
	
}

