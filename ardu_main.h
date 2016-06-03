#ifndef ardu_main_h__
#define ardu_main_h__

//game settings

#ifdef _DEBUG
#define SKIP_INTRO
#endif

//#define SHOW_PELLET_COUNT
//#define CHEAT_ALWAYS_HAVE_POWER_PILL

//the highscore system is the same as the breakout example - pick an unused eeprom slot!
#define EEPROM_HIGH_SCORE_SAVE_SLOT 8

#define LIVES_COUNT 3
#define PLAYER_FAST_SPEED 0.6f
#define PLAYER_SLOW_SPEED 0.48f //when eating pellets
#define GHOST_SPEED 0.5f

#define GHOST_MAX 4
#define STARTING_GHOSTS 4
#define GHOSTS_TO_ADD_EACH_LEVEL 0 //if not 0, we add to the total ghost count each level, allowing crazy bonuses
#define GHOST_KILL_DELAY_MS 380 //the freeze when you "pop" a ghost
#define TUNNEL_X 53 //top left of where the entity touches
#define GHOST_CUBBY_HOLE_X 53
#define GHOST_CUBBY_HOLE_Y 23
#define GHOST_TUNNEL_SPEED_MOD 0.6f; //ghost speed is multiplied by this when in tunnel
#define GHOST_START_Y 17
#define TOTAL_PELLETS 244 //arcade game has 240.  Fruit at 70 and 170 
#define C_POWER_PILL_TIME_MS 6000
#define POWER_WEAR_OFF_WARNING_MS 1000
#define GHOST_KILL_POINTS 200
#define PELLET_POINTS 10
#define POWER_PELLET_POINTS 50


void main_loop();
void main_setup();
void ClearAndRedrawLevel();
void DrawScore();

enum GAME_MODE
{
	MODE_NONE,
	MODE_MENU,
	MODE_START,
	MODE_PLAYING,
	MODE_DYING,
	MODE_HIGHSCORES

};

extern GAME_MODE g_mode;

#endif // ardu_main_h__




