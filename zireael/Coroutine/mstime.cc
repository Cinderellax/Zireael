#include "mstime.h"

using namespace Zireael;

Time Time::now()
{
    struct timeval tv;
    ::gettimeofday(&tv,0);
    //ms级别
    int64_t res = tv.tv_sec*1000 + tv.tv_usec/1000;
    //借用构造函数返回
    return Time(res);
}

struct timespec Time::timeIntervalFromNow()
{
	struct timespec ts;
	int64_t microseconds = _timeVal - Time::now().getTimeVal();
	if (microseconds < 1)
	{//若时间间隔已经小于Time的精度ms时，即当作1us
		ts.tv_sec = 0;
		ts.tv_nsec = 1000;
	}
	else 
	{
		ts.tv_sec = static_cast<time_t>(
			microseconds / 1000);
		ts.tv_nsec = static_cast<long>(
			(microseconds % 1000) * 1000 * 1000);
	}
	return ts;
}




