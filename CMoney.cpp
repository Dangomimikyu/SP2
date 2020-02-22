#include "CMoney.h"


CMoney::CMoney()
{
	money = 0;
}

CMoney::~CMoney()
{

}

int CMoney::GetMoney()
{
	return money;
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
	if (m < 0)
		return false;
	else
		return true;
}