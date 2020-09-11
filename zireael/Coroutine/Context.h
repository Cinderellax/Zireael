#ifndef __ZIREAEL_CONTEXT_H__
#define __ZIREAEL_CONTEXT_H__

#include <ucontext.h>

namespace Zireael{

//前置声明这个Processor对象
class Processor;

class Context{
public:

#ifndef Attention
    //构造函数中的栈大小的初始值设置为:
    Context(const size_t& stackSize = 0);
#endif
    //复制构造函数不用关心
    //移动构造函数也是不用担心
    //赋值构造函数应该也是不用担心的
    ~Context();
public:
    //用函数指针设置当前context的上下文入口
    void makeContext(void (*func)(), Processor*, Context*);

    //直接用当前程序状态设置当前context的上下文
    void makeCurContext();

    //将当前上下文保存到oldCtx中，然后切换到当前上下文，若oldCtx为空，则直接运行
    void swapToMe(Context* pOldCtx);

    //获取当前上下文的ucontext_t指针
    inline struct ucontext_t* getUCtx() { return &_ctx; };
private:
    //上下文信息
    struct ucontext_t _ctx;
    //运行时的栈
    void* _stack;
    //栈的大小
    size_t _stackSize;
};
}
#endif