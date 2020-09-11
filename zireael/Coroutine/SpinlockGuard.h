#ifndef __ZIREAEL_SPINLOCKGUARD_H__
#define __ZIREAEL_SPINLOCKGUARD_H__

#include "Spinlock.h"

namespace Zireael{

class SpinlockGuard{
public:
    //需要阻止所有的拷贝操作

    SpinlockGuard(Spinlock& s):
    lock(s)
    {
        lock.lock();
    }
    ~SpinlockGuard()
    {
        lock.unlock();
    }
private:
    //设置为引用，防止拷贝
    Spinlock& lock;
};
}

#endif