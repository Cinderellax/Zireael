#include "Processor.h"
#include "SpinlockGuard.h"
#include "parameter.h"

using namespace Zireael;

//线程独享变量初始化为-1
__thread int threadIdx = -1;

/*
        //线程号
        int _tid;
        //状态
        Status _status;
        //线程
        std::thread* _loop;
        //新任务队列，双缓冲队列
        std::queue<Coroutine*> _newCoroutines[2];
        //双缓冲队列当前使用的下标
        volatile int _runningNewQue;
        //两个用于互斥的锁
        Spinlock newQueLock_;
        Spinlock coPoolLock_;
        //EPoll中的活跃事件
        std::vector<Coroutine*> _actCoroutines;
        //所有的协程集合
        std::set<Coroutine*> _coSet;
        //超时的协程集合
        std::vector<Coroutine*> _timerExpiredCo;
        //被移除的协程列表，先放入其中，之后才会被删除
        std::vector<Coroutine*> _removedCo;
        //每个Processor都独占一个Epoil和timer
        Epoll _epoll;
        Timer _timer;
        //当前运行的协程
        Coroutine* _CurCoroutine;
        //主上下文
        Context _mainCtx;
*/

Processor::Processor(int tid):
_tid(tid),
_status(PRO_STOPPED),
_loop(nullptr),
_runningNewQue(0),
//这两块可能是有问题
_epoll(),
//按理说未在这里进行初始化，init中初始化
_timer(&_epoll),
_curCoroutine(nullptr),
_mainCtx(0)
{
    //就是将当前的上下文存到mainctx中
    //不知道在构造函数中进行这个？？？？？
    _mainCtx.makeCurContext();
}

//一般情况下不会被调用的
//只要在程序运行过程中会始终保持一个core一个Processor
Processor::~Processor()
{
    if (PRO_RUNNING == _status)
	{
        //后面实现的功能
		stop();
	}
	if (PRO_STOPPING == _status)
	{
        //后面实现的功能
		join();
	}
	if (nullptr != _loop)
	{
        //删除线程
		delete _loop;
	}
	for (auto co : _coSet)
	{
        //删除所有的协程
		delete co;
	}
}

void Processor::resume(Coroutine* co)
{
	if (nullptr==co || _coSet.count(co)==0)
    {
        //这里可以打日志
        //屁勒，就是故意留给定时器事件的，定时器的fd中绑定的co指针是NULL
        return;
    }

	_curCoroutine = co;
	co->resume();
}

void Processor::yield()
{
    //这个yield只是将协程中的状态改变
    _curCoroutine->yield();
    //将当前的上下文保存到_curCoroutine中
    //然后切换到_mainCtx
	_mainCtx.swapToMe(_curCoroutine->getCtx());
}

void Processor::wait(Time time)
{
    //先放弃当前的协程
	_curCoroutine->yield();
    //设置定时器
	_timer.runAfter(time,_curCoroutine);
    //切换到主上下文（用来调度）
	_mainCtx.swapToMe(_curCoroutine->getCtx());
}

void Processor::goCo(Coroutine* co)
{
	{
		SpinlockGuard lock(_newQueLock);
        //未在运行的一条队列添加协程
		_newCoroutines[!_runningNewQue].push(co);
	}
    //唤醒epoll
	wakeUpEpoller();
}

//批量添加协程
void Processor::goCoBatch(std::vector<Coroutine*>& cos){
	{
		SpinlockGuard lock(_newQueLock);
		for(auto co : cos){
			_newCoroutines[!_runningNewQue].push(co);
		}
	}
	wakeUpEpoller();
}

bool Processor::loop()
{
    //这里本来有初始化epoll和timer，被我放在构造函数中了

    _loop = new std::thread(&Processor::runInLoop, this);
    return true;
}

void Processor::runInLoop()
{
    threadIdx = _tid;
    _status = PRO_RUNNING;
    while (PRO_RUNNING == _status)
    {
        //清空所有列表
        if(!_actCoroutines.empty())
            _actCoroutines.clear();
        if(!_timerExpiredCo.empty())
            _timerExpiredCo.clear();
#ifndef Attention
//定时器获得的将会是一个NULL,存放于_actCoroutines中
        //获取活跃事件
        _epoll.getActEvents(_actCoroutines,parameter::epollTimeOutMs);
//要命的是不管定时器是否到时间，都会强制启动定时器检查机制
        //处理超时协程
        _timer.getExpiredCoroutines(_timerExpiredCo);
#endif
//超时协程的队列获得
        size_t timerCoCnt = _timerExpiredCo.size(); 
        for (size_t i = 0; i < timerCoCnt; ++i)
        {
            //有点狗的在resume中判断是否是NULL来排除定时器事件
            //这操作其实只是将——curroutine设置为第i个活跃co
            //然后改变一下状态？？？？
            //放弃我上面说的，定时器中按理存放的是普通协程
            resume(_timerExpiredCo[i]);
        }

        //执行新来的协程
        Coroutine* pNewCo = nullptr;
        int runningQue = _runningNewQue;
        //正在运行的双缓冲队列之一
        while (!_newCoroutines[runningQue].empty())
        {
            {
                pNewCo = _newCoroutines[runningQue].front();
                _newCoroutines[runningQue].pop();
                _coSet.insert(pNewCo);
            }
            //将该缓冲队列中的所有协程都切换为运行状态
            resume(pNewCo); 
        }
        //切换缓冲队列
        {
            SpinlockGuard lock(_newQueLock);
            _runningNewQue = !runningQue;
        }

        //执行被唤醒的协程,epoll获得的
        size_t actCoCnt = _actCoroutines.size();
        for (size_t i = 0; i < actCoCnt; ++i)
        {
            resume(_actCoroutines[i]);
        }

        //清除已经执行完毕的协程
        for (auto deadCo : _removedCo)
        {
            _coSet.erase(deadCo);
            //delete deadCo;
            {
                SpinlockGuard lock(_coPoolLock);
#ifndef Attention
//这边原版使用的是对象池
                delete deadCo;
#endif
            }
        }
        _removedCo.clear();
        
    }
    _status = PRO_STOPPED;
}

//等待fd上的ev事件返回
void Processor::waitEvent(int fd, int ev){
	_epoll.addEvent(_curCoroutine, fd, ev);
	yield();
	_epoll.removeEvent(_curCoroutine, fd, ev);
}

void Processor::stop()
{
	_status = PRO_STOPPING;
}

void Processor::join()
{
	_loop->join();
}

void Processor::wakeUpEpoller()
{
	_timer.wakeUp();
}

void Processor::goNewCo(std::function<void()>&& coFunc, size_t stackSize)
{
	Coroutine* pCo = new Coroutine(this, stackSize, std::move(coFunc));
	// Coroutine* pCo = nullptr;

	// {
	// 	SpinlockGuard lock(coPoolLock_);
	// 	pCo = coPool_.new_obj(this, stackSize, std::move(coFunc));
	// }

	goCo(pCo);
}

void Processor::goNewCo(std::function<void()>& coFunc, size_t stackSize)
{
	Coroutine* pCo = new Coroutine(this, stackSize, coFunc);
	// Coroutine* pCo = nullptr;

	// {
	// 	SpinlockGuard lock(coPoolLock_);
	// 	pCo = coPool_.new_obj(this, stackSize, coFunc);
	// }
	
	goCo(pCo);
}

void Processor::killCurCo()
{
	_removedCo.push_back(_curCoroutine);
}