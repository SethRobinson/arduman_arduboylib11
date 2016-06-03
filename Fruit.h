//  ***************************************************************
//  Fruit - Creation date: 06/02/2016
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2016 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef Fruit_h__
#define Fruit_h__

class Fruit
{
public:
	Fruit();
	~Fruit();

	void ResetForNewLevel();
	void OnPlayerDie();
	void Update();
	void OnPelletEaten();
	void Render();
	void UnRender();
	void RenderSideFruits();

private:
	
	bool m_bActive;
	unsigned long m_killTimer;
};

extern Fruit fruit;
#endif // Fruit_h__