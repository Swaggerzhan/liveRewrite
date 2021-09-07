//
// Created by Swagger on 2021/9/7.
//

#ifndef LIVEREWRITE_RTPCONNECTION_H
#define LIVEREWRITE_RTPCONNECTION_H

#include "rtp.h"
#include <arpa/inet.h>
#include "muduo/net/Callbacks.h"
#include <memory>

class RtpConnection{
public:

    bool setUpRtpOverUdp(MediaChannel channelID, int rtp_port, int rtcp_port);

    /* 开始播放 */
    void play();
    void record();
    /* 关闭 */
    void teardown();


private:

    void setRtpHeader(MediaChannel channelID, RtpPacket pkt);
    void setFrameType(uint8_t frameType);
    int sendRtpOverUdp(MediaChannel channelID, RtpPacket pkt);
    int sendRtpPacket(MediaChannel channelID, RtpPacket pkt);

private:

    TransportType transportType_;   // 传输模式

    /* tcp */
    std::weak_ptr<muduo::net::TcpConnection> tcpConn_;
    bool isClosed_;

    uint8_t frameType_;
    MediaChannelInfo mediaChannelInfo_[2]; // video and audio

    int localRtpPort_[2];
    int localRtcpPort_[2];

    /* 对端 */
    int rtpfd_[2];                  // rtp  channel
    int rtcpfd_[2];                 // rtcp channel
    sockaddr_in peerAddr_;          // 对端地址
    sockaddr_in rtpPeerAddr_[2];    // rtp  addr
    sockaddr_in rtcpPeerAddr_[2];   // rtcp addr

};




#endif //LIVEREWRITE_RTPCONNECTION_H
