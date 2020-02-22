#ifndef MONEY_H
#define MONEY_H

#include <string>

class CMoney
{

private:
	int money;
public:

	CMoney();
	~CMoney();

	int AddMoney(int);
	int RemoveMoney(int);
	std::string PrintMoney();
	bool CheckMoney(int);

	int GetMoney(); // to set for conditions -> if player can afford item A or not

};

#endif