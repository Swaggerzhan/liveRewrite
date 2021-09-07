//
// Created by Swagger on 2021/9/7.
//

#ifndef LIVEREWRITE_RTSPSERVER_H
#define LIVEREWRITE_RTSPSERVER_H

#include <unordered_map>
#include <mutex>

#include "rtsp.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"
#include "mediasession.h"
#include "rtp.h"
#include "rtspconnection.h"



class RtspServer : public Rtsp{
public:

    typedef std::unordered_map<std::string, std::shared_ptr<RtspConnection>> RtspConnectionMap;

    RtspServer(muduo::net::EventLoop* loop, const muduo::net::InetAddress);
    ~RtspServer();

    void start();

    /*
     * 当连接时触发的回调函数，使得连接被注册
     **/
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    /* ???? */
    void onMessage(const muduo::net::TcpConnectionPtr conn, muduo::net::Buffer* buf, muduo::Timestamp);

    int addMediaSession(MediaSession* session);
    void remoteMediaSession(int sessionID);

    bool pushFrame(int sessionID, MediaChannel channelID, AVFrame frame);


private:

private:

    RtspConnectionMap rtspConnectionMap_;
    muduo::net::TcpServer server_;

    std::mutex sessionMutex_;
    /* 通过sessionID找到对应session */
    std::unordered_map<int, std::shared_ptr<MediaSession>> mediaSession_;
    std::unordered_map<std::string, int> rtspSuffixMap_;



};









#endif //LIVEREWRITE_RTSPSERVER_H
