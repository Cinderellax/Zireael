#include "Timer.h"

#include <sys/timerfd.h>
#include <string.h>

using namespace Zireael;

Timer::Timer(Epoll* p):
//CLOCK_MONOTONIC 表示系统开启之后的时间，而不是设置的时间，这样可以有效防止修改服务器时间之后带来的副作用
//设置为非阻塞的文件描述符和exec时关闭
_timeFd(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC))
{
    //将定时器fd添加到epoll中,事件是数据可读时通知
    p->addEvent(nullptr, _timeFd, EPOLLIN | EPOLLPRI | EPOLLRDHUP);
}

Timer::~Timer()
{   
    //析构中只用释放这一个资源
    ::close(_timeFd);
}

void Timer::getExpiredCoroutines(std::vector<Coroutine*>& res)
{
    Time nowTime = Time::now();
    //从小根堆中读取前面的所有时间到达的事件
    //将其存储到res中
    while(!_timerCoHeap.empty() && _timerCoHeap.top().first <= nowTime)
    {
        res.push_back(_timerCoHeap.top().second);
        _timerCoHeap.pop();
    }

#ifndef Attention
    //目的应该是从timerfd中取出所有的超时事件
    if(!res.empty())
    {
        size_t cnt = 1024;
        //读干净
        while(cnt >= 1024)
        {
            //这个read好像有问题
            /*
            *   uint64_t exp;
            *   s = read(fd, &exp, sizeof(uint64_t));
            *   exp中应该是超时的事件数量
            *   s应该== sizeof(uint64_t)，否则就是出错
            */
            cnt = ::read(_timeFd,_dummyBuf, 1024);
        }
    }
#endif
    
    //等待堆顶时间之后再触发EPOLL
    if(!_timerCoHeap.empty())
    {
        Time time = _timerCoHeap.top().first;
		resetTimeOfTimefd(time);
    }
}

void Timer::runAt(Time time, Coroutine* co)
{
    _timerCoHeap.push(std::move(std::pair<Time, Coroutine*>(time, co)));
	//判断是否成了堆头
    if (_timerCoHeap.top().first == time)
	{
        //新加入的任务是最紧急的任务则需要更改timefd所设置的时间
		resetTimeOfTimefd(time);
	}
}

//给timefd重新设置时间，time是绝对时间
bool Timer::resetTimeOfTimefd(Time time)
{
	struct itimerspec newValue;
	struct itimerspec oldValue;
	memset(&newValue, 0, sizeof newValue);
	memset(&oldValue, 0, sizeof oldValue);
	newValue.it_value = time.timeIntervalFromNow();
	int ret = ::timerfd_settime(_timeFd, 0, &newValue, &oldValue);
	return ret < 0 ? false : true;
}

void Timer::runAfter(Time time, Coroutine* pCo)
{
	runAt( Time::now() + time, pCo);
}

void Timer::wakeUp()
{
	resetTimeOfTimefd(Time::now());
}