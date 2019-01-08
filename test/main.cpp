#include "Allocator.h"
#include "PoolObject.h"

struct TestA : public PoolObject<TestA> {
	int a;
	TestA(int a) {
		a = a;
	}
	static std::string GetClassName() {
		return std::string("TestA");
	}
};

struct TestB : public PoolObject<TestB> {
	int a;
	int b;
	static std::string GetClassName() {
		return std::string("TestB");
	}
};

struct TestC : public TestA{
	int b;

	static std::string GetClassName() {
		return std::string("TestC");
	}
};
int main() {

	TestA* a = new TestA(1);
	TestB* b = new TestB();



	PoolObject<TestA>::Dump();
	PoolObject<TestB>::Dump();
	//PoolObject<TestC>::Dump();
	return 0;
}