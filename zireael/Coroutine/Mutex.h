//@author Liu Yukang
#pragma once
#include "Coroutine.h"
#include "Spinlock.h"
#include "Scheduler.h"
#include "SpinlockGuard.h"

#include <atomic>
#include <queue>

namespace Zireael{

    enum muStatus
	{
		MU_FREE = 0,
		MU_READING,
		MU_WRITING
	};

    //读写锁
    class RWMutex{
    public:
        RWMutex()
            : state_(MU_FREE), readingNum_(0)
        {};
        ~RWMutex(){};

        //读锁
        void rlock();
        //解读锁
        void runlock();

        //写锁
        void wlock();
        //解写锁
        void wunlock();

    private:
        void freeLock();

        int state_;

        std::atomic_int readingNum_;

        Spinlock lock_;

        std::queue<Coroutine*> waitingCo_;

    };

}