#ifndef LOGGER_INTERFACE_H
#define LOGGER_INTERFACE_H

class LoggerInterface {
public:
	LoggerInterface() {

	}

	virtual ~LoggerInterface() {

	}

	virtual void RuntimeLog(std::string& log) = 0;

	virtual void LuaLog(const char* file, std::string& log) = 0;
}

#endif