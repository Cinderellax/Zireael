#ifndef __ZIREAEL_MSTIME_H__
#define __ZIREAEL_MSTIME_H__

#include <sys/time.h>
#include <stdint.h>
#include <utility>

namespace Zireael{
//时间类的单位为ms
class Time{
public:
    //构造函数
    Time(int64_t timeVal):
    _timeVal(timeVal)
    {}
	//析构函数
	~Time(){}
    //用于重载的全局操作符
    int64_t getTimeVal() const {return _timeVal;}
public:
	//静态函数用于给别的类调用
	static Time now();

	#ifdef Attention_Useless
	//1970到现在的秒数
	static time_t nowSec();
	#endif

	//计算当前的时区所在的时间
	static void toLocalTime(time_t second, long timezone, struct tm* tm_time);

	//到现在的时间
	struct timespec timeIntervalFromNow();
private:
    int64_t _timeVal;
};

//重载的全局操作符
	inline bool operator < (const Time& lhs, const Time& rhs)
	{
		return lhs.getTimeVal() < rhs.getTimeVal();
	}

    inline bool operator == (const Time& lhs, const Time& rhs)
	{
		return lhs.getTimeVal() == rhs.getTimeVal();
	}

    inline bool operator > (const Time& lhs, const Time& rhs)
	{
		return lhs.getTimeVal() > rhs.getTimeVal();
	}

	inline bool operator <= (const Time& lhs, const Time& rhs)
	{
		return !(lhs>rhs);
	}

	inline bool operator >= (const Time& lhs, const Time& rhs)
	{
		return !(lhs<rhs);
	}

	inline Time operator + (const Time& lhs, const Time& rhs)
	{
		return Time(lhs.getTimeVal() + rhs.getTimeVal());
	}

}

#endif