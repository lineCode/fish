
#ifndef INTERFACE_H
#define INTERFACE_H

#include "EventPoller.h"
#include "Network.h"
#include <string>

namespace Network
{
	class InputHandler
	{
	public:
		virtual ~InputHandler() {};
		virtual int HandleInput() = 0;
	};

	class OutputHandler
	{
	public:
		virtual ~OutputHandler() {};
		virtual int HandleOutput() = 0;
	};

	class ErrorHandler
	{
	public:
		virtual ~ErrorHandler() {};
		virtual int HandleError() = 0;
	};
}


#endif