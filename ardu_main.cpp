//Arduman by Seth A. Robinson (rtsoft.com)

//Thanks to: http://fuopy.github.io/arduboy-image-converter/


#include <Arduboy.h>
#include "ConvertToArduboy11.h"

//#include <ArduboyPlaytune.h>
#include "level_bitmaps.h"
#include "Player.h"
#include "Ghost.h"
#include "Music.h"
#include "ardu_main.h"
#include "Highscores.h"
#include "Fruit.h"
//function declarations added by Seth because we're a real .cpp file now.  Also changed loop() to main_loop and setup() to
//main_setup(), as our simple Arduboy.ino file calls them

//Highscore code taken from the Breakout example (by nootropic design?)
//Music found from:
//Music converted using miditones

//***********************


Arduboy arduboy;
//ArduboyPlaytune tunes;
//AbPrinter text(arduboy);

Player player;
uint8_t g_level=1;

GAME_MODE g_mode = MODE_MENU;

void DrawScore()
{
	text.setCursor(40,6);
	text.print("Score:");
	printNumberFancy(55, 16, player.m_score, true, false);
}

void StartGame()
{
	arduboy.initRandomSeed();
	g_mode = MODE_PLAYING;
#ifdef RT_ARDUDEV
	GetAudioManager()->Play("audio/game_start.wav");
#else
	tunes.playScore(level_intro);
#endif
	ClearAndRedrawLevel();
	arduboy.display();

#ifndef SKIP_INTRO
	delay(1000);
	
	DrawScore();
	text.setCursor(30,46);
	text.print("Level ");
	text.print(g_level);

	arduboy.display();
	delay(1500);
	ClearAndRedrawLevel();
	//DrawScore();
	delay(300);

	/*
	text.setCursor(25,46);
	text.print("Get ready!");
	arduboy.display();
	delay(1000);
*/
#endif
	ClearAndRedrawLevel();
	ResetGhostPositions(true);
	arduboy.display();
	g_mode = MODE_PLAYING;
}

void ClearAndRedrawLevel()
{
	arduboy.clear();
	arduboy.drawBitmap(0, 0, level, WIDTH, HEIGHT, 1);
	player.Render();
	fruit.RenderSideFruits();
}

void main_setup()
{
	tunes.initChannel(PIN_SPEAKER_1);
	tunes.initChannel(PIN_SPEAKER_2);

	arduboy.begin();
	
	if (!arduboy.audio.enabled())
	{
		//if I don't do this, audio plays even though it shouldn't
		arduboy.audio.off();
		arduboy.audio.saveOnOff();
	}
	//arduboy.boot();         // if we wanted to skip the arduboy title
	//arduboy.audio.begin();
	
	arduboy.setFrameRate(40); //doubled for ardulib 1.1 which I guess has a timing issue
   //arduboy.clear();
	
}

void PlayingGame()
{
	player.UnRender();
	fruit.UnRender();

	for (int i=0; i < GHOST_COUNT; i++)
	{
		ghosts[i].UnRender();
	}

	player.Update();
	fruit.Update();

	for (int i=0; i < GHOST_COUNT; i++)
	{
		ghosts[i].Update();
	}

	player.Render();
	fruit.Render();

	for (int i=0; i < GHOST_COUNT; i++)
	{
		ghosts[i].Render(false);
	}

	player.OnEndOfLoop();
	arduboy.display();

	if (player.LevelPassed())
	{
		#ifdef RT_ARDUDEV
		GetAudioManager()->Play("audio/win_level.wav");
#else
		tunes.playScore(music_finish_level);
#endif
		g_level++;
		delay(3500);
		player.ResetLevel();
		StartGame();
	}
}
int8_t g_selection = 0;

void DrawMenuSelection(char *pText, uint8_t id, uint8_t curSel, uint8_t x, uint8_t y)
{
	text.setCursor(x,y);
	if (id == curSel) text.print("[ ");
	text.print(pText);
	if (id == curSel) text.print(" ]");
}

enum eMainMenu 
{   MENU_PLAY,
	MENU_HIGHSCORES,
	MENU_AUDIO_TOGGLE,
	//add more options above here
	MENU_COUNT
};

void DrawMenu()
{
	arduboy.clear();
	//text.setSize(2);
	text.setCursor(5,5);
	text.println("Ardu-man");
	//text.setSize(1);
	//menu
	DrawMenuSelection("Play", MENU_PLAY, g_selection, 5, 25);
	DrawMenuSelection("High Scores", MENU_HIGHSCORES, g_selection, 5, 35);
	
	if (arduboy.audio.enabled())
	{
		DrawMenuSelection("Sound is: ON", MENU_AUDIO_TOGGLE, g_selection, 5, 45);
	} else
	{
		DrawMenuSelection("Sound is: OFF", MENU_AUDIO_TOGGLE, g_selection, 5, 45);
	}
	arduboy.display();
}

void StartMenu()
{
	DrawMenu();
	
	int8_t oldChoice = g_selection;
	if (arduboy.pressed(UP_BUTTON))
	{
		--g_selection;
		if (g_selection < 0) g_selection = MENU_COUNT-1;
	}

	if (arduboy.pressed(DOWN_BUTTON))
	{
		g_selection = (g_selection+1)%MENU_COUNT;
	}
	
	if (arduboy.pressed(A_BUTTON))
	{
		DrawMenu();
		tunes.tone(623, 20);
		delay(200);

		if (g_selection == MENU_AUDIO_TOGGLE)
		{
			if (arduboy.audio.enabled())
			{
				arduboy.audio.off();
			} else
			{
				arduboy.audio.on();
			}
			arduboy.audio.saveOnOff();
			return;
		}
		if (g_selection == MENU_PLAY)
		{
			g_mode = MODE_START;
			return;
		}

		if (g_selection == MENU_HIGHSCORES)
		{
			g_mode = MODE_HIGHSCORES;
			return;
		}
	}
	if (g_selection != oldChoice)
	{
		DrawMenu();
		tunes.tone(523, 20);
		delay(200);
	}
}

void main_loop()
{
  // pause render until it's time for the next frame
 if (!(arduboy.nextFrame()))
    return;

 switch(g_mode)
 {
 case MODE_START:
	StartGame();
	 break;
 case MODE_PLAYING:
	 PlayingGame();
	 break;
 case MODE_MENU:
	 player.ResetGame();
	 StartMenu();
	 break;
 case MODE_HIGHSCORES:
	 displayHighScores(EEPROM_HIGH_SCORE_SAVE_SLOT);
	 break;

 }
  
}


