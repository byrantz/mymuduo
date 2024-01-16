#pragma once

#include "noncopyable.h"

class InetAddress;

// 封装 socket fd
class Socket : noncopyable {
public:
    explicit Socket(int sockfd) 
        : sockfd_(sockfd) 
    {}

    ~Socket();

    int fd() const { return sockfd_; }
    void bindAddress(const InetAddress &localaddr); // 调用 bind 绑定服务器 IP 端口
    void listen(); // 调用 listen 监听套接字
    int  accept(InetAddress *peeraddr); // 调用 accept 接受新客户连接请求

    void shutdownWrite(); // 调用 shutdown 关闭服务端写通道

    // 下面四个函数都是调用 setsockopt 来设置一些 socket 选项
    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
private:
    const int sockfd_;
};