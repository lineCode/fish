#ifndef COMMON_H

#include <string>
#include <unordered_map>

#define SERVER_TYPE_MAP(MAKE) \
	MAKE(1, LOG, log) \
	MAKE(2, DB, db) \
	MAKE(3, LOGIN, login) \
	MAKE(4, AGENT, agent) \
	MAKE(5, AGENT_MASTER, agentmaster) \
	MAKE(6, SCENE, scene) \
	MAKE(7, SCENE_MASTER, scenemaster) \
	MAKE(8, MAX, max) \


enum SERVER_TYPE
{
#define MAKE(num, name, string) e##name = num,
	SERVER_TYPE_MAP(MAKE)
#undef MAKE
};

static std::unordered_map<uint32_t, const std::string> SERVER_TYPE_NAME = {
#define MAKE(num, name, string) { num, #string },
	SERVER_TYPE_MAP(MAKE)
#undef MAKE
};

#endif