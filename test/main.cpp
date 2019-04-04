#include <unordered_map>
#include <iostream>
#include "oolua.h"
#include "util/Vector3.h"
using namespace std;

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

extern "C" int luaopen_pto(lua_State* L);

int main(int argc,const char* argv[]) {
	OOLUA::Script script;

	//luaL_requiref(script.state(), "pto", luaopen_pto, 1);

	//if ( !script.run_file(argv[1]) ) {
	//	cout << OOLUA::get_last_error(script) << endl;
	//}

	Vector3 v1(1, 2, 3);
	Vector3 v2(4, 5, 6);

	v1 += v2;

	Vector3 v3 = v1 + v2;

	return 0;
}
