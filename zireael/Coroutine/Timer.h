#ifndef __ZIREAEL_TIMER_H__
#define __ZIREAEL_TIMER_H__

#include "Epoll.h"
#include "Coroutine.h"
#include "mstime.h"

#include <queue>
#include <iostream>
#include <vector>

namespace Zireael{

class Timer{
public:
    using T = std::pair<Time, Coroutine*>;
    //将优先队列转换为小根堆
    //设置第三个参数是>
    using TimerHeap = std::priority_queue<T, std::vector<T>, std::greater<T>>;
public:
    Timer(Epoll* p);
    ~Timer();
public:
    bool init(Epoll*);

    //获取所有已经超时的需要执行的函数
    void getExpiredCoroutines(std::vector<Coroutine*>& res);

    //在time时刻需要恢复协程co(绝对时间)
    void runAt(Time time, Coroutine* pCo);

    //经过time毫秒恢复协程co(相对时间)
    void runAfter(Time time, Coroutine* pCo);
    
    //将定时器事件设置为当前事件，则第一时间唤醒定时器
    void wakeUp();

private:
    //给timefd重新设置时间，time是绝对时间
    bool resetTimeOfTimefd(Time time);

    inline bool isTimeFdUseful() { return _timeFd < 0 ? false : true; };
private:
    int _timeFd;
    //用于读timeFd上的数据,这一块可能不是必须的
    char _dummyBuf[1024];
    //小根堆
    TimerHeap _timerCoHeap;
};
}
#endif