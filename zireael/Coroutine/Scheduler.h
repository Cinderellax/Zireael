#ifndef __ZIREAEL_SCHEDULER_H__
#define __ZIREAEL_SCHEDULER_H__

#include "ProcessorSelect.h"

#include <mutex>

namespace Zireael{

class Scheduler{
public:
    Scheduler();
    ~Scheduler();
public:
    //单例模式，获得一个调度器
    static Scheduler* getScheduler();
    //创建新的协程，需要支持lambda
    void createNewCo(std::function<void()>&& func, size_t stackSize);
    void createNewCo(std::function<void()>& func, size_t stackSize);

    Processor* getProcessor(int);

    int getProCnt();

	void join();

private:
    //初始化Scheduler，threadCnt为开启几个线程
    bool startScheduler(int threadCnt);

#ifndef Attention
//我也不清楚这两个是干啥用的
    //单例模式的调度器！！！！
    static Scheduler* pScher_;

    //用于保护的锁，为了服务器执行效率，原则上不允许长久占有此锁
    static std::mutex scherMtx_;
#endif
    std::vector<Processor*> _processors;
    ProcessorSelect _proSelector;
};
}

#endif