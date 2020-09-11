#ifndef __ZIREAEL_PARAMETER_H__
#define __ZIREAEL_PARAMETER_H__

#include <stddef.h>

namespace Zireael{
    namespace parameter{
        //协程栈的大小
        const static size_t coroutineStackSize = 32 * 1024;
        //epoll中初始的活动数组的长度
        const static size_t firstSize = 16;
        //epoll_wait的阻塞时长
		static constexpr int epollTimeOutMs = 10000;
        //监听队列的长度
		constexpr static unsigned backLog = 4096;
    }
}
#endif