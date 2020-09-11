#ifndef __ZIREAEL_COROUTINE_H__
#define __ZIREAEL_COROUTINE_H__

#include <functional>
#include "Context.h"

namespace Zireael{

class Processor;

class Coroutine{
public:
    enum coStatus{
        CO_DEAD = 0,
        CO_READY   ,
        CO_RUNNING ,
        CO_WAITING 
    };
public:
    //这个是使用lambda表达式的情况下使用右值
    Coroutine(Processor*, size_t stackSize, std::function<void()>&&);
    Coroutine(Processor*, size_t stackSize, std::function<void()>&);
    //默认的析构函数
    ~Coroutine(){};

    //恢复运行当前协程
    void resume();
    //暂停运行当前协程
    void yield();
    Processor* getProcess(){return _processor;}

    //运行该协程的函数
    inline void startFunc() { _func(); };

    //获取该协程的上下文
    inline Context* getCtx() { return &_ctx; }
private:
    std::function<void()> _func;
    Processor* _processor;
    int _status;
    Context _ctx;
};
};

#endif