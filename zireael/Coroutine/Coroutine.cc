#include "Coroutine.h"
#include "Processor.h"

#include <utility>
using namespace Zireael;

static void coRealFunc(Processor* p)
{
    p->getCurRunningCo()->startFunc();
	p->killCurCo();
}

Coroutine::Coroutine(Processor* p, size_t stackSize, std::function<void()>&& func):
//传入参数之后func会变化为左值
//move将左值转化为右值，防止func变量销毁后找不到
_func(std::move(func)),
_processor(p),
_status(CO_DEAD),
_ctx(stackSize)
{
    //直到初始化步骤的完成之后再状态变化
    _status = CO_READY;
}

Coroutine::Coroutine(Processor* p, size_t stackSize, std::function<void()>& func):
_func(func),
_processor(p),
_status(CO_DEAD),
_ctx(stackSize)
{
    //直到初始化步骤的完成之后再状态变化
    _status = CO_READY;
}

void Coroutine::resume()
{
    Context* mainCtx = _processor->getMainCtx();
    switch(_status)
    {
        case CO_READY:
            _status = CO_RUNNING;
            _ctx.makeContext((void (*)(void)) coRealFunc, _processor, mainCtx);
            _ctx.swapToMe(mainCtx);
            break;
        case CO_WAITING:
            _status = CO_RUNNING;
            _ctx.swapToMe(mainCtx);
            break;
        default:
            //do nothing....
            break;
    }
}

void Coroutine::yield()
{
    //我觉得不一定是已经运行过了
    _status = CO_WAITING;
}