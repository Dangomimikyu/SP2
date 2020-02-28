#ifndef MONEY_H
#define MONEY_H

#include <string>

class CMoney
{

public:
	static int money;

	CMoney();
	~CMoney();

	int AddMoney(int);
	int RemoveMoney(int);
	std::string PrintMoney();
	bool CheckMoney(int);
};

#endif