#include "Co_api.h"

void Zireael::cothread(std::function<void()>& func, size_t stackSize, int tid)
{
    if(tid<0)
	{
		Scheduler::getScheduler()->createNewCo(func, stackSize);
	}
	else
	{
		tid %= Scheduler::getScheduler()->getProCnt();
		Scheduler::getScheduler()->getProcessor(tid)->goNewCo(func, stackSize);
	}
}

//针对lambda的形式的创建协程
//其中需要使用move来讲左值转换为右值来调用别的函数
void Zireael::cothread(std::function<void()>&& func, size_t stackSize, int tid)
{
    if(tid<0)
	{
		Scheduler::getScheduler()->createNewCo(std::move(func), stackSize);
	}
	else
	{
		tid %= Scheduler::getScheduler()->getProCnt();
		Scheduler::getScheduler()->getProcessor(tid)->goNewCo(std::move(func), stackSize);
	}
}

void Zireael::co_sleep(Time t)
{
    //这个threadIdx是线程独占的变量
    //每个线程都是不同的，保存的是自己的线程号
    Scheduler::getScheduler()->getProcessor(threadIdx)->wait(t);
}

void Zireael::sche_join()
{
    Scheduler::getScheduler()->join();
}
