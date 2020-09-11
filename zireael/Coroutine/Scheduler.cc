#include "Scheduler.h"
#include <sys/sysinfo.h>

using namespace Zireael;

Scheduler* Scheduler::pScher_ = nullptr;
std::mutex Scheduler::scherMtx_;

Scheduler::Scheduler():
//按声明的顺序初始化，所以不会出现问题
//没有在初始化列表中的会按声明的顺序进行默认初始化
_proSelector(_processors)
{
}

Scheduler::~Scheduler()
{
    for(auto it : _processors)
	{
		it->stop();
	}
	for (auto it : _processors)
	{
        //分离线程，使得可以被资源回收
		it->join();
		delete it;
	}
}

bool Scheduler::startScheduler(int threadCnt)
{
    for(int i=0; i<threadCnt; ++i)
    {
        _processors.emplace_back(new Processor(i));
        //启动，该新线程开始独立的循环
        _processors[i]->loop();
    }
    return true;
}

//单例模式，获得全局唯一的调度器
Scheduler* Scheduler::getScheduler()
{
	if (nullptr == pScher_)
	{
		std::lock_guard<std::mutex> lock(scherMtx_);
        //临界区，防止创建几个！
		if (nullptr == pScher_)
		{
			pScher_ = new Scheduler();
			pScher_->startScheduler(::get_nprocs_conf());
		}
	}
	return pScher_;
}

void Scheduler::createNewCo(std::function<void()>&& func, size_t stackSize)
{
	_proSelector.next()->goNewCo(std::move(func), stackSize);
}

void Scheduler::createNewCo(std::function<void()>& func, size_t stackSize)
{
	_proSelector.next()->goNewCo(func, stackSize);
}

void Scheduler::join()
{
	for (auto pP : _processors)
	{
		pP->join();
	}
}

Processor* Scheduler::getProcessor(int id)
{
	return _processors[id];
}

int Scheduler::getProCnt()
{
	return static_cast<int>(_processors.size());
}