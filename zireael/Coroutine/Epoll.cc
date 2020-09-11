#include "Epoll.h"
#include "parameter.h"

#include <string.h>

using namespace Zireael;

Epoll::Epoll():
_epollFd(::epoll_create1(EPOLL_CLOEXEC)),
_activeEvents(parameter::firstSize)
{
}

Epoll::~Epoll()
{
    ::close(_epollFd);
}

bool Epoll::epollCtrl(Coroutine* co, const int& fd,const uint32_t& event, const int& operation)
{
    ::epoll_event Tevent;
    memset(&Tevent, 0 ,sizeof(Tevent));
    Tevent.events = event;
    //将信息存储再data.ptr中，此为联合体
    Tevent.data.ptr = co;
    if(::epoll_ctl(_epollFd,operation,fd,&Tevent) < 0)
        return false;
    return true;
}

bool Epoll::modifyEvent(Coroutine* co, const int& fd, const int& event)
{
    return epollCtrl(co,fd,event,EPOLL_CTL_MOD);
}

bool Epoll::addEvent(Coroutine* co, const int& fd, const int& event)
{
    return epollCtrl(co,fd,event,EPOLL_CTL_ADD);
}

bool Epoll::removeEvent(Coroutine* co, const int& fd, const int& event)
{
    return epollCtrl(co,fd,event,EPOLL_CTL_DEL);
}

int Epoll::getActEvents(std::vector<Coroutine*>& activeEvServs, int timeOutMs)
{
    int Tsize = _activeEvents.size();
    int actNum = ::epoll_wait(_epollFd, &*_activeEvents.begin(), Tsize, timeOutMs);
    int resErrno = errno;
    if(actNum < 0 || actNum > Tsize)
        return resErrno;
    for(int i=0; i<actNum; ++i)
    {
        Coroutine* co = static_cast<Coroutine*>(_activeEvents[i].data.ptr);
        activeEvServs.push_back(co);
    }
    if(Tsize == actNum)
    {
        _activeEvents.resize(Tsize*2);
    }
    return resErrno;
}

