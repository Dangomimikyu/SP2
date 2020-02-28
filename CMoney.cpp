#include "CMoney.h"

int CMoney::money = 0;
CMoney::CMoney()
{
}

CMoney::~CMoney()
{

}

int CMoney::AddMoney(int m)
{
	return (money += m);
}

int CMoney::RemoveMoney(int m)
{
	return (money -= m);
}

std::string CMoney::PrintMoney()
{
	return std::to_string(money);
}

bool CMoney::CheckMoney(int m)
{
	if (money < m)
		return false;
	else
		return true;
}