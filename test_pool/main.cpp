#include <iostream>
#include <vector>
#include "util/PoolObject.h"

class TestClass : public PoolObject<TestClass>
{
public:
	TestClass()
	{
	}
};

int main()
{
	std::vector<TestClass*> tcs;
	for (int i = 0;i < 129 ;i++)
	{
		TestClass* tc = new TestClass();
		tcs.push_back(tc);
	}

	for (int i = 0;i < tcs.size();i++)
	{
		delete tcs[i];
	}


	std::vector<TestClass*> tcs_;
	for (int i = 0;i < 129 ;i++)
	{
		TestClass* tc = new TestClass();
		tcs_.push_back(tc);
	}

	for (int i = 0;i < tcs_.size();i++)
	{
		delete tcs_[i];
	}

	

	return 0;
}