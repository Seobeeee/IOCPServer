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
	vector<int32, StlAllocator<int32>> v;
}