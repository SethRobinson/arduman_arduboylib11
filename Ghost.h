//  ***************************************************************
//  Ghost - Creation date: 05/29/2016
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2016 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef Ghost_h__
#define Ghost_h__

#include "Utils.h"
#include "Player.h"
#include "ardu_main.h"
extern Player player;

extern uint8_t GHOST_COUNT;

const unsigned char ghost_bmp[] PROGMEM =
{
	0x07, 0x01, 0x07,
};


class Ghost
{
public:
	Ghost();
	~Ghost();

	void SetPauseTimer(long time);
	float GetAdjustedSpeed();
	void ResetPosition();
	void SetLastTurnedSpot(int16_t x, int16_t y);
	bool IsNewTurnSpot(int16_t x, int16_t y);
	bool IsChasing();
	void Update();
	void Blit(bool color, bool bDying);
	void UnRender();
	void Render(bool bDying);
	void SetGhostID(int8_t id){m_id = id;}
	void OnReleasePellet(uint8_t foodX, uint8_t foodY);
	void ResetForNewGame();

	float m_x;
	float m_y;
	uint8_t m_curDir;
	float m_speed;
	uint8_t m_foodX,m_foodY;
	long m_pauseTimer;
	uint8_t m_id;
	int16_t m_oldX, m_oldY;
	bool m_bIsChasing;
	unsigned long m_thinkTimer;
};

extern Ghost ghosts[GHOST_MAX];

void DontDrawPelletHere(uint8_t x, uint8_t y);
uint8_t RandomTurn(uint8_t dir);

void ResetGhostPositions(bool bNewGame);
#endif // Ghost_h__