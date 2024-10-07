#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include "ThreadManager.h"
#include "PlayerManager.h"
#include "AccountManager.h"
#include "Memory.h"

class Knight
{
public:
	Knight()
	{
		cout << endl;
	}
	Knight(int32 hp) : _hp(hp)
	{

	}

	~Knight()
	{

	}

private:

	int32 _hp = 100;

};

int main()
{
	Knight* knight = xnew<Knight>(100);

	xdelete(knight);
}