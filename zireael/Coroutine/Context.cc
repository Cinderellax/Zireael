#include "Context.h"
#include "parameter.h"

#include <stdlib.h>

using namespace Zireael;

Context::Context(const size_t& stackSize):
_stack(nullptr),
_stackSize(stackSize)
{
    if(stackSize > 0)
        _stack = ::malloc(_stackSize);
}

Context::~Context()
{
    if(_stack)
        ::free(_stack);
}

void Context::makeContext(void (*func)(), Processor* pP, Context* pLink)
{
    ::getcontext(&_ctx);
    _ctx.uc_stack.ss_sp = _stack;
    _ctx.uc_stack.ss_size = parameter::coroutineStackSize;
    //使用pLink的上下文作为结束后的运行
    _ctx.uc_link = pLink->getUCtx();
    ::makecontext(&_ctx, func, 1, pP);
}

void Context::makeCurContext()
{
    ::getcontext(&_ctx);
}

void Context::swapToMe(Context* pOldCtx)
{
    if(nullptr == pOldCtx)
        ::setcontext(&_ctx);
    else
        ::swapcontext(pOldCtx->getUCtx(),&_ctx);
}