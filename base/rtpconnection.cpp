//
// Created by Swagger on 2021/9/7.
//
#include "rtpconnection.h"
#include <cstring>
#include <iostream>
#include <random>
#include <unistd.h>

#include "muduo/net/TcpConnection.h"
#include "muduo/net/EventLoop.h"

void RtpConnection::setRtpHeader(MediaChannel channelID, RtpPacket pkt) {
    /* 已经开始播放了，则开始封装数据包头 */
    if ( mediaChannelInfo_[channelID].isPlay || mediaChannelInfo_[channelID].isRecord){
        mediaChannelInfo_[channelID].rtpHeader.marker = pkt.last;
        mediaChannelInfo_[channelID].rtpHeader.timestamp = htonl(pkt.timestamp); // 时间戳
        /* 序列号默认直接+1 */
        mediaChannelInfo_[channelID].rtpHeader.seq = htons(mediaChannelInfo_[channelID].rtpHeader.seq++);
        /* 将生成好的包头数据拷贝到对应的data中去 */
        memcpy(pkt.data.get() + 4, &mediaChannelInfo_[channelID].rtpHeader, 12);
    }
}


int RtpConnection::sendRtpOverUdp(MediaChannel channelID, RtpPacket pkt) {
    int ret = sendto(
            rtpfd_[channelID], pkt.data.get() + 4, pkt.size - 4, 0,
            (sockaddr*)&rtpPeerAddr_[channelID],
            sizeof( struct sockaddr_in)
            );

    if ( ret < 0 ){
#ifdef DEBUG
        std::cout << "sendto Error, code: " << code << std::endl;
#endif
        exit(1);
    }
    return ret;
}

void RtpConnection::setFrameType(uint8_t frameType) {
    frameType_ = frameType;
//    if(!_hasIDRFrame && (frameType_ == 0 || frameType_ == VIDEO_FRAME_I)) {
//        _hasIDRFrame = true;
//    }
}


int RtpConnection::sendRtpPacket(MediaChannel channelID, RtpPacket pkt) {

    if ( isClosed_ )
        return -1;

    auto tcpConn = tcpConn_.lock();
    if ( tcpConn == nullptr )
        return -1;

    tcpConn->getLoop()->runInLoop(
            std::bind( [this](MediaChannel channelID, RtpPacket pkt){
                this->setFrameType(pkt.type); // 设置帧类型
                this->setRtpHeader(channelID, pkt); // 生成头部

                if ( mediaChannelInfo_[channelID].isPlay ||
                        mediaChannelInfo_[channelID].isRecord ){
                    // TCP暂时不考虑
                    sendRtpOverUdp(channelID, pkt);
                }
            }, channelID, pkt));
    return 1;
}


void RtpConnection::play() {
    for (int i=0; i<2; i++) {
        if ( mediaChannelInfo_[i].isSetup){
            mediaChannelInfo_[i].isPlay = true;
        }
    }
}


void RtpConnection::record() {
    for (auto & i : mediaChannelInfo_) {
        if ( i.isSetup ){
            i.isPlay = true;
            i.isRecord = true;
        }
    }
}


void RtpConnection::teardown() {
    if ( !isClosed_ ){
        isClosed_ = true;
        for (auto & i : mediaChannelInfo_) {
            i.isPlay = false;
            i.isRecord = false;
        }
    }
}


bool RtpConnection::setUpRtpOverUdp(MediaChannel channelID, int rtp_port, int rtcp_port) {

    auto tcpConn = tcpConn_.lock();
    if ( tcpConn == nullptr ){
        return false;
    }
    socklen_t addrlen = sizeof(struct sockaddr_in);
    getpeername(tcpConn->getFd(), (sockaddr*)&peerAddr_, &addrlen);

    mediaChannelInfo_[channelID].rtpPort = rtp_port;
    mediaChannelInfo_[channelID].rtcpPort = rtcp_port;
    std::random_device rd;
    for(int n = 0; n <= 10; ++n) {
        if(n == 10)
            return false;

        localRtpPort_[channelID] = rd() & 0xfffe;
        localRtcpPort_[channelID] = localRtpPort_[channelID] + 1;

        /* 绑定RTP信道 */
        rtpfd_[channelID] = ::socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in rtpAddr = { 0 };
        rtpAddr.sin_family = AF_INET;
        rtpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        rtpAddr.sin_port = htons(localRtpPort_[channelID]);
        if(bind(rtpfd_[channelID], (struct sockaddr*)&rtpAddr, sizeof rtpAddr) == -1) {
            close(rtpfd_[channelID]);
            continue;
        }

        /* 绑定RTCP信道 */
        rtcpfd_[channelID] = ::socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in rtcpAddr = { 0 };
        rtcpAddr.sin_family = AF_INET;
        rtcpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        rtcpAddr.sin_port = htons(localRtcpPort_[channelID]);
        if(bind(rtcpfd_[channelID], (struct sockaddr*)&rtcpAddr, sizeof rtcpAddr) == -1) {
            close(rtcpfd_[channelID]);
            /* 如果绑定RTCP错误，则断开RTP的绑定 */
            close(rtpfd_[channelID]);
            continue;
        }
        break;
    }
    //设置发送缓冲区大小

    rtpPeerAddr_[channelID].sin_family = AF_INET;
    rtpPeerAddr_[channelID].sin_addr.s_addr = peerAddr_.sin_addr.s_addr;
    rtpPeerAddr_[channelID].sin_port = htons(mediaChannelInfo_[channelID].rtcpPort);

    rtcpPeerAddr_[channelID].sin_family = AF_INET;
    rtcpPeerAddr_[channelID].sin_addr.s_addr = peerAddr_.sin_addr.s_addr;
    rtcpPeerAddr_[channelID].sin_port = htons(mediaChannelInfo_[channelID].rtcpPort);

    mediaChannelInfo_[channelID].isSetup = true;
    transportType_ = RTP_OVER_UDP;

    return true;
}



