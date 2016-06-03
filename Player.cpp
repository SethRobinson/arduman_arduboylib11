#include "Player.h"
#include "Utils.h"
#include "Ghost.h"
#include "Music.h"
#include "ardu_main.h"
#include "Highscores.h"
#include "Fruit.h"

CrumbManager crumbManager;

Player::Player()
{
	ResetGame();
}

void Player::ResetGame()
{
	m_livesLeft = LIVES_COUNT;
	m_score = 0;
	m_bGotExtraLife = false;
	ResetLevel();
}
void Player::ResetLevel()
{
	m_noAnimate = false;
	m_pelletsEaten = 0;

	fruit.ResetForNewLevel();

	m_pills[0].Setup(3,3);
	m_pills[1].Setup(3,60);
	m_pills[2].Setup(105,3);
	m_pills[3].Setup(105,60);


	ResetStatus();
}

void Player::ResetStatus()
{
	m_powerTimer = 0;
	crumbManager.Reset();
	m_wasHurt = false;
	m_speed = 0.5;
	m_x = 53;
	m_y = 44;
	m_slowTimer = 0;
	m_ghostsKilled = 0;
	m_curDir = m_inputState = 0; //0 means no direction
	m_sfxTimer = 0;
}

void Player::OnPowerPillGet()
{
	m_ghostsKilled = 0;
	m_powerTimer = millis()+ max(1000, (C_POWER_PILL_TIME_MS-((g_level-1)*1000)));
	AddScore(POWER_PELLET_POINTS);
}

bool Player::PowerIsActive()
{
#ifdef CHEAT_ALWAYS_HAVE_POWER_PILL
return true;
#endif
	return (m_powerTimer > millis());
}
long Player::GetPowerTimeLeft()
{
#ifdef CHEAT_ALWAYS_HAVE_POWER_PILL
	return 2000;
#endif

	return m_powerTimer-millis();
}

bool Player::PowerIsActiveDisplay()
{
#ifdef CHEAT_ALWAYS_HAVE_POWER_PILL
	return true;
#endif

	//so ghosts stop blinking before the power is actually gone
	if (m_powerTimer < POWER_WEAR_OFF_WARNING_MS) return false;
	return (m_powerTimer-POWER_WEAR_OFF_WARNING_MS > millis());
}
void Player::OnKilledGhost()
{
	m_ghostsKilled++;
	AddScore(200*m_ghostsKilled);
	m_powerTimer += GHOST_KILL_DELAY_MS; //don't want the kill to waste power up time
}

void Player::AddScore(long score)
{
	m_score += score;
	if (!m_bGotExtraLife && m_score >= 10000)
	{
		m_livesLeft++;
		m_bGotExtraLife = true;
	}
}

void Player::Update()
{
	if (m_wasHurt) return;

	uint8_t input = arduboy.getInput();

	if (PowerIsActive())
	{
		if (m_sfxTimer < millis())
		{
#ifdef RT_ARDUDEV
			GetAudioManager()->Play("audio/power_tick.wav");
#else
			tunes.tone(800, 30);
#endif
			m_sfxTimer = millis()+300;
		}
	}

	if (input != 0)
		m_inputState = input;

	//get new position
	float x = m_x;
	float y = m_y;

	if (m_inputState != m_curDir)
	{
		if (m_inputState & RIGHT_BUTTON && CanGoRight(m_x, m_y))
		{
			m_curDir = RIGHT_BUTTON;
		}
		if (m_inputState & UP_BUTTON && CanGoUp(m_x, m_y))
		{
			m_curDir = UP_BUTTON;
		}
		if (m_inputState & LEFT_BUTTON && CanGoLeft(m_x, m_y))
		{
			m_curDir = LEFT_BUTTON;
		}
		if (m_inputState & DOWN_BUTTON && CanGoDown(m_x, m_y))
		{
			m_curDir = DOWN_BUTTON;
		}
	
	}

	if (m_slowTimer > millis())
	{
		//they just ate something, move slower
		m_speed = PLAYER_SLOW_SPEED;
	} else
	{
		m_speed = PLAYER_FAST_SPEED;
	}

	crumbManager.AddCrumb(m_x, m_y, m_curDir);

	MovePosition(&x,&y, m_curDir, m_speed);

	if (HitWall(x, y))
	{
		m_curDir = m_inputState;
		m_noAnimate = true;
		return;
	}

	//valid move
	m_x = x;
	m_y = y;

	WorldWrapY(m_y);

		uint8_t foodX,foodY;
		if (GetFoodWeAreOn(&foodX, &foodY, m_x, m_y))
		{
#ifdef RT_ARDUDEV
			GetAudioManager()->Play("audio/get_pellet.wav");
#else
			tunes.tone(523, 20);
#endif
			m_slowTimer = millis()+200;
			AddScore(PELLET_POINTS);
			m_pelletsEaten++;
			fruit.OnPelletEaten();
			
		}
	
	//check for power pills
	for (int i=0; i < POWER_PILL_COUNT; i++)
	{
		if (m_pills[i].m_active && EntitiesAreTouching(m_x, m_y, m_pills[i].m_x, m_pills[i].m_y, 2))
		{
			m_pills[i].m_active = false;
			OnPowerPillGet();
		}
	}

	
	crumbManager.AddCrumb(m_x, m_y, m_curDir);
}

bool Player::LevelPassed()
{
	return 	(m_pelletsEaten >= TOTAL_PELLETS);
}

void Player::Blit(bool color)
{

		if (m_noAnimate || m_curDir == 0 || (millis()%500) > 250)
		{
			m_noAnimate = false;
			
			switch (m_curDir)
			{
			case LEFT_BUTTON:
					pBitmap = player_left_open;
				break;
			case UP_BUTTON:
				pBitmap = player_up_open;
				break;
			case DOWN_BUTTON:
				pBitmap = player_down_open;
				break;
			default:
				//facing right, the default
				pBitmap = player_right_open;
				break;
			}
		} else
		{
			pBitmap = player_right_closed;
		}

		arduboy.drawBitmap(m_x, m_y, pBitmap, PLAY_W,PLAY_H, color);
}

void Player::UnRender()
{
	
	arduboy.drawBitmap(m_x, m_y, solid_box, PLAY_W,PLAY_H, 0);
	for (int i=0; i < POWER_PILL_COUNT; i++)
	{
		m_pills[i].UnRender();
	}
}

void Player::Render()
{
	
	Blit(1);
	for (int i=0; i < POWER_PILL_COUNT; i++)
	{
		m_pills[i].Render();
	}


	//info overlays

	arduboy.fillRect(121, 0, WIDTH-121, HEIGHT-0, 0); //blank the right strip (where score goes)
	arduboy.fillRect(109, 55, WIDTH-109, HEIGHT-55, 0); //blank the bottom right strip (where lives go)

	//debug
#ifdef SHOW_PELLET_COUNT
	printNumberFancy(110, 40, m_pelletsEaten, false, false);
#endif

	printNumberFancy(121, 1, m_score, false, true);

	//draw lives left
	
	int x = 110;
	for (int i=0; i < min(5, m_livesLeft-1); i++)
	{
		arduboy.drawBitmap(x, 58 , player_right_open, PLAY_W,PLAY_H, 1);
		x += 4; //spacer
	}
}

void Player::OnHurt()
{
	m_wasHurt = true;
	DontDrawPelletHere(m_x, m_y);
}

void Player::Melt(uint16_t y)
{
	delay(300);
	arduboy.drawLine(m_x, m_y+y, m_x+3, m_y+y, 0);
	arduboy.display();
}
void Player::OnEndOfLoop()
{
	if (m_wasHurt)
	{
#ifdef RT_ARDUDEV
		GetAudioManager()->Play("audio/die.wav");
#else
		tunes.playScore(music_death);
#endif
		m_livesLeft--;
		fruit.OnPlayerDie();
		
		//melt "effect"
		Melt(0);
		Melt(1);
		Melt(2);
		
		if (m_livesLeft == 0)
		{
			delay(0);
			DrawScore();
			text.setCursor(30, 35);
			text.print("GAME OVER");
			arduboy.display();
			delay(2000);
			
			g_mode = MODE_MENU;

			if (m_score > 0)
			{
				if (enterHighScore(EEPROM_HIGH_SCORE_SAVE_SLOT))
				{
					//jump directly to the highscore screen, since they are on it
					g_mode = MODE_HIGHSCORES;
				}

				tunes.tone(523, 20);
				delay(300);
			} 
			
			return;
		}
		
		delay(500);
		UnRender(); //remove pellet if applicable
		ResetStatus();
		ResetGhostPositions(false);
	}
}