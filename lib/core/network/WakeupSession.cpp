#include "WakeupSession.h"
#include "Network.h"
#include "../time/Timestamp.h"
#include <sys/timerfd.h> 
namespace Network
{
	WakeupSession::WakeupSession(EventPoller* poller,int wait)
	{
		_id = -1;
		_poller = poller;

		_fd = timerfd_create(CLOCK_MONOTONIC,O_NONBLOCK|O_CLOEXEC);

		int sec = wait / 1000;
		int ms = wait % 1000; 
		
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		  
		struct itimerspec spec;

		long long nosec = (now.tv_sec + sec)*1000*1000*1000 + now.tv_nsec + ms*1000*1000;
		spec.it_value.tv_sec = nosec / (1000*1000*1000);
		spec.it_value.tv_nsec = nosec % (1000*1000*1000);
		spec.it_interval.tv_sec = sec;
		spec.it_interval.tv_nsec = ms*1000*1000;	

		if (0 != timerfd_settime(_fd,TFD_TIMER_ABSTIME,&spec,0))
			assert(false);
	}


	WakeupSession::~WakeupSession()
	{
	}

	void WakeupSession::SetId(int id)
	{
		_id = id;
	}

	int WakeupSession::GetId()
	{
		return _id;
	}

	int WakeupSession::Timerfd()
	{
		return _fd;
	}

	int WakeupSession::HandleInput()
	{
		int64 r = 0;
		int len = SocketRead(_fd,(char*)&r,sizeof(int64));
		if (len > 0)
			_poller->Timer().Update(TimeStamp());
		else if (len == 0)
			fprintf(stderr,"timerfd sinal\n");
		else if (len < 0)
			assert(false);
		return 0;
	}
}
