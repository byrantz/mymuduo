#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false)
{

}

Channel::~Channel()
{

}

// Channel 
void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}

/*
当改变 channel 所表示 fd 的 events 事件后，update 负责在 poller 里面更改 fd 相应的事件 epoll_ctl
EventLoop => ChannelList Poller
*/
void Channel::update()
{
    // 通过 channel 所属的 EventLoop, 调用 poller 的相应方法，注册 fd 的 events 事件
    loop_->updateChannel(this);
}

// 在 channel 所属的 EventLoop 中，把当前的 channel 删除掉
void Channel::remove()
{
    loop_->removeChannel(this);
}

// fd 得到 poller 通知以后，处理事件的
void Channel::handleEvent(Timestamp receiveTime) {
    if (tied_) {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        }
    }
    else {
        handleEventWithGuard(receiveTime);
    }
}

// 根据 poller 通知的 channel 发生的具体事件，由 channel 负责具体的回调操作
void Channel::handleEventWithGuard(Timestamp receiveTime) {
    LOG_INFO("channel handleEvent revents:%d\n", revents_);

    if ((revents_ &  EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (revents_ & EPOLLERR) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    if (revents_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}
