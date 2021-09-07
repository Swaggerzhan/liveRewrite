//
// Created by Swagger on 2021/9/5.
//

#include "media.h"
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <chrono>

using std::string;


Media* Media::CreateMedia(uint32_t frameRate) {
    return new Media(frameRate);
}


Media::Media(uint32_t frameRate)
:   frameRate_(frameRate),
    payload_(96),           // ?
    clockRate_(90000),      // ?
    mediaType_(H264),       // 媒体类型，96
    fd_(-1),
    sdp_()
{
//    fd_ = open(filename_.c_str(), O_RDONLY);
//    if (fd_ < 0 ){
//        std::cout << "open file Error!" << std::endl;
//        exit(1);
//    }
}

Media::~Media() {

}


string Media::getSdp() {
    if ( !sdp_.empty() ){
        return sdp_;
    }
    char* tmp = new char[4096];
    int len = getSdp(tmp, 4096);
    sdp_ = string(tmp, len); // 缓存！
    return sdp_;
}


int Media::getSdp(char *data, int len) {

    snprintf(data, len,
            "v=0\r\n"                 // version
            "o=- 9%ld 1 IN IP4 %s\r\n"      // origin
            "t=0 0\r\n"                     // 长连接
            "a=control:*\r\n"               // RTSP服务器
            "m=video 0 RTP/AVP 96\r\n"      // 视频信息
            "a=rtpmap:96 H264/90000\r\n"    // rtp map信息
            "a=control:track0\r\n",         // 这里应该是信道？
            time(nullptr), "127.0.0.1"      // IP地址，这里先写死固定
            // 后续IP地址可以从函数参数传入等等，上层定义MediaSession
            );
    return static_cast<int>(strlen(data));
}


void Media::setSendFrameCallBack(SendFrameCallBack cb) {
    cb_ = std::move(cb);
}


string Media::getMediaDescribe(int port) {
    char buf[64] { 0 };
    sprintf(buf,
            "m=video %u RTP/AVP %u",
            port, mediaType_
            );
    return string(buf);
}

uint32_t Media::getTimestamp() {
    auto timePoint = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
    return static_cast<uint32_t>((timePoint.time_since_epoch().count() + 500) / 1000 * 90);  // TODO
}


bool Media::handleFrame(MediaChannel channelId, AVFrame frame) {
    uint8_t* frameBuf = frame.buffer.get();
    uint32_t frameSize = frame.size;

    if(frame.timestamp == 0)
        frame.timestamp = getTimestamp();

    if(frameSize <= MAX_RTP_PAYLOAD_SIZE) { // 单一封包
        RtpPacket rtpPkt;
        rtpPkt.type = frame.type;
        rtpPkt.timestamp = frame.timestamp;
        rtpPkt.size = frame.size + 4 + RTP_HEADER_SIZE;  // RTP_OVER_TCP需要预留4个字节
        rtpPkt.last = 1;
        memcpy(rtpPkt.data.get() + 4 + RTP_HEADER_SIZE, frameBuf, frameSize);

        if(_sendFrameCallback) {
            if(!_sendFrameCallback(channelId, rtpPkt))
                return false;
        }
    }
    else {   // 分片封包
        char FU_A[2] = { 0 };
        FU_A[0] = (frameBuf[0] & 0xE0) | 28;    // FU indiactor: 保留前3位 + 28
        FU_A[1] = 0x80 | (frameBuf[0] & 0x1f);  // FU header: 开始标志 + 结束标志 + 0 + 保留后5位

        frameBuf += 1;
        frameSize -= 1;

        while(frameSize + 2 > MAX_RTP_PAYLOAD_SIZE) {
            RtpPacket rtpPkt;
            rtpPkt.type = frame.type;
            rtpPkt.timestamp = frame.timestamp;
            rtpPkt.size = 4 + RTP_HEADER_SIZE + MAX_RTP_PAYLOAD_SIZE;
            rtpPkt.last = 0;

            rtpPkt.data.get()[RTP_HEADER_SIZE+4] = FU_A[0];
            rtpPkt.data.get()[RTP_HEADER_SIZE+5] = FU_A[1];
            memcpy(rtpPkt.data.get()+4+RTP_HEADER_SIZE+2, frameBuf, MAX_RTP_PAYLOAD_SIZE-2);

            if(_sendFrameCallback) {
                if(!_sendFrameCallback(channelId, rtpPkt))
                    return false;
            }

            frameBuf += MAX_RTP_PAYLOAD_SIZE - 2;
            frameSize -= MAX_RTP_PAYLOAD_SIZE - 2;

            FU_A[1] &= ~0x80;   // 开始标志置0
        }

        {
            RtpPacket rtpPkt;
            rtpPkt.type = frame.type;
            rtpPkt.timestamp = frame.timestamp;
            rtpPkt.size = 4 + RTP_HEADER_SIZE + 2 + frameSize;
            rtpPkt.last = 1;

            FU_A[1] |= 0x40;   // 结束标志置1
            rtpPkt.data.get()[RTP_HEADER_SIZE+4] = FU_A[0];
            rtpPkt.data.get()[RTP_HEADER_SIZE+5] = FU_A[1];
            memcpy(rtpPkt.data.get()+4+RTP_HEADER_SIZE+2, frameBuf, frameSize);

            if(_sendFrameCallback) {
                if(!_sendFrameCallback(channelId, rtpPkt))
                    return false;
            }
        }
    }
    return true;
}








