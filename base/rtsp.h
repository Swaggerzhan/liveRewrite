//
// Created by Swagger on 2021/9/7.
//

#ifndef LIVEREWRITE_RTSP_H
#define LIVEREWRITE_RTSP_H

#include <iostream>

struct RtspUrlInfo{
    std::string     url;
    std::string     ip;
    int             port;
    std::string     suffix;
};


class Rtsp{
public:

    Rtsp();
    virtual ~Rtsp();

    virtual void setVersion(const std::string& version) {
        version_ = version;
    }
    virtual std::string getVersion() const {
        return version_;
    }

    virtual std::string getRtspUrl() const {
        return rtspUrlInfo_.url;
    }

    bool parseRtspUrl(const std::string& url) {
        char ip[16] = { 0 };
        char suffix[128] = { 0 };
        uint16_t port = 0;

        if(sscanf(url.c_str() + 7, "%[^:]:%hu/%s", ip, &port, suffix) == 3) {
            _rtspUrlInfo.port = port;
        }
        else if(sscanf(url.c_str() + 7, "%[^/]/%s", ip, suffix) == 2) {
            _rtspUrlInfo.port = 554;
        }
        else {
            return false;
        }

        rtspUrlInfo_.url = url;
        rtspUrlInfo_.ip = ip;
        rtspUrlInfo_.suffix = suffix;
        return true;
    }

    ///////////// ??
    virtual MediaSessionPtr lookMediaSession(const std::string& suffix) {
        return nullptr;
    }
    virtual MediaSessionPtr lookMediaSession(MediaSessionId sessionId) {
        return nullptr;
    }


private:

    RtspUrlInfo rtspUrlInfo_;   // 具体信息
    std::string version_;               // 版本

};



#endif //LIVEREWRITE_RTSP_H
