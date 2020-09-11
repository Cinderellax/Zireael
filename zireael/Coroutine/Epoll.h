#ifndef __ZIREAEL_EPOLL_H__
#define __ZIREAEL_EPOLL_H__

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

#include "Coroutine.h"


namespace Zireael{

class Epoll{
public:
    Epoll();
    ~Epoll();
public:

    //修改Epoller中的事件
    bool modifyEvent(Coroutine* co, const int& fd, const int& event);

    //向Epoller中添加事件
    bool addEvent(Coroutine* co, const int& fd, const int& event);

    //从Epoller中移除事件
    bool removeEvent(Coroutine* co, const int& fd, const int& event);

    //获取被激活的事件服务,返回errno
    int getActEvents(std::vector<Coroutine*>& activeEvServs, int timeOutMs);

private:
    //封装的epoll_ctl，传入控制的参数来对上面的一些函数进行实际的操作
    bool epollCtrl(Coroutine* co,const int& fd, const uint32_t& event, const int& operation);
    //epoll的文件描述符
    int _epollFd;
    //检测到的激活到的事件 
    std::vector<::epoll_event> _activeEvents;
};
}

#endif