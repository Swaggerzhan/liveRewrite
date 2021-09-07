//
// Created by Swagger on 2021/9/5.
//

#ifndef LIVEREWRITE_RTP_H
#define LIVEREWRITE_RTP_H

#include <iostream>
#include <memory>

/*
 *
 *    0                   1                   2                   3
 *    7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0|7 6 5 4 3 2 1 0
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |V=2|P|X|  CC   |M|     PT      |       sequence number         |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                           timestamp                           |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |           synchronization source (SSRC) identifier            |
 *   +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *   |            contributing source (CSRC) identifiers             |
 *   :                             ....                              :
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
struct RtpHeader{
    /* byte 0 */
    uint8_t csrcLen:4;      // CC
    uint8_t extension:1;    // X
    uint8_t padding:1;      // P
    uint8_t version:2;      // V

    /* byte 1 */
    uint8_t payloadType:7;  // PT
    uint8_t marker:1;       // M

    /* byte 2-3 */
    uint16_t seq;           // sequence number

    /* bytes 4-7 */
    uint32_t timestamp;

    /* bytes 8-11 */
    uint32_t ssrc;
};


//struct RtpPacket{
//    struct RtpHeader header;
//    uint8_t* payload;
//};

/* 区分传输方式，目前只支持UDP方式进行传输 */
enum TransportType{
    RTP_OVER_TCP,
    RTP_OVER_UDP,
};

/* for sdp */
enum MediaType{
    H264 = 96,
    None,
};

enum MediaChannel{
    channel_0,      // for video?
    channel_1,      // for audio?
};


struct AVFrame {
    std::shared_ptr<uint8_t> buffer;     // 帧数据
    uint32_t size;                       // 帧大小
    uint8_t type;                        // 帧类型
    uint32_t timestamp;                  // 时间戳

    AVFrame(uint32_t size = 0) :
            buffer(new uint8_t[size]) {
        this->size = size;
        type = 0;
        timestamp = 0;
    }
};


struct RtpPacket {
    /* RTP OVER TCP需要空出前面的4bytes */
    std::shared_ptr<uint8_t> data; // pkt数据，包括RtpHeader也在其中
    uint32_t size;
    uint32_t timestamp;
    uint8_t type;
    uint8_t last;

    RtpPacket() :
            data(new uint8_t[1600]) {
        size = 0;
        timestamp = 0;
        type = 0;
        last = 0;
    }
};

/* 维护每个MediaChannel的信息 */
struct MediaChannelInfo {
    RtpHeader rtpHeader;

    // tcp
    uint16_t rtpChannel;
    uint16_t rtcpChannel;

    // udp
    uint16_t rtpPort;
    uint16_t rtcpPort;
    uint16_t packetSeq;
    uint32_t clockRate;

    //rtcp
    uint64_t packetCount;
    uint64_t octetCount;
    uint64_t lastRtcpNtpTime;

    bool isSetup;
    bool isPlay;
    bool isRecord;
};


#define MAX_RTP_PAYLOAD_SIZE 1420
#define RTP_HEADER_SIZE         12





#endif //LIVEREWRITE_RTP_H
