#include "CWhackARoboMole.h"
//int CMoney::money = 0;

CWhackARoboMole::CWhackARoboMole() : mole(0), hit(0), miss(0), totalscore(0)
{
	srand(time(0));
}

CWhackARoboMole::~CWhackARoboMole()
{

}

int CWhackARoboMole::MoleRand()
{
	return mole = rand() % 9;
}

void CWhackARoboMole::MoleCheck(bool m)
{
	if (m == true)
		hit += 5;
	else
		return;
}

void CWhackARoboMole::EndGame()
{
	totalscore = hit - miss;
	if (totalscore < 0)
		totalscore = 0;
}

int CWhackARoboMole::TotalScore()
{
	//EndGame();
	return totalscore = hit;
}

void CWhackARoboMole::MoneyEarned()
{
	CMoney::money += (totalscore / 5);
}