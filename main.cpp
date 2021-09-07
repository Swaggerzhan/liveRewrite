#include <iostream>
#include <thread>

#include "base/timeval.h"

#include "muduo/net/EventLoop.h"
#include "muduo/net/TcpServer.h"
#include "base/mediasession.h"
#include "base/media.h"





int main() {

    std::string ip = "0.0.0.0";
    muduo::net::InetAddress serverAddr(ip, 554);
    muduo::net::EventLoop loop;
    RtspServer server(&loop, serverAddr);

    MediaSession* session = MediaSession::CreateMediaSession();
    const std::string rtspUrl = "rtsp://" + ip + ":554/" + session->getRtspUrlSuffix();

    session->addMediaSource(channel_0, Media::createNew());
    int32_t sessionId = server.addMediaSession(session);

    std::thread t(sendFrameThread, &server, sessionId, &h264File);
    t.detach();

    std::cout << "Play URL: " << rtspUrl << std::endl;

    server.start();
    loop.loop();
    return 0;

}
