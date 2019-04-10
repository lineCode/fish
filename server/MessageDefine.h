#ifndef MESSAGE_DEFINE_H
#define MESSAGE_DEFINE_H


#define CMD_LOGIN 1


#define AGENT_CMD(REGISTER) \
	REGISTER(1, ReqAuth) \
	REGISTER(2, ReqEnter) \

#endif