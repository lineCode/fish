﻿#ifndef COMMON_H

#include <string>
#include <unordered_map>

#define APP_TYPE_MAP(MAKE) \
	MAKE(1, LOG,			log) \
	MAKE(2, DB,				db) \
	MAKE(3, LOGIN,			login) \
	MAKE(4, AGENT,			agent) \
	MAKE(5, AGENT_MASTER,	agentm) \
	MAKE(6, SCENE,			scene) \
	MAKE(7, SCENE_MASTER,	scenem) \
	MAKE(8, MASTER,			master) \
	MAKE(9, MAX,			max) \


enum APP_TYPE
{
#define MAKE(num, name, string) e##name = num,
	APP_TYPE_MAP(MAKE)
#undef MAKE
};

static std::unordered_map<uint32_t, const std::string> APP_TYPE_DESC = {
#define MAKE(num, name, string) { num, #string },
	APP_TYPE_MAP(MAKE)
#undef MAKE
};

static std::unordered_map<uint32_t, const std::string> APP_TYPE_NAME = {
#define MAKE(num, name, string) { num, #name },
	APP_TYPE_MAP(MAKE)
#undef MAKE
};


#endif
