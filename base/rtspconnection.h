//
// Created by Swagger on 2021/9/5.
//

#ifndef LIVEREWRITE_RTSPCONNECTION_H
#define LIVEREWRITE_RTSPCONNECTION_H

#include <memory>
#include "muduo/net/TcpConnection.h"

class RtspServer;
class Media;
class RtspRequest;
class RtspRespond;

class RtspConnection{
public:

    RtspConnection(RtspServer* server, const muduo::net::TcpConnectionPtr& conn);
    ~RtspConnection();

    /* 请求入口 */
    void handleMessage();
    // 处理describe请求
    void handleCmdDescribe();

private:


private:
    std::unique_ptr<RtspRequest> rtspRequest_;
    //std::unique_ptr<RtspRespond> rtspRespond_;
    std::unique_ptr<Media> media_;

};


#endif //LIVEREWRITE_RTSPCONNECTION_H

