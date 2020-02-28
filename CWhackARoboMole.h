#pragma once
#include <iostream>
#include <ctime>
#include "CMoney.h"
class CWhackARoboMole
{
private:
	float mole;
	float hit, miss;
	float totalscore;

public:

	CWhackARoboMole();
	~CWhackARoboMole();

	int MoleRand();
	int TotalScore();

	void MoleCheck(bool m);
	void EndGame();

	void MoneyEarned();
};

