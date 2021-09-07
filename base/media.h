//
// Created by Swagger on 2021/9/5.
//

#ifndef LIVEREWRITE_MEDIA_H
#define LIVEREWRITE_MEDIA_H

#include <iostream>
#include <functional>
#include "rtp.h"

// 这里media只做比较简单的做法，直接从h264中提取nalu单元
class Media{
public:

    typedef std::function<bool(MediaChannel channelID, RtpPacket pkt)> SendFrameCallBack;

    static Media* CreateMedia(uint32_t frameRate);
    ~Media();


    /* 封装好的 */
    std::string getSdp();
    static uint32_t getTimestamp();
    std::string getMediaDescribe(int port);
    bool handleFrame(MediaChannel channelID, AVFrame frame);
    void setSendFrameCallBack(SendFrameCallBack cb);

private:

    Media(uint32_t frameRate);

    /**
     * 此函数暂时作模仿使用
     * @param data 空缓冲区
     * @param len 长度
     * @return 字符串长度
     */
    int getSdp(char* data, int len);

private:
    std::string filename_;
    int fd_;                // 文件指针

    std::string sdp_;       // 缓存

    uint32_t frameRate_;    // 帧率
    uint32_t payload_;      // ?
    uint32_t clockRate_;    // ?
    MediaType mediaType_;   // 媒体类型
    SendFrameCallBack cb_;  // 解析完成后的回调函数

};











#endif //LIVEREWRITE_MEDIA_H
