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
            if ( firstCRLF ){
                ret = parseRequestLine(buffer->peek(), firstCRLF);
                buffer->retrieveUntil(firstCRLF+2); // 指标移动到下一个新的开始
            }
            /* 解析请求头结束后state状态应该已经转移 */
            if ( state_ == ParseRequestHeader ){
                continue;
            }
            /* 状态没有转移成功，必定出错 */
            break;
        } else if ( state_ == ParseRequestHeader ){
            const char* lastCRLF = buffer->findLastCRLF();
            if ( lastCRLF ){
                ret = parseHeader(buffer->peek(), lastCRLF);
                buffer->retrieveUntil(lastCRLF + 2);
            }
            /* 没有找到CRLF则直接break返回，此时数据还未完整 */
            break;
        }else if ( state_ == Done ){
            buffer->retrieveAll();
            return true;
        }
    }
    return ret;
}


bool RtspRequest::parseRequestLine(const char *start, const char *end) {
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


bool RtspRequest::parseHeader(const char *start, const char *end) {
    string message(start, end);
    if ( !parseCSeq(message) ){ // 对于没有CSeq的请求，必定错误！
        /* 当再次重入时，如果已经解析了CSeq，则解析正常 */
        if ( headers_.find("CSeq") == headers_.end() )
            return false;
    }

    /* 对于Option，只需要拿到CSeq即可，不需要再向下请求了 */
    if ( method_ == Option ){
        state_ = Done;
        return true;
    }

    /* 对于Describe请求，需要判断所接受的是不是sdp数据 */
    if ( method_ == Desc ){
        /* 解析完Accept，Describe就基本结束 */
        if (parseAccept(message)){
            state_ = Done;
            return true;
        }
    }

    /* 对于Setup请求，就需要解析Transport */
    if ( method_ == Setup ){
        if ( parseTransport(message) ){
            state_ = Done; // 拿到Transport方式，setup解析就可以就此结束了
        }
        return true;
    }

    /* 对于Play请求，其中重要的为SessionID */
    if ( method_ == Play ){
        if ( parseSessionID(message) ){
            state_ = Done;
        }
        return true;
    }
    return true;
}


bool RtspRequest::parseAccept(std::string &message) {
    /* 对于只要少了Accept关键字和sdp关键字，则客户端的请求无法接受，即
     * 服务器只接受sdp格式的协议 */
    if ( message.find("Accept") == string::npos ||
        message.find("sdp") == string::npos
    ){
        return false;
    }
    return true;
}


bool RtspRequest::parseCSeq(string& message) {
    uint32_t CSeq;
    if (sscanf(message.c_str(), "%*[^:]: %u", &CSeq) != 1 ){
            return false;
    }
    headers_.emplace("CSeq", HeaderValue("", CSeq));
    return true;
}


bool RtspRequest::parseTransport(std::string &message) {
    if ( message.find("Transport") == string::npos ){ // 没有找到Transport字段
        return false;
    }

    if ( message.find("RTP/AVP/TCP") != string::npos ){
        transportType_ = RTP_OVER_TCP; // 暂时不实现
    }else if ( message.find("RTP/AVP") != string::npos ){
        transportType_ = RTP_OVER_UDP; // 默认的传输方式
        //TODO:单播与多播，实现多播。。。
        size_t pos;
        if ( (pos = message.find("client_port=")) != string::npos ){
            int rtp;
            int rtcp;
            if ( sscanf(message.c_str()+pos, "client_port=%d-%d", &rtp, &rtcp) != 2){
                return false;
            }
#ifdef DEBUG
            std::cout << "got rtp over port" << rtp << " " << rtcp << std::endl;
#endif
            headers_.emplace("RTP_CHANNEL", HeaderValue("", rtp));
            headers_.emplace("RTCP_CHANNEL", HeaderValue("", rtcp));
            return true;
        }
        /* udp情况没有找到port也为错误 */
        return false;
    }else {
        return false;
    }
}


bool RtspRequest::parseSessionID(std::string &message) {
    size_t pos;
    if ( (pos = message.find("Session")) == string::npos ){
        return false;
    }
    uint32_t sessionID;
    if ( sscanf(message.c_str() + pos, "Session: %d", &sessionID) != 1 ){
        return false;
    }
    headers_.emplace("Session", HeaderValue("", sessionID));
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






