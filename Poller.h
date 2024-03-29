#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;

// moduo 库中多路事件分发器的核心 IO 复用模块
class Poller : noncopyable {
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    // 给所有 IO 复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    // 判断参数 channel 是否在当前 Poller 当中
    bool hasChannel(Channel *channel) const;

    // EventLoop 可以通过该接口获取默认的 IO 复用的具体实现
    static Poller* newDefaultPoller(EventLoop *loop);

protected:
    // map 的 key 为 sockfd value 为 sockfd 所属的 channel 通道类型
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
private:
    EventLoop *ownerLoop_; // 定义 Poller 所属的事件循环 EventLoop
};