#ifndef __ZIREAEL_COAPI_H__
#define __ZIREAEL_COAPI_H__

#include "Scheduler.h"
#include "mstime.h"
#include "parameter.h"

namespace Zireael{

/*
 * 创建一个协程
 * @func: 可调用对象
 * @stackSize: 使用的栈的长度
 * @tid: 协程运行所在线程的线程号
 *         -1: 程序选择
 *        其他: 用户指定，范围是[0,coreNum-1]
 */
void cothread(std::function<void()>& func, size_t stackSize = parameter::coroutineStackSize, int tid = -1);
void cothread(std::function<void()>&& func, size_t stackSize = parameter::coroutineStackSize, int tid = -1);

//协程等待t单位的ms
void co_sleep(Time t);

//等待调度器的结果
void sche_join();

}

#endif