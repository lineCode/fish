#include "time/Timestamp.h"
#include <iostream>
#include <time.h>
int main() {

	std::cout << Now() << ":" << TimeStamp() << std::endl;
	std::cout << GetTimeZone() << std::endl;

	char tmp[64];
	strftime(tmp, sizeof( tmp ), "%Y-%m-%d %H:%M:%S", Now());
	return 0;
}