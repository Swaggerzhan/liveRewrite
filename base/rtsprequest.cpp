//
// Created by Swagger on 2021/9/5.
//
#include <cstring>
#include "rtsprequest.h"

using std::string;

RtspRequest::RtspRequest() {

}


RtspRequest::~RtspRequest() {

}


bool RtspRequest::parseRequest(muduo::net::Buffer *buffer) {

    bool ret = true;
    while (true) {
        if ( state_ == ParseRequestLine ){
            const char* firstCRLF = buffer->findCRLF(); // 找到第一个\r\n

        }

    }
}


bool RtspRequest::parseRequestLine(char *start, char *end) {
    string message(start, end);
    char method[32] = { 0 };
    char url[256] = { 0 };
    char version[32] = { 0 };
    /* 得到对应信息，sscanf没有错误响应！ */
    if ( sscanf(message.c_str(), "%s %s %s", method, url, version) != 3){
        return false;
    }

    string methodString(method);
    if ( methodString == "OPTIONS" ){
        method_ = Option;
    }else if ( methodString == "DESCRIBE" ){
        method_ = Desc;
    }else if ( methodString == "SETUP" ){
        method_ = Setup;
    }else if ( methodString == "PLAY" ){
        method_ = Play;
    }else if ( methodString == "TEARDOWN" ){
        method_ = Teardown;
    }else {
        method_ = None;
    }

    if ( strncasecmp(url, "rtsp://", 7) != 0 ){
        return false;
    }
    url_ = string(url);

    if ( strncasecmp(version, "RTSP/1.0", 8) != 0 ){
        return false;
    }
    version_ = string(version);

    state_ = ParseRequestHeader; // 状态转换
    return true;
}


bool RtspRequest::parseHeader(char *start, char *end) {
    string message(start, end);
    if ( !parseCSeq(message) ){ // 解析错误
        return false; // 对于没有CSeq的请求，必定错误！
    }



}


bool RtspRequest::parseCSeq(string& message) {
    uint32_t CSeq;
    if (sscanf(message.c_str(), "%*[^:]: %u", &CSeq) != 1 ){
        return false;
    }
    headers_.emplace("CSeq", HeaderValue("", CSeq));
    return true;
}


void RtspRequest::reset() {
    method_ = None;
    state_ = ParseRequestLine;

}


int32_t RtspRequest::getCSeq() {
    auto iter = headers_.find("CSeq");
    if (iter != headers_.end()){
        return iter->second.second;
    }
    return 0;
}


int RtspRequest::buildOptionRes(char *data, int len) {
    memset(data, 0, len);
    snprintf(data, len,
             "RTSP/1.0 200 OK\r\n"
             "CSeq: %u\r\n"
             "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY\r\n"
             "\r\n",
             getCSeq()
             );
    return static_cast<int>(strlen(data));
}

int RtspRequest::buildDescribeRes(char *data, int len, const char *sdp) {
    snprintf(data, len,
            "RTSP/1.0 200 OK\r\n"
            "CSeq: %u\r\n"
            "Content-Base: %s\r\n" // 这里是rtsp协议地址！
            "Content-Type: application/sdp\r\n"
            "Content-length: %d\r\n\r\n"
            "%s", // 这里就是sdp了
            getCSeq(),
            url_.c_str(), // url
            static_cast<int>(strlen(sdp)),
            sdp
            );
    return static_cast<int>(strlen(data));
}






