#include <unordered_map>
#include <iostream>
#include "oolua.h"

using namespace std;

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

extern "C" int luaopen_pto(lua_State* L);

int main(int argc,const char* argv[]) {
	OOLUA::Script script;

	luaL_requiref(script.state(), "pto", luaopen_pto, 1);

	if ( !script.run_file(argv[1]) ) {
		cout << OOLUA::get_last_error(script) << endl;
	}

	return 0;
}
