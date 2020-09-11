#ifndef __ZIREAEL_PROCESSORSELECT_H__
#define __ZIREAEL_PROCESSORSELECT_H__

#include "Processor.h"

namespace Zireael{

class ProcessorSelect{
public:
    enum scheduleStrategy
	{
		MIN_EVENT_FIRST = 0 , //最少事件优先
		ROUND_ROBIN			  //轮流分发
	};
public:
    ProcessorSelect(std::vector<Processor*>& processors, int strategy = MIN_EVENT_FIRST):  
    _curPro(-1),
    _strategy(strategy),
    _processors(processors)
    {}
    ~ProcessorSelect() {}
public:
    //设置分发任务的策略
    //MIN_EVENT_FIRST则每次挑选EventService最少的EventManager接收新连接
    //ROUND_ROBIN则每次轮流挑选EventManager接收新连接
    inline void setStrategy(int strategy) { _strategy = strategy; };
    Processor* next();
private:
    //当前使用的Prodessor中的下标
    int _curPro;
    int _strategy;
    //vector中存放的是指针呀
    std::vector<Processor*>& _processors;
};
}

#endif