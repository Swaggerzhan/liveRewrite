//
// Created by Swagger on 2021/9/5.
//

#include "media.h"
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

using std::string;

Media::Media(string filename)
:   filename_(std::move(filename)),
    fd_(-1),
    sdp_()
{
    fd_ = open(filename_.c_str(), O_RDONLY);
    if (fd_ < 0 ){
        std::cout << "open file Error!" << std::endl;
        exit(1);
    }
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








