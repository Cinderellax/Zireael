#ifndef __ZIREAEL_PROCESSOR_H__
#define __ZIREAEL_PROCESSOR_H__

#include "Epoll.h"
#include "Coroutine.h"
#include "Timer.h"
#include "Spinlock.h"

#include <thread>
#include <set>

//gcc中的变量，__thread关键字每个线程都只有一个实体，线程之间不互相影响
extern __thread int threadIdx;

namespace Zireael{
class Processor{
public:
    enum Status{
        PRO_RUNNING,
        PRO_STOPPING,
        PRO_STOPPED
    };
    enum CoAddStatus{
        CO_ADDING,
        CO_ADDED
    };
public:
    Processor(int tid);
    ~Processor();
public:
    //运行一个新协程，该协程的函数是func
    void goNewCo(std::function<void()>&& func, size_t stackSize);
    void goNewCo(std::function<void()>& func, size_t stackSize);
    //放弃运行
    void yield();
    //当前协程等待time毫秒
    void wait(Time time);

    //清除当前正在运行的协程
    void killCurCo();

    bool loop();

    void stop();

    void join();

    //等待fd上的ev事件返回
    void waitEvent(int fd, int ev);

    //获取当前正在运行的协程
    inline Coroutine* getCurRunningCo() { return _curCoroutine; };

    inline Context* getMainCtx() { return &_mainCtx; }

    inline size_t getCoCnt() { return _coSet.size(); }

    void goCo(Coroutine* co);

    void goCoBatch(std::vector<Coroutine*>& cos);


private:
    //恢复运行指定协程
    void resume(Coroutine*);

    inline void wakeUpEpoller();
    //传递给pthread运行的loop
    void runInLoop();
    
    //线程号
    int _tid;
    //状态
    Status _status;
    //线程
    std::thread* _loop;
    //新任务队列，双缓冲队列
    std::queue<Coroutine*> _newCoroutines[2];
    /*有些变量是用 volatile 关键字声明的。
    *当两个线程都要用到某一个变量且该变量的值会被改变时，
    *应该用 volatile 声明，该关键字的作用是防止优化编译器把变量从内存装入 CPU 寄存器中。
    *如果变量被装入寄存器，那么两个线程有可能一个使用内存中的变量，
    *一个使用寄存器中的变量，这会造成程序的错误执行。
    */
    //新任务双缓存队列中正在运行的队列号，另一条用于添加任务
    volatile int _runningNewQue;
    //两个自旋锁
    Spinlock _newQueLock;
    Spinlock _coPoolLock;

    //Epoll的活跃事件
    std::vector<Coroutine*> _actCoroutines;

    std::set<Coroutine*> _coSet;

    //定时器任务列表
    std::vector<Coroutine*> _timerExpiredCo;

    //被移除的协程列表，要移除某一个事件会先放在该列表中，一次循环结束才会真正delete
    std::vector<Coroutine*> _removedCo;

    Epoll _epoll;

    Timer _timer;
#ifdef Attention
//    ObjPool<Coroutine> coPool_;
#endif

    Coroutine* _curCoroutine;

    Context _mainCtx;
};
}
#endif