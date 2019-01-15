#include <unordered_map>
#include <iostream>
int main() {
	std::unordered_map<const char*, int> tbl;
	tbl["a"] = 1;
	tbl["b"] = 1;
	
	return 0;
}