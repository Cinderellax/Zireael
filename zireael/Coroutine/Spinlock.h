#ifndef __ZIREAEL_SPINLOCK_H__
#define __ZIREAEL_SPINLOCK_H__

#include <atomic>

namespace Zireael{
class Spinlock{
public:
    Spinlock():
    _sem(1)
    {}
    ~Spinlock()
    {
        unlock();
    }
public:
    void lock()
    {
        int exp = 1;
        //compare_exchange_strong(exp,val)
        //如果exp==sem,则sem = val,返回true
        //否则exp = val,返回false
        //阻塞在此处
        for(; !_sem.compare_exchange_strong(exp,0); exp = 1);
    }

    void unlock()
    {
        _sem.store(1);
    }
private:
    std::atomic_int _sem;
};
}

#endif